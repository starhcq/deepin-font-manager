######################################################################
# Automatically generated by qmake (3.0) ?? 12? 20 11:40:37 2017
######################################################################

QT += core gui svg sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = deepin-font-manager

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget freetype2 fontconfig

INCLUDEPATH += $$PWD/../libdeepin-font-manager \
               $$PWD/interfaces
LIBS += -L$$OUT_PWD/../libdeepin-font-manager -ldeepin-font-manager
DEFINES += QT_MESSAGELOGCONTEXT

# Input
HEADERS += homepage.h \
           listview.h \
	   listitem.h \
	   mainwindow.h \
	   multifilepage.h \
	   singlefilepage.h \
	   utils.h \
	   progress.h \
	   refreshthread.h \
    views/dfontmgrmainwindow.h \
    globaldef.h \
    interfaces/dfontmenumanager.h \
    interfaces/dfontpreviewitemdelegate.h \
    interfaces/dfontpreviewproxymodel.h \
    interfaces/dfontpreviewlistview.h \
    views/dfinstallnormalwindow.h \
    views/dfinstallerroritemwidget.h \
    views/dfinstallerrordialog.h \
    views/dsplitlistwidget.h \
    views/dfontinfodialog.h \
    views/dfquickinstallwindow.h \
    views/dfdeletedialog.h \

SOURCES += homepage.cpp \
           listview.cpp \
	   listitem.cpp \
	   main.cpp \
	   mainwindow.cpp \
	   multifilepage.cpp \
	   singlefilepage.cpp \
	   utils.cpp \
	   progress.cpp \
	   refreshthread.cpp \
    views/dfontmgrmainwindow.cpp \
    interfaces/dfontmenumanager.cpp \
    interfaces/dfontpreviewitemdelegate.cpp \
    interfaces/dfontpreviewproxymodel.cpp \
    interfaces/dfontpreviewlistview.cpp \
    views/dfinstallnormalwindow.cpp \
    views/dfinstallerroritemwidget.cpp \
    views/dfinstallerrordialog.cpp \
    views/dsplitlistwidget.cpp \
    views/dfontinfodialog.cpp \
    views/dfquickinstallwindow.cpp \
    views/dfdeletedialog.cpp

RESOURCES += deepin-font-manager.qrc
TRANSLATIONS += translations/deepin-font-manager.ts

!system(deepin-policy-ts-convert policy2ts com.deepin.pkexec.font-install.policy.tmp policy-install-translation): message("Failed policy to ts")
!system(deepin-policy-ts-convert policy2ts com.deepin.pkexec.font-uninstall.policy.tmp policy-uninstall-translation): message("Failed policy to ts")
!system(deepin-policy-ts-convert ts2policy com.deepin.pkexec.font-install.policy.tmp policy-install-translation com.deepin.pkexec.font-install.policy) {
    system(cp com.deepin.pkexec.font-install.policy.tmp com.deepin.pkexec.font-install.policy)
}
!system(deepin-policy-ts-convert ts2policy com.deepin.pkexec.font-uninstall.policy.tmp policy-uninstall-translation com.deepin.pkexec.font-uninstall.policy) {
    system(cp com.deepin.pkexec.font-uninstall.policy.tmp com.deepin.pkexec.font-uninstall.policy)
}

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-font-manager

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/deepin-font-manager.desktop

policy.path = /usr/share/polkit-1/actions
policy.files = com.deepin.pkexec.font-install.policy \
               com.deepin.pkexec.font-uninstall.policy

# Automating generation .qm files from .ts files
!system($$PWD/translate_generation.sh): error("Failed to generate translation")

translations.path = /usr/share/deepin-font-manager/translations
translations.files = $$PWD/translations/*.qm

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/images/deepin-font-manager.svg

INSTALLS += target desktop policy translations icon_files
