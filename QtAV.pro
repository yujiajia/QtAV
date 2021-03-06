include(root.pri)

TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS = libqtav examples tests

greaterThan(QT_MAJOR_VERSION, 4) {
  qtHaveModule(quick) {
    SUBDIRS += libqmlav
    libqmlav.file = qml/libQmlAV.pro
    libqmlav.depends += libqtav
    examples.depends += libqmlav
  }
}
libqtav.file = src/libQtAV.pro
examples.depends += libqtav
tests.depends += libqtav

OTHER_FILES += README.md TODO.txt Changelog
OTHER_FILES += templates/vo.h templates/vo.cpp templates/COPYRIGHT.h templates/mkclass.sh
OTHER_FILES += \
	templates/base.h templates/base.cpp templates/base_p.h \
	templates/derived.h templates/derived.cpp templates/derived_p.h \
	templates/final.h templates/final.cpp
#OTHER_FILES += config.test/mktest.sh


EssentialDepends = avutil avcodec avformat swscale
OptionalDepends = \
    swresample \
    avresample \
    gl
# no-xxx can set in $$PWD/user.conf
!no-openal: OptionalDepends *= openal
!no-portaudio: OptionalDepends *= portaudio
!no-direct2d: OptionalDepends *= direct2d
!no-gdiplus: OptionalDepends *= gdiplus
win32 {
    !no-dxva: OptionalDepends *= dxva
}
unix {
    !no-xv: OptionalDepends *= xv
    !no-vaapi: OptionalDepends *= vaapi
}

runConfigTests()

PACKAGE_VERSION = 1.3.0
PACKAGE_NAME= QtAV

include(pack.pri)
#packageSet(1.3.0, QtAV)
