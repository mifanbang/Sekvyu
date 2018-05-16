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

#include <windows.h>

#include <QApplication>



namespace {


bool EnablePrivilege(const wchar_t* name)
{
	HANDLE handle;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle) == FALSE)
		return false;
	std::unique_ptr<void, decltype(&CloseHandle)> hToken(handle, CloseHandle);

	LUID luid;
	if (LookupPrivilegeValue(nullptr, name, &luid) == FALSE)  // SE_INC_WORKING_SET_NAME
		return false;

	TOKEN_PRIVILEGES tokenPriv;
	ZeroMemory(&tokenPriv, sizeof(tokenPriv));
	tokenPriv.PrivilegeCount = 1;
	tokenPriv.Privileges[0].Luid = luid;
	tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	return AdjustTokenPrivileges(hToken.get(), false, &tokenPriv, sizeof(tokenPriv), nullptr, nullptr) != FALSE;
}


}  // unnamed namespace



int main(int argc, char* argv[])
{
	EnablePrivilege(SE_INC_WORKING_SET_NAME);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
