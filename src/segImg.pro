######################################################################
# Automatically generated by qmake (3.0) ? 1? 25 11:02:33 2018
######################################################################

TEMPLATE = app
TARGET = segImg
INCLUDEPATH += .

# Input
HEADERS += disjoint-set.h \
           main.h \
           mainwindow.h \
           SeedsRevised.h \
           segmentimage.h \
           Tools.h
SOURCES += main.cpp mainwindow.cpp SeedsRevised.cpp segmentimage.cpp Tools.cpp

QT += widgets
CONFIG += c++11
QT_CONFIG -= no-pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv