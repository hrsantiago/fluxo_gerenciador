#-------------------------------------------------
#
# Project created by QtCreator 2013-02-07T12:42:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = fluxo_gerenciador
TEMPLATE = app
INCLUDEPATH += src/

SOURCES += src/main.cpp\
        src/view/mainwindow.cpp \
    src/core/project.cpp \
    src/view/people.cpp \
    src/tools.cpp \
    src/view/proposals.cpp \
    src/view/contracts.cpp \
    src/view/companies.cpp \
    src/view/templates.cpp \
    src/core/thing.cpp \
    src/view/events.cpp \
    src/view/itemlisttabwidget.cpp

HEADERS  += src/view/mainwindow.h \
    src/core/project.h \
    src/const.h \
    src/view/people.h \
    src/tools.h \
    src/view/proposals.h \
    src/view/companies.h \
    src/view/templates.h \
    src/core/thing.h \
    src/view/events.h \
    src/headers.h \
    src/view/contracts.h \
    src/view/itemlisttabwidget.h

TRANSLATIONS = translations/pt-BR.ts
win32:RC_FILE = src/resources.rc
