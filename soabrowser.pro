#-------------------------------------------------
#
# Project created by QtCreator 2012-04-28T16:43:01
#
#-------------------------------------------------

QT       += core gui xml opengl network

TARGET = soabrowser
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	sqlite3.c \
	soawidget.cpp \
	dialogs/dlgnewrepository.cpp \
	widgets/wdgtree.cpp \
	widgets/wdgtab.cpp \
	widgets/wdggraph.cpp \
	tinyxml/tinyxml.cpp \
	tinyxml/tinystr.cpp \
	tinyxml/tinyxmlparser.cpp \
	tinyxml/tinyxmlerror.cpp \
	solver/solver.cpp \
	widgets/wdginspector.cpp \
	solver/solverwork.cpp \
	solver/spheresolver.cpp \
	widgets/wdgtable.cpp \
	dialogs/dlgschema.cpp \
	widgets/wdggraphhighlight.cpp \
	widgets/wdggraphdraw.cpp \
	widgets/wdggraphmouse.cpp \
    database/soatable.cpp \
    database/soarow.cpp \
    database/soapattern.cpp \
    database/soalogger.cpp \
    database/soadatabaseportfolio.cpp \
    database/soadatabaseio.cpp \
    database/soadatabaseinterface.cpp \
    database/soadatabase.cpp \
    database/soacolumn.cpp \
    database/soacell.cpp \
    database/soatableschema.cpp \
    helpers/recents.cpp \
    mainwindowslots.cpp \
    dialogs/dlgwelcome.cpp \
    database/soatableportfolio.cpp \
    dialogs/dlgnewasset.cpp \
    widgets/wdginspectordelegate.cpp \
    widgets/wdgtreeslots.cpp \
    widgets/wdgtreeevents.cpp \
    database/soadatabasespecific.cpp \
    widgets/wdggraphrefresh.cpp \
    soawidgetactions.cpp \
    database/soatableio.cpp \
    database/soatablerows.cpp \
    database/soatablecolumns.cpp \
    gateways/layer7loader.cpp \
    gateways/soahttp.cpp \
    gateways/gateway.cpp \
    widgets/wdggateway.cpp \
    gateways/entities.cpp \
    widgets/wdggatewayconflate.cpp \
    widgets/wdggatewaytree.cpp \
    dialogs/dlgintermediaries.cpp \
    dialogs/dlgregistries.cpp \
    dialogs/dlgloadregistry.cpp \
    widgets/wdggatewayslots.cpp \
    dialogs/dlgchooseasset.cpp \
    qs.cpp \
    widgets/assetscombo.cpp \
    dialogs/dlgdependencies.cpp \
    logger/logger.cpp \
    fonts/font.cpp \
    fonts/fontdata.cpp

HEADERS  += mainwindow.h \
	sqlite3.h \
	soawidget.h \
	dialogs/dlgnewrepository.h \
	widgets/wdgtree.h \
	widgets/wdgtab.h \
	widgets/wdggraph.h \
	tinyxml/tinyxml.h \
	tinyxml/tinystr.h \
	solver/solver.h \
	widgets/wdginspector.h \
	soamath.h \
	solver/spheresolver.h \
	interfaces/iview.h \
	interfaces/iviewable.h \
	widgets/wdgtable.h \
	dialogs/dlgschema.h \
    database/soatable.h \
    database/soarow.h \
    database/soapattern.h \
    database/soalogger.h \
    database/soadatabase.h \
    database/soacolumn.h \
    database/soacell.h \
    database/soaenums.h \
    helpers/recents.h \
    dialogs/dlgwelcome.h \
    database/soahints.h \
    dialogs/dlgnewasset.h \
    gateways/layer7loader.h \
    gateways/soahttp.h \
    gateways/gateway.h \
    widgets/wdggateway.h \
    gateways/conflict.h \
    gateways/entities.h \
    dialogs/dlgintermediaries.h \
    dialogs/dlgregistries.h \
    dialogs/dlgloadregistry.h \
    qs.h \
    dialogs/dlgchooseasset.h \
    widgets/assetscombo.h \
    dialogs/dlgdependencies.h \
    logger/logger.h \
    fonts/font.h

FORMS    += mainwindow.ui \
	dlgnewrepository.ui \
	dialogs/dlgschema.ui \
    dialogs/dlgwelcome.ui \
    dialogs/dlgnewasset.ui \
    dialogs/dlglayer7.ui \
    dialogs/dlgintermediaries.ui \
    dialogs/dlgregistries.ui \
    dialogs/dlgchooseasset.ui \
    dialogs/dlgdependencies.ui

#BUILDNO = $$system($$quote(hg --debug tags))
#DEFINES += BUILD=$${BUILDNO}

DEFINES += TIXML_USE_STL

win32 {
	LIBS += -lglu32
}
unix:!macx {
        LIBS += -lGLU -ldl
}

RESOURCES += \
    resources.qrc
