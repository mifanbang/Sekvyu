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

#ifndef ARCHIVEIMAGEVIEW_H
#define ARCHIVEIMAGEVIEW_H

#include <cstdint>
#include <memory>

#include <QLabel>
#include <QPixmap>

#include "archive.h"



class ArchiveImageView : public QLabel
{
	Q_OBJECT

public:
	enum class Rotation
	{
		First,
		Last,
		Previous,
		Next
	};


	explicit ArchiveImageView(QWidget* parent);

	bool setArchive(std::shared_ptr<Archive>& archive);
	inline size_t getCurrentIndex() const   { return m_index; }


signals:
public slots:
	void rotate(Rotation target);
	void setIndex(size_t index);


private:
	void loadCurrPixmapFromArchive();
	void refreshView();

	virtual void resizeEvent(QResizeEvent*) override;


	std::shared_ptr<Archive> m_archive;
	size_t m_index;
	QPixmap m_currPixmap;  // img data before transform
};



#endif // ARCHIVEIMAGEVIEW_H

