QT += scxml xml

greaterThan(QT_MAJOR_VERSION, 5): QT += network

qtHaveModule(scxml-private) {
    QT += scxml-private
    DEFINES += USE_SCXML_TRIGGERED_TRANSITIONS
}

INCLUDEPATH += $$PWD

HEADERS += \
        $$PWD/scxmlexternmonitor2.h
