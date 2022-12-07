QT += svg scxml xml

greaterThan(QT_MAJOR_VERSION, 5): QT += network svgwidgets

qtHaveModule(scxml-private) {
    QT += scxml-private
    DEFINES += USE_SCXML_TRIGGERED_TRANSITIONS
}

INCLUDEPATH += $$PWD

HEADERS += \
        $$PWD/scxmlexternmonitor2.h \
        $$PWD/scxmlsvgitem.h \
        $$PWD/scxmlsvgqmlitem.h
