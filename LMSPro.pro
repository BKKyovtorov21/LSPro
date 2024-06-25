QT       += core gui sql pdf quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/databasemanager.cpp \
    src/login.cpp \
    src/main.cpp \
    src/mainmenu.cpp \
    src/mainwindow.cpp \
    src/register.cpp \
    src/requestaccess.cpp \
    src/sha256.cpp

HEADERS += \
    src/databasemanager.hpp \
    src/login.hpp \
    src/mainmenu.hpp \
    src/mainwindow.hpp \
    src/register.hpp \
    src/requestaccess.hpp \
    src/sha256.hpp

FORMS += \
    src/login.ui \
    src/mainmenu.ui \
    src/mainwindow.ui \
    src/register.ui \
    src/requestaccess.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/resources.qrc
