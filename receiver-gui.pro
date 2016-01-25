#-------------------------------------------------
#
# Project created by QtCreator 2015-10-30T23:52:00
#
#-------------------------------------------------

QT       += core gui serialport

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = receiver-gui
TEMPLATE = app


INCLUDEPATH += /usr/include/marble/

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    configdialog.cpp \
    pages.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    mymarblewidget.h \
    configdialog.h \
    pages.h \
    telemetrypacketwrapper.h

FORMS    += mainwindow.ui

DISTFILES += \
    flightsim/flightsim.py

LIBS += -lprotobuf /usr/lib64/libmarblewidget-qt5.so#/usr/lib64/libprotobuf.so /usr/lib64/libprotoc.so /usr/lib64/libmarblewidget-qt5.so


PROTOS = proto/telemetry.proto

PROTOPATH += .
PROTOPATH += ./proto
for(p, PROTOPATH):PROTOPATHS += --proto_path=$${p}

protobuf_decl.name = protobuf headers
protobuf_decl.input = PROTOS
protobuf_decl.output = ${QMAKE_FILE_IN_PATH}/${QMAKE_FILE_BASE}.pb.h
protobuf_decl.commands = protoc --cpp_out=${QMAKE_FILE_IN_PATH} --proto_path=${QMAKE_FILE_IN_PATH} ${QMAKE_FILE_NAME}
protobuf_decl.variable_out = HEADERS
QMAKE_EXTRA_COMPILERS += protobuf_decl

protobuf_impl.name = protobuf sources
protobuf_impl.input = PROTOS
protobuf_impl.output = ${QMAKE_FILE_IN_PATH}/${QMAKE_FILE_BASE}.pb.cc
protobuf_impl.depends = ${QMAKE_FILE_IN_PATH}/${QMAKE_FILE_BASE}.pb.h
protobuf_impl.commands = $$escape_expand(\n)
protobuf_impl.variable_out = SOURCES
QMAKE_EXTRA_COMPILERS += protobuf_impl
