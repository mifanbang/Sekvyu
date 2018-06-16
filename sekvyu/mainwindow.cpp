/*
 *  This file is a part of Sekvyu, a 7z archive image viewer.
 *  Copyright (C) 2018 Mifan Bang <https://debug.tw>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"

#include <algorithm>
#include <array>
#include <limits>

#include <QCoreApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>
#include <QSpacerItem>
#include <QString>

// Extract7Z
#include <Buffer.h>
#include <Password.h>

#include "fileformat.h"
#include "git.h"
#include "ui_mainwindow.h"



namespace {


template <typename T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi)
{
	return std::max(std::min(v, hi), lo);
}


QString GetArchiveOpenErrorMsg(Archive::OpenResult result)
{
	if (result == Archive::OpenResult::DllNotFound)
		return "Failed to load 7z.dll.";
	else if (result == Archive::OpenResult::ExtractionError)
		return "Failed to open the archive.";
	else
		return "Unknown error.";
}


bool TranslateToNavigationKey(int key, ArchiveImageView::Rotation& rotation)
{
	static const std::array<std::pair<int, ArchiveImageView::Rotation>, 6> navKeys = {{
		{ Qt::Key_Left,	 ArchiveImageView::Rotation::Previous },
		{ Qt::Key_PageUp,   ArchiveImageView::Rotation::Previous },
		{ Qt::Key_Right,	ArchiveImageView::Rotation::Next },
		{ Qt::Key_PageDown, ArchiveImageView::Rotation::Next },
		{ Qt::Key_Home,	 ArchiveImageView::Rotation::First },
		{ Qt::Key_End,	  ArchiveImageView::Rotation::Last }
	}};

	return std::any_of(navKeys.cbegin(), navKeys.cend(), [key, &rotation](const auto& mapping) -> bool {
		if (key == mapping.first) {
			rotation = mapping.second;
			return true;
		}
		return false;
	} );
}


QSettings& GetSettings()
{
	static QSettings settings("debug", "Sekvyu");
	return settings;
}


}  // unnamed namespace



MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_ui(new Ui::MainWindow)
	, m_flagFirstTimeShown(true)
	, m_archive()
{
	m_ui->setupUi(this);

	auto& settings = GetSettings();
	restoreGeometry(settings.value("geometry").toByteArray());

	QObject::connect(this, &MainWindow::sigOpenFile, this, &MainWindow::loadArchive);
}

MainWindow::~MainWindow()
{
	delete m_ui;
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
	QWidget::keyPressEvent(event);

	ArchiveImageView::Rotation rotation;
	if (TranslateToNavigationKey(event->key(), rotation)) {
		m_ui->imageView->rotate(rotation);
		refreshWindowTitle();
	}
}


void MainWindow::paintEvent(QPaintEvent*)
{
	if (!m_flagFirstTimeShown)
		return;
	m_flagFirstTimeShown = false;

	const auto& argv = QCoreApplication::arguments();
	if (argv.size() <= 1)
		askOpenFile();
	else
		emit sigOpenFile(argv[1]);
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const auto& itemText = event->mimeData()->text();
	QFile file(itemText.mid(8));  // skip "file:///"
	QByteArray firstLine;  // = 16 byte

	bool isFileDragged = itemText.startsWith("file://");
	bool isFileReadable = isFileDragged && file.open(QIODevice::ReadOnly);
	bool isFirstLineRead = isFileReadable && (firstLine = std::move(file.read(16))).size() == 16;
	bool has7ZipSig = isFirstLineRead && FileFormat::GetType(reinterpret_cast<const uint8_t*>(firstLine.data()), 16) == FileFormat::Type::SevenZip;

	if (has7ZipSig)
		event->acceptProposedAction();
}


void MainWindow::dropEvent(QDropEvent* event)
{
	const auto& filePath = event->mimeData()->text().mid(8);
	if (filePath.length() > 0)
		emit sigOpenFile(filePath);
}


void MainWindow::closeEvent(QCloseEvent* event)
{
	auto& settings = GetSettings();
	settings.setValue("geometry", saveGeometry());
	QMainWindow::closeEvent(event);
}


void MainWindow::askOpenFile()
{
	QString currDir = m_archive ? QFileInfo(m_archive->GetPath()).dir().absolutePath() : QString();
	auto filePath = QFileDialog::getOpenFileName(this, "Open Archive", currDir, "7z Archives (*.7z *.cb7)");
	if (filePath.length() > 0)
		emit sigOpenFile(filePath);
}


void MainWindow::askImageIndex()
{
	if (!m_archive || m_archive->GetFileCount() == 0)
		return;

	int numImage = Clamp(static_cast<int>(m_archive->GetFileCount()), 0, std::numeric_limits<int>::max());
	int currIndex = Clamp(static_cast<int>(m_ui->imageView->getCurrentIndex() + 1), 0, std::numeric_limits<int>::max());

	bool isOkPressed;
	auto index = QInputDialog::getInt(
		this,
		"Go To...",
		QString("Image Index (1-%1)").arg(numImage),
		currIndex,
		1,  // start
		numImage,
		1,  // step
		&isOkPressed
	);
	if (!isOkPressed)
		return;

	m_ui->imageView->setIndex(index - 1);
	refreshWindowTitle();
}


void MainWindow::loadArchive(const QString& filePath)
{
	if (m_archive && m_archive->GetPath() == filePath)
		return;

	// extraction
	Password password( [this](std::wstring& outPasswd) -> bool {
		auto passwd = QInputDialog::getText(this, "Password", "Enter Password", QLineEdit::Password);
		outPasswd = reinterpret_cast<const wchar_t*>(passwd.utf16());
		return true;  // never ask again
	} );
	auto newArchive = std::make_shared<Archive>();
	auto result = newArchive->Open(filePath, password);
	if (result != Archive::OpenResult::Success) {
		showError(GetArchiveOpenErrorMsg(result));
		return;
	}

	// filter based on file type
	newArchive->Filter( [](const FileRecord& frec) -> bool {
		auto fileSize = frec.data->GetSize();
		if (fileSize == 0)
			return false;
		auto fileType = FileFormat::GetType(frec.data->GetData(), fileSize);
		return fileType == FileFormat::Type::Jpeg || fileType == FileFormat::Type::Png;
	} );
	if (newArchive->GetFileCount() == 0) {
		showError("The archive does not contain any valid image.");
		return;
	}

	m_archive = std::move(newArchive);
	m_ui->imageView->setArchive(m_archive);
	m_ui->actionSaveImageAs->setEnabled(true);
	m_ui->actionGoTo->setEnabled(true);

	refreshWindowTitle();
}


void MainWindow::showImgCtxMenu(const QPoint&)
{
	if (!m_archive || m_archive->GetFileCount() == 0)
		return;

	QMenu contextMenu(this);
	contextMenu.addAction(m_ui->actionSaveImageAs);
	contextMenu.exec(QCursor::pos());
}


bool MainWindow::saveCurrentImg()
{
	if (!m_archive || m_archive->GetFileCount() == 0)
		return false;

	auto currIndex = m_ui->imageView->getCurrentIndex();
	const auto& fileRecord = m_archive->GetContent().at(currIndex);

	auto&& inArchiveName = QString::fromUtf16(reinterpret_cast<const ushort*>(fileRecord.name.c_str()));
	auto&& defaultName = QFileInfo(inArchiveName).fileName();
	auto&& filePath = QFileDialog::getSaveFileName(this, "Save Image", m_archive->GetLastSavedDir() + defaultName);
	if (filePath.length() == 0)
		return false;

	return m_archive->Save(currIndex, filePath);
}


void MainWindow::showAbout()
{
#ifdef _M_X64
	#define ARCH	"64"
#else
	#define ARCH	"32"
#endif  // _M_X64
	static const QString title = "About Sekvyu";
	static const QString text = QString(
		"<b>Sekvyu</b> - A 7z archive image viewer<br>"
		"Built from revision %1 (" ARCH "bit) <br><br>"
		"Copyright &copy; 2018 Mifan Bang.<br><br>"
		"This software is licensed under the GNU General Public License version 3 or any of its later version.<br><br>"
		"This software includes source code and uses binary component written by <a href=https://www.7-zip.org>Igor Pavlov</a> under the GNU Lesser General Public License version 2.1 or a later version.<br><br>"
		"Github: <a href=https://github.com/mifanbang/sekvyu>mifanbang/sekvyu</a><br>"
		"Website: <a href=https://debug.tw>debug.tw</a><br>"
	).arg(QString(GIT_HEAD_REV).left(7));
#undef ARCH

	QMessageBox about(QMessageBox::NoIcon, title, text, QMessageBox::Close, this);
	about.setIconPixmap(QPixmap(":/app_logo.png"));
	QGridLayout* layout = qobject_cast<QGridLayout*>(about.layout());
	if (layout != nullptr) {
		QSpacerItem* horizontalSpacer = new QSpacerItem(550, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
	}
	about.exec();
}


void MainWindow::refreshWindowTitle()
{
	if (!m_archive)
		return;

	auto currIndex = m_ui->imageView->getCurrentIndex();
	const auto& fileRecord = m_archive->GetContent().at(currIndex);
	const auto fileName = reinterpret_cast<const ushort*>(fileRecord.name.c_str());
	const auto& title = QString("%1 [%2] (%3/%4) - Sekvyu")
		.arg(m_archive->GetName())
		.arg(fileName)
		.arg(currIndex + 1)
		.arg(m_archive->GetFileCount());
	setWindowTitle(title);
}


void MainWindow::showError(const QString &msg)
{
	QMessageBox::critical(this, "Error", msg);
}

