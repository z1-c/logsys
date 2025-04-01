QT += core gui sql
CONFIG += c++11
TARGET = logsys
TEMPLATE = app

SOURCES += src/main.cpp \
           src/dbmanager.cpp \
           src/logupload.cpp \
           src/userauth.cpp

HEADERS += include/dbmanager.h \
           include/logupload.h \
           include/userauth.h

INCLUDEPATH += include

