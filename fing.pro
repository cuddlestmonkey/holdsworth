TEMPLATE=app
CONFIG+=qt thread console 
CONFIG-=app_bundle

OBJECTS_DIR=.obj
SOURCES += main.cpp
SOURCES += getopt.cpp
HEADERS += mygetopt.h

HEADERS += holdsworth/algorithm.h
HEADERS += holdsworth/vn_algorithm.h
HEADERS += holdsworth/chunk.h
HEADERS += holdsworth/constraints.h
HEADERS += holdsworth/engine.h
HEADERS += holdsworth/instrumentdefn.h
HEADERS += holdsworth/note.h
HEADERS += holdsworth/types.h
HEADERS += holdsworth/debugging.h
HEADERS += holdsworth/handmodel.h
HEADERS += holdsworth/handmodelx.h
HEADERS += holdsworth/handmodelx2.h
HEADERS += holdsworth/musicxmlloader.h
HEADERS += holdsworth/musicxmlreader.h

SOURCES += holdsworth/algorithm.cpp
SOURCES += holdsworth/vn_algorithm.cpp
SOURCES += holdsworth/chunk.cpp
SOURCES += holdsworth/engine.cpp
SOURCES += holdsworth/instrumentdefn.cpp
SOURCES += holdsworth/note.cpp
SOURCES += holdsworth/debugging.cpp
SOURCES += holdsworth/handmodel.cpp
SOURCES += holdsworth/handmodelx.cpp
SOURCES += holdsworth/handmodelx2.cpp
SOURCES += holdsworth/musicxmlloader.cpp
SOURCES += holdsworth/musicxmlreader.cpp

#DEFINES+=SOME_DEBUG
#DEFINES+=EXTRA_DEBUG

DISTFILES += Doxyfile
DISTFILES += README
DISTFILES += Changelog
DISTFILES += ChangeLog
DISTFILES += version.i

QT-=gui
