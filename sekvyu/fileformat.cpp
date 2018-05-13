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

#include "fileformat.h"



// all assume LITTLE-endian
namespace {


inline bool CheckUint32(const uint8_t* data, uint32_t value)
{
	return *reinterpret_cast<const uint32_t*>(data) == value;
}

inline bool CheckUint16(const uint8_t* data, uint16_t value)
{
	return *reinterpret_cast<const uint16_t*>(data) == value;
}


bool IsJpeg(const uint8_t* rawData, size_t size)
{
	if (size > 4 && CheckUint32(rawData, 0xDBFFD8FF))
		return true;
	if (size > 12 && CheckUint32(rawData, 0xE0FFD8FF) && CheckUint16(rawData+6, 0x464A) && CheckUint32(rawData+8, 0x01004649))
		return true;
	if (size > 12 && CheckUint32(rawData, 0xE1FFD8FF) && CheckUint16(rawData+6, 0x7845) && CheckUint32(rawData+8, 0x00006669))
		return true;
	return false;
}

bool IsPng(const uint8_t* rawData, size_t size)
{
	return (size > 8 && CheckUint32(rawData, 0x474E5089) && CheckUint32(rawData+4, 0x0A1A0A0D));
}

bool IsSevenZip(const uint8_t* rawData, size_t size)
{
	return (size > 6 && CheckUint32(rawData, 0xAFBC7A37) && CheckUint16(rawData+4, 0x1C27));
}


}  // unnamed namespace



// REF: https://en.wikipedia.org/wiki/List_of_file_signatures
FileFormat::Type FileFormat::GetType(const uint8_t* rawData, size_t size)
{
#define CHECK(type)	 { if (Is##type##(rawData, size)) return Type::##type##; }
	CHECK(Jpeg);
	CHECK(Png);
	CHECK(SevenZip);
#undef CHECK

	return Type::Unknown;
}

