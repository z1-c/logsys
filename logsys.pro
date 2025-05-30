QT += core sql
CONFIG += c++11
TARGET = logsys
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/dbmanager.cpp \
    src/userauth.cpp \
    src/logupload.cpp \
    src/audit.cpp \
    src/MainThreadObject.cpp\  # 确保包含 MainThreadObject.cpp
    src/UserOperation.cpp

HEADERS += \
    include/dbmanager.h \
    include/userauth.h \
    include/logupload.h \
    include/audit.h \
    include/MainThreadObject.h\  # 确保包含 MainThreadObject.h
    include/UserOperation.h
    

INCLUDEPATH += include
