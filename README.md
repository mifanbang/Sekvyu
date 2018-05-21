# Sekvyu
A viewer for your secret images.

## Download

Pre-built executables can be found in the [Releases page](https://github.com/mifanbang/Sekvyu/releases). They require no installation.

## Features

- Made to work with 7z archives.
- Support of encrypted archives.
- Images inside an archive are extracted only to the memory and therefore, with a good chance, leave no trace on your disk\*.

\*: The operating system may move data from RAM to disk to free up some of the RAM space (see [Paging](https://en.wikipedia.org/wiki/Paging)). Sekvyu calls Windows API `VirtualLock()` to request keeping crucial memory from being swapped out, but it's up to Windows to decide, based on many runtime factors, whether to comply. Thus this software cannot guarantee that your decompressed data will never be written onto the disk.

## Build Instructions

There are some prerequisites for building from the source code:

- Visual Studio 2017
- [Qt 5](https://www.qt.io/download) (>= 5.10 recommended)
- The latest package of "7-zip Source code" or "LZMA SDK" from [this page](https://www.7-zip.org/download.html)

After you have them, it's pretty simple:

1. Extract either "7-zip Source code" or "LZMA SDK" into the folder `extract7z/external/7zip/` so that the following path can be accessed: `extract7z/external/7zip/CPP/`.
2. Open `sekvyu.pro` with Qt Designer and build with mkspec `win32-msvc`.


## Copyright

Copyright (C) 2018 Mifan Bang <https://debug.tw>.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the [GNU General Public License](LICENSE) for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.

Sekvyu includes a part of source code from 7-Zip written by [Igor Pavlov](https://www.7-zip.org) which is licensed under the GNU Lesser General Public License version 2.1 or later. For further copyright information of 7Zip, please read https://www.7-zip.org/license.txt.
