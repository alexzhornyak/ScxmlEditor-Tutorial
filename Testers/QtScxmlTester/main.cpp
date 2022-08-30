#include <QCoreApplication>

#include <iostream>
#include <memory>

#include <QDebug>
#include <QLoggingCategory>
#include <QScxmlStateMachine>
#include <QScxmlInvokableService>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDomDocument>
#include <QFile>
#include <QCommandLineParser>
#include <QDateTime>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../Include/scxmlexternmonitor2.h"

#define MLOG(VAL)	std::wcout << L#VAL << "> " << QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdWString() << " QtScxmlTester.exe "

typedef enum { cttDefault, cttBool, cttInteger, cttDouble, cttString } TContentTriggerType;

struct Settings {
    // flags
    bool bCheckIssue = false;
    bool bExitStop = false;
    bool bScxmlMonitorLog = false;
    bool bHideDebugInfo = false;
    // network
    long nRemotePort = 11005;
    long nLocalPort = 11001;
    // scxml
    QString sScxmlFilePath = "";
};

static Settings settings;
static QUdpSocket socket;
static std::unique_ptr<QScxmlStateMachine> machine_ptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
        if (QString::compare(context.category, "qt.scxml.statemachine")==0) {

            if (msg.contains("failed to parse") || msg.contains("had error")) {
                MLOG(ERROR) << msg.toStdWString() << std::endl;
            } else if (settings.bScxmlMonitorLog) {
                MLOG(INFO) << msg.toStdWString() << std::endl;
            }            

        } else {
            MLOG(INFO) << msg.toStdWString() << std::endl;
        }
        break;
    case QtWarningMsg:
        MLOG(WARNING) << msg.toStdWString() << std::endl;
        break;
    case QtCriticalMsg:
    case QtFatalMsg:
        MLOG(ERROR) << msg.toStdWString() << std::endl;
        break;
    default:
        MLOG(UNKNOWN) << msg.toStdWString() << std::endl;
        break;
    }
}

int main(int argc, char *argv[])
{
    int iRes = 1;

    auto showCMD = [](const QStringList &arguments) {
        MLOG(INFO) << L"CommandLine [";
        for (const auto &it : arguments) {
            std::wcout << it.toStdWString() << L" ";
        }
        std::wcout << L"]" << std::endl;
    };

    try {

        std::locale::global(std::locale("",std::locale::messages));
        std::locale::global(std::locale("", std::locale::ctype));

        QCoreApplication a(argc, argv);
        QCoreApplication::setApplicationName("QtScxmlTester");

        QCommandLineParser parser;
        parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
        parser.addHelpOption();

        parser.addOptions({
                             { "issue", "check scxml for issues and errors, by default: false", "issue", "false" },
                             { "scxmlcoutlog", "log all StateMachine events, by default: false", "scxmlcoutlog", "false" },
                             { "exitstop", "pause application on exit, by default: false", "exitstop", "false" },
                             { "r", "port for GUI application, by default: 11005", "remote_port", QString::number(settings.nRemotePort) },
                             { "l", "local port for QtScxmlTester application, by default: 11001", "local_port", QString::number(settings.nLocalPort) },
                             { "f", "full path to scxml file, for example: C:\\Projects\\helloWorld.scxml", "scxml_path" },
                             { "hidedebuginfo", "hides debug information, by default: false", "hidedebuginfo", "false" }
                          });


        if (!parser.parse(a.arguments())) {
            showCMD(a.arguments());

            MLOG(WARNING) << parser.errorText().toStdWString() << std::endl;
        }

        settings.bCheckIssue = parser.value("issue") == "true";
        settings.bExitStop = parser.value("exitstop") == "true";
        settings.bScxmlMonitorLog = parser.value("scxmlcoutlog") == "true";
        settings.sScxmlFilePath = parser.value("f");
        settings.nLocalPort = parser.value("l").toLong();
        settings.nRemotePort = parser.value("r").toLong();
        settings.bHideDebugInfo = parser.value("hidedebuginfo") == "true";

        if (!settings.bHideDebugInfo) {
            showCMD(a.arguments());
        }

        qInstallMessageHandler(myMessageOutput);
        QLoggingCategory::setFilterRules("qt.scxml.statemachine=true");

        machine_ptr.reset(QScxmlStateMachine::fromFile(settings.sScxmlFilePath));
        if (!machine_ptr) throw std::runtime_error("Can not load <" + settings.sScxmlFilePath.toStdString() + ">");        

        const auto vecErrors = machine_ptr->parseErrors();
        for (const auto &itErr : vecErrors) {
            MLOG(ERROR) << itErr.toString().toStdWString() << std::endl;
        }

        if (!vecErrors.empty()) throw std::exception("Application is terminated due to parsing critical errors!");

        if (!socket.bind(QHostAddress::Any, static_cast<quint16>(settings.nLocalPort))) {
            MLOG(ERROR) << socket.errorString().toStdWString() << std::endl;
            return -1;
        }

        Scxmlmonitor::UDPScxmlExternMonitor *monitor = new Scxmlmonitor::UDPScxmlExternMonitor(machine_ptr.get());
        monitor->setRemotePort(settings.nRemotePort);
        monitor->setScxmlStateMachine(machine_ptr.get());

        auto getTriggerValue = [](const QString &sText, const TContentTriggerType triggerType) {
            QVariant varData;
            switch (triggerType) {
            case cttInteger:
                varData = sText.toInt();
                break;
            case cttDouble:
                varData = sText.toDouble();
                break;
            case cttBool:
                {
                    const bool bVal = sText.compare("true",Qt::CaseInsensitive) == 0;
                    varData = bVal;
                } break;
            case cttString:
            default:
                varData = sText;
                break;
            }
            return varData;
        };

        QObject::connect(&socket, &QUdpSocket::readyRead, [&](){
            while (socket.hasPendingDatagrams()) {
                QNetworkDatagram datagram = socket.receiveDatagram();

                /* Custom Commands */
                QString sIncomeData = QString::fromUtf8(datagram.data());
                if (sIncomeData.startsWith("SYNC_INVOKED@")) {

                    monitor->synchronizeMonitor("", sIncomeData.replace("SYNC_INVOKED@", ""));

                    continue;
                }

                /* Events */
                QDomDocument doc;
                doc.setContent(datagram.data());

                QDomElement docElem = doc.documentElement();
                if (!docElem.isNull() && docElem.nodeName()=="EVENT") {

                    QVariant varData;
                    QVariantMap varMap;

                    bool isContext = false;

                    QDomNode n = docElem.firstChild();
                    while(!n.isNull()) {
                        QDomElement e = n.toElement(); // try to convert the node to an element.
                        if(!e.isNull()) {
                            if (e.nodeName()=="content") {
                                const TContentTriggerType triggerType = TContentTriggerType(e.attribute("type","0").toInt());
                                varData = getTriggerValue(e.text(), triggerType);
                                isContext = true;
                            }   else if (e.nodeName()=="param") {

                                QString key = e.attribute("name");
                                QString value = e.attribute("expr");
                                const TContentTriggerType triggerType = TContentTriggerType(e.attribute("type","0").toInt());
                                varMap[key] = getTriggerValue(value, triggerType);
                            }
                        }
                        n = n.nextSibling();
                    }

                    machine_ptr->submitEvent(docElem.attribute("name"), isContext ? varData : varMap);
                }


            }
        } );

        QObject::connect(machine_ptr.get(), &QScxmlStateMachine::finished, &a, &QCoreApplication::quit);

        machine_ptr->start();

        iRes = a.exec();
    } catch (std::exception &e) {
        MLOG(ERROR) << QString(e.what()).toStdWString() << std::endl;
        iRes = -1;
    } catch (...) {
        MLOG(ERROR) << L"Unknown exception while executing a program!" << std::endl;
        iRes = -1;
    }

    if (!settings.bHideDebugInfo) {
        MLOG(INFO) << L"Program is finished!" << std::endl;
    }

    return iRes;
}
