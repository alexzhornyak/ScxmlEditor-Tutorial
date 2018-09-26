#ifndef SCXMLEXTERNMONITOR_H
#define SCXMLEXTERNMONITOR_H

#include <iostream>

#include <QLoggingCategory>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDomDocument>
#include <QDateTime>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>

#define MLOG(VAL)	std::wcout << L#VAL << "> " << QDateTime::currentDateTime().toString("HH:mm:ss.zzz").toStdWString() << " QtScxmlTester.exe "

typedef enum {
    smttUnknown, smttAfterEnter, smttBeforeEnter, smttAfterExit, smttBeforeExit, smttStep, smttBeforeExecContent, smttAfterExecContent,
    smttBeforeInvoke, smttAfterInvoke, smttBeforeUnInvoke, smttAfterUnInvoke, smttBeforeTakingTransition, smttAfterTakingTransition, smttMAXSIZE
}TScxmlMsgType;

static QString ScxmlMsgTypeToString(const TScxmlMsgType AType) {
    switch (AType) {
    case smttAfterEnter: return "smttAfterEnter";
    case smttBeforeEnter: return "smttBeforeEnter";
    case smttAfterExit: return "smttAfterExit";
    case smttBeforeExit: return "smttBeforeExit";
    case smttBeforeExecContent: return "smttBeforeExecContent";
    case smttAfterExecContent: return "smttAfterExecContent";
    case smttBeforeInvoke: return "smttBeforeInvoke";
    case smttAfterInvoke: return "smttAfterInvoke";
    case smttBeforeUnInvoke: return "smttBeforeUnInvoke";
    case smttAfterUnInvoke: return "smttAfterUnInvoke";
    case smttBeforeTakingTransition: return "smttBeforeTakingTransition";
    case smttAfterTakingTransition: return "smttAfterTakingTransition";
    case smttStep: return "smttStep";
    }
    return "smttUnknown";
}

struct Settings {
    // flags
    bool bCheckIssue = false;
    bool bExitStop = false;
    bool bScxmlMonitorLog = false;
    // network
    long nRemotePort = 11005;
    long nLocalPort = 11001;
    // scxml
    QString sScxmlFilePath = "";
};

static QString g_ScxmlStateMachineName = "";
static Settings g_Settings;
static QUdpSocket g_Socket;

static const QRegExp rxEnterExit("^[^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\) (entering|exiting) states \\(([^\\(\\)]*)\\)$");
static const QRegExp rxSelectTransitionsEvent("^[^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\) selectTransitions with event (\\{[^\\{\\}]*\\})$");
static const QRegExp rxSelectEventlessTransition("^[^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\) selectEventlessTransitions$");
static const QRegExp rxBeforeInvoke("^[^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\) preparing to start [^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\)$");
static const QRegExp rxBeforeUnInvoke("^[^\\(\\)]*\\(0x[a-f0-9]*, name = \"([^\\(\\)]*)\"\\) exiting SCXML processing$");

void sendTestingMessage(const QString &sInterpreter, const QString &sName, const TScxmlMsgType AType) {
    const QString sMsg = QString::number(AType) + "@" + sInterpreter + "@" + sName;

    QByteArray ba;
    ba+=sMsg;

    g_Socket.writeDatagram(ba, QHostAddress::LocalHost, g_Settings.nRemotePort);
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
    case QtDebugMsg:
    case QtInfoMsg:
        if (QString::compare(context.category, "qt.scxml.statemachine")==0) {

            if (msg.contains("failed to parse") || msg.contains("had error")) {
                MLOG(ERROR) << msg.toStdWString() << std::endl;
            } else if (g_Settings.bScxmlMonitorLog) {
                MLOG(INFO) << msg.toStdWString() << std::endl;
            }


            if (rxSelectEventlessTransition.indexIn(msg)!=-1) {
                const int iCaptureCount = rxSelectEventlessTransition.captureCount();
                if (iCaptureCount==1) {
                    sendTestingMessage(rxSelectEventlessTransition.capturedTexts()[1], "*", smttBeforeTakingTransition);
                } else
                    MLOG(ERROR) << L"RegExp [selectEventlessTransitions] wait 1 captures, but got " << iCaptureCount << std::endl;

            } else if (rxSelectTransitionsEvent.indexIn(msg)!=-1) {
                const int iCaptureCount = rxSelectTransitionsEvent.captureCount();
                if (iCaptureCount==2) {
                    const QString sInterpreter = rxSelectTransitionsEvent.capturedTexts()[1];
                    QJsonDocument d = QJsonDocument::fromJson( rxSelectTransitionsEvent.capturedTexts()[2].toUtf8());
                    QJsonObject docObj = d.object();

                    sendTestingMessage(sInterpreter, docObj.value("name").toString(), smttBeforeTakingTransition);
                } else
                    MLOG(ERROR) << L"RegExp [SelectTransitionsEvent] wait 2 captures, but got " << iCaptureCount << std::endl;
            } else if (rxEnterExit.indexIn(msg)!=-1) {
                const int iCaptureCount = rxEnterExit.captureCount();
                if (iCaptureCount==3) {
                    const QString sInterpreter = rxEnterExit.capturedTexts()[1];
                    const bool active = QString::compare(rxEnterExit.capturedTexts()[2], "entering") == 0;
                    const QStringList &states = rxEnterExit.capturedTexts()[3].split(",",QString::SkipEmptyParts);

                    foreach (QString it_str, states) {

                        it_str.replace('\"', "");

                        sendTestingMessage(sInterpreter, it_str.trimmed(), active ? smttBeforeEnter : smttBeforeExit);
                    }
                } else
                    MLOG(ERROR) << L"RegExp [Enter-Exit] wait 3 captures, but got " << iCaptureCount << std::endl;
            } else if (rxBeforeInvoke.indexIn(msg)!=-1) {
                const int iCaptureCount = rxBeforeInvoke.captureCount();
                if (iCaptureCount==2) {
                    const QString sInterpreter = rxBeforeInvoke.capturedTexts()[1];
                    const QString sInvoke = rxBeforeInvoke.capturedTexts()[2];

                    sendTestingMessage(sInterpreter, sInvoke, smttBeforeInvoke);

                } else
                    MLOG(ERROR) << L"RegExp [Before Invoke] wait 2 captures, but got " << iCaptureCount << std::endl;
            } else if (rxBeforeUnInvoke.indexIn(msg)!=-1) {
                const int iCaptureCount = rxBeforeUnInvoke.captureCount();
                if (iCaptureCount==1) {
                    const QString sInvoke = rxBeforeUnInvoke.capturedTexts()[1];
                    // we are receiving info only about scxml processing,
                    // we will inform about UnInvoke only if invoke name differs from main statemachine name
                    if (g_ScxmlStateMachineName!=sInvoke) {
                        sendTestingMessage(g_ScxmlStateMachineName, sInvoke, smttBeforeUnInvoke);
                    }

                } else
                    MLOG(ERROR) << L"RegExp [selectEventlessTransitions] wait 1 captures, but got " << iCaptureCount << std::endl;

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
    }
}

#endif // SCXMLEXTERNMONITOR_H
