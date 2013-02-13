#-------------------------------------------------
#
# Project created by QtCreator 2013-02-07T12:42:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fluxo_gerenciador
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/project.cpp \
    src/people.cpp \
    src/tools.cpp \
    src/proposals.cpp \
    src/contracts.cpp \
    src/proposal.cpp \
    src/companies.cpp \
    src/person.cpp

HEADERS  += src/mainwindow.h \
    src/project.h \
    src/const.h \
    src/people.h \
    src/tools.h \
    src/proposals.h \
    src/contracts.h \
    src/proposal.h \
    src/companies.h \
    src/person.h

TRANSLATIONS = translations/pt-BR.ts
win32:RC_FILE = src/resources.rc
