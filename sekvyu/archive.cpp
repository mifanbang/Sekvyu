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

#include "archive.h"

#include <QFileInfo>

#include <Extractor7Z.h>



Archive::Archive()
	: m_name()
	, m_path()
	, m_content(new FileArchive)  // always allocate an empty one
{
}


Archive::OpenResult Archive::Open(const QString& path, Password& password)
{
	if (path == m_path)
		return OpenResult::Success;
	else if (!Extractor7Z::CheckLibrary())
		return OpenResult::DllNotFound;

	auto newArchive = Extractor7Z::ExtractFrom(reinterpret_cast<const wchar_t*>(path.utf16()), password);
	if (!newArchive)
		return OpenResult::ExtractionError;

	m_name = QFileInfo(path).fileName();
	m_path = path;
	m_content = newArchive;

	return OpenResult::Success;
}


QString Archive::GetName() const
{
	return m_name;
}

QString Archive::GetPath() const
{
	return m_path;
}


