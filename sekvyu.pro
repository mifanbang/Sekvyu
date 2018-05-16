
QT += core gui widgets
TARGET = sekvyu
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000


# build configuration-specific
CONFIG(debug, debug|release) {
    MY_BUILD_CONFIG = debug
    CONFIG += console
}
CONFIG(release, debug|release) {
    MY_BUILD_CONFIG = release
    CONFIG += windows
}
# platform-specific
contains(QMAKE_TARGET.arch, x86_64) {
    MY_BUILD_ARCH = x64
}
else {
    MY_BUILD_ARCH = win32
}


# headers/libraries
INCLUDEPATH += "./extract7z/include/"
LIBS += OleAut32.lib User32.lib Advapi32.lib "extract7z/bin/$${MY_BUILD_ARCH}/$${MY_BUILD_CONFIG}/extract7z.lib"

# output/intermediate folders
DESTDIR = build/bin/$${MY_BUILD_CONFIG}
MOC_DIR = build/moc/$${MY_BUILD_CONFIG}
OBJECTS_DIR = build/obj/$${MY_BUILD_CONFIG}
RCC_DIR = build/rcc/$${MY_BUILD_CONFIG}
UI_DIR = build/ui/
PRECOMPILED_DIR = build  # prevent qmake from auto-generating debug and release

# pre-build events
extract7z.target = extract7z/bin/$${MY_BUILD_ARCH}/$${MY_BUILD_CONFIG}/extract7z.lib
extract7z.commands = msbuild extract7z\extract7z.vcxproj /p:Configuration=$${MY_BUILD_CONFIG};Platform=$${MY_BUILD_ARCH}
extract7z.depends = FORCE
git_header.target = sekvyu/git.h
git_header.commands = gen_git_header.bat
git_header.depends = FORCE
PRE_TARGETDEPS += extract7z/bin/$${MY_BUILD_ARCH}/$${MY_BUILD_CONFIG}/extract7z.lib sekvyu/git.h
QMAKE_EXTRA_TARGETS += extract7z git_header


SOURCES += \
    sekvyu/main.cpp \
    sekvyu/mainwindow.cpp \
    sekvyu/fileformat.cpp \
    sekvyu/archiveimageview.cpp \
    sekvyu/archive.cpp

HEADERS += \
    sekvyu/mainwindow.h \
    sekvyu/fileformat.h \
    sekvyu/archiveimageview.h \
    sekvyu/archive.h

FORMS += \
        sekvyu/mainwindow.ui

DISTFILES +=

RESOURCES += \
    resource/sekvyu.qrc

RC_FILE = resource/app.rc

