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

#define NOMINMAX

#include "archive.h"

#include <windows.h>

#include <QFileInfo>

#include <Extractor7Z.h>
#include <Buffer.h>



namespace {


bool SetWorkingSetSizeHint(size_t sizeHint)
{
	HANDLE hProc = GetCurrentProcess();
	size_t minSize, maxSize;
	GetProcessWorkingSetSize(hProc, &minSize, &maxSize);

	if (minSize < sizeHint)
		return SetProcessWorkingSetSize(hProc, sizeHint, std::max(sizeHint, maxSize)) != FALSE;
	else
		return true;  // no need to expand
}


}  // unnamed namespace



Archive::Archive()
	: m_name()
	, m_path()
	, m_content(new FileArchive)  // always allocate an empty one
	, m_lastSavedDir()
{
}


Archive::OpenResult Archive::Open(const QString& path, Password& password)
{
	if (path == m_path)
		return OpenResult::Success;
	else if (!Extractor7Z::CheckLibrary())
		return OpenResult::DllNotFound;

	auto filePath = reinterpret_cast<const wchar_t*>(path.utf16());
	size_t uncompressedSize;
	Extractor7Z::GetUncompressedSize(filePath, &password, uncompressedSize);
	SetWorkingSetSizeHint(uncompressedSize + 1024 * 1024 * 100);  // +100MB for all other things in the process

	Extractor7Z::ExtractOptions options;
	options.passwd = &password;
	options.isSecrecy = true;
	auto newArchive = Extractor7Z::ExtractFrom(filePath, options);
	if (!newArchive)
		return OpenResult::ExtractionError;

	const auto& fileInfo = QFileInfo(path);
	m_name = fileInfo.fileName();
	m_path = path;
	m_content = newArchive;
	m_lastSavedDir = fileInfo.absolutePath() + "/";

	return OpenResult::Success;
}


bool Archive::Save(size_t index, const QString& path)
{
	if (index >= m_content->size())
		return false;

	const auto& fileBuffer = m_content->at(index).data;
	QFile ourFile(path);
	if (!ourFile.open(QIODevice::WriteOnly))
		return false;
	auto sizeWritten = ourFile.write(reinterpret_cast<const char*>(fileBuffer->GetData()), fileBuffer->GetSize());
	ourFile.close();

	m_lastSavedDir = QFileInfo(ourFile).absolutePath() + "/";
	return sizeWritten == fileBuffer->GetSize();
}


QString Archive::GetName() const
{
	return m_name;
}

QString Archive::GetPath() const
{
	return m_path;
}

QString Archive::GetLastSavedDir() const
{
	return m_lastSavedDir;
}

