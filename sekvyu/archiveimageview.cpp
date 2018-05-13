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

#include "archiveimageview.h"

// Extract7Z
#include <Buffer.h>



ArchiveImageView::ArchiveImageView(QWidget *parent)
	: QLabel(parent)
	, m_archive()
	, m_index(0)
	, m_currPixmap()
{

}


bool ArchiveImageView::setArchive(std::shared_ptr<Archive>& archive)
{
	if (!archive || archive->GetFileCount() == 0)
		return false;

	m_archive = archive;
	m_index = 0;
	loadCurrPixmapFromArchive();
	refreshView();

	return true;
}


void ArchiveImageView::rotate(Rotation target)
{
	size_t fileCount;
	if (!m_archive || (fileCount = m_archive->GetFileCount()) == 0)
		return;

	if (target == Rotation::Previous && m_index > 0)
		--m_index;
	else if (target == Rotation::Next && m_index < fileCount - 1)
		++m_index;
	else if (target == Rotation::First && m_index > 0)
		m_index = 0;
	else if (target == Rotation::Last && m_index < fileCount - 1)
		m_index = fileCount - 1;
	else
		return;

	loadCurrPixmapFromArchive();
	refreshView();
}


void ArchiveImageView::setIndex(size_t index)
{
	size_t fileCount;
	bool shouldSkip =
		!m_archive ||
		index == m_index ||
		index >= (fileCount = m_archive->GetFileCount()) ||
		fileCount == 0
	;
	if (shouldSkip)
		return;

	m_index = index;
	loadCurrPixmapFromArchive();
	refreshView();
}


void ArchiveImageView::loadCurrPixmapFromArchive()
{
	if (!m_archive || m_archive->GetFileCount() == 0)
		return;

	auto imgData = m_archive->GetContent().at(m_index).data;
	m_currPixmap.loadFromData( imgData->GetData(), static_cast<uint>(imgData->GetSize()) );
}


void ArchiveImageView::refreshView()
{
	if (m_currPixmap.isNull())
		return;

	setPixmap(m_currPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));  // fit to current size
}


void ArchiveImageView::resizeEvent(QResizeEvent*)
{
	refreshView();
}

