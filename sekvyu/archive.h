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

#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <algorithm>
#include <memory>

#include <QString>

// Extract7Z
#include <BufferedFile.h>
#include <Password.h>



class Archive
{
public:
	enum class OpenResult
	{
		Success,
		DllNotFound,
		ExtractionError
	};


	Archive();

	OpenResult Open(const QString& path, Password& password);

	inline FileArchive& GetContent()		{ return const_cast<FileArchive&>(reinterpret_cast<const Archive*>(this)->GetContent()); }
	const FileArchive& GetContent() const   { return *m_content; }
	inline size_t GetFileCount() const	  { return m_content->size(); }
	QString GetName() const;
	QString GetPath() const;

	template <typename FuncType>
	void Filter(const FuncType& func)
	{
		if (m_content->size() == 0)
			return;

		FileArchive filteredFiles;
		std::copy_if(m_content->cbegin(), m_content->cend(), std::back_inserter(filteredFiles), func);
		*m_content = std::move(filteredFiles);
	}


private:
	QString m_name;
	QString m_path;
	std::shared_ptr<FileArchive> m_content;
};



#endif // ARCHIVE_H
