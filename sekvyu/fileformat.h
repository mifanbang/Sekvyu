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

#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <cstdint>



class FileFormat {
public:
	enum class Type {
		// images
		Jpeg,
		Png,

		// archives
		SevenZip,

		Unknown
	};

	static Type GetType(const uint8_t* rawData, size_t size);
};



#endif // FILEFORMAT_H

