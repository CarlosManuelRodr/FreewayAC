TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11
LIBS += -pthread

SOURCES += main.cpp \
    Auxiliar.cpp \
    BmpWriter.cpp \
    CellularAutomata.cpp \
    Experiments.cpp \
    Export.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    Auxiliar.h \
    BmpWriter.h \
    CellularAutomata.h \
    Experiments.h \
    Export.h \
    optionparser.h \
    ThreadPool.h

