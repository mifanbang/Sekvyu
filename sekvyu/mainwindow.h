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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QPixmap>

#include "archive.h"
#include "archiveimageview.h"


namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();


signals:
	void sigOpenFile(QString path);


public slots:
	void askOpenFile();
	void askImageIndex();
	void loadArchive(const QString& filePath);
	void showImgCtxMenu(const QPoint& cursorPos);
	bool saveCurrentImg();
	void showAbout();


private:
	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
	virtual void closeEvent(QCloseEvent* event) override;

	void refreshWindowTitle();
	void showError(const QString& msg);


	Ui::MainWindow* m_ui;
	bool m_flagFirstTimeShown;
	std::shared_ptr<Archive> m_archive;
};



#endif // MAINWINDOW_H

