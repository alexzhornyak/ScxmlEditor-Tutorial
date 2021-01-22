#ifndef SCXMLEXTERNMONITOR2_H
#define SCXMLEXTERNMONITOR2_H

#include <QLoggingCategory>
#include <QScxmlStateMachine>
#include <QUdpSocket>
#include <QNetworkDatagram>

namespace Scxmlmonitor {

const std::size_t SCXML_MONITOR_VERSION = 1;

typedef std::tuple<QString /*parent*/, QString /*invoker*/, QString /*id*/> InvokerTuple;

/*          External SCXML monitor for ScxmlEditor            */
/* See 'https://github.com/alexzhornyak/ScxmlEditor-Tutorial' */

typedef enum { /* some elements are not used and are present for compatibility */
    smttUnknown,
    smttAfterEnter, smttBeforeEnter,
    smttAfterExit, smttBeforeExit,
    smttStep,
    smttBeforeExecContent, smttAfterExecContent,
    smttBeforeInvoke, smttAfterInvoke,
    smttBeforeUnInvoke, smttAfterUnInvoke,
    smttBeforeTakingTransition, smttAfterTakingTransition,
    smttStableConfiguration,
    smttBeforeProcessingEvent,
    smttMAXSIZE
}TScxmlMsgType;

/* Interface fot Qt Scxml monitors */
class IScxmlExternMonitor: public QObject {
    Q_OBJECT

    Q_PROPERTY(QScxmlStateMachine *scxmlStateMachine READ scxmlStateMachine WRITE setScxmlStateMachine NOTIFY scxmlStateMachineChanged)

public:
    inline explicit IScxmlExternMonitor(QObject *parent = nullptr): QObject(parent) {}

    /* sends all active states of state machine and invoked sessions */
    Q_INVOKABLE void synchronizeAllMonitors(void) {
        processClearAllMonitors();

        processSyncAllMonitors(_machine);
    }

    inline QScxmlStateMachine *scxmlStateMachine(void) { return _machine; }
    inline void setScxmlStateMachine(QScxmlStateMachine *machine) {
        if (_machine != machine) {
            _machine = machine;

            /* cleanup section */
            this->cleanup();

            /* setup section */
            if (_machine) {

                auto runningconnection = connect(_machine, &QScxmlStateMachine::runningChanged, this,
                        [=]() {
                    this->processClearAllMonitors();
                });
                _scxmlConnections.append(runningconnection);

                /* Main Machine */
                connectMonitorToMachine(_machine);

                /* Synchronize with editor if machine is already running */
                if (_machine->isRunning()) {
                    this->processSyncAllMonitors(_machine);
                }
            }

            emit scxmlStateMachineChanged(_machine);
        }
    }

Q_SIGNALS:
    void scxmlStateMachineChanged(QScxmlStateMachine *scxmlStateMachine);

protected:

    virtual void processMonitorMessage(const QString &sInterpreter, const QString &sName, const TScxmlMsgType AType) = 0;
    virtual void processClearMonitor(const QString &sInterpreter) = 0;
    virtual void processClearAllMonitors(void) = 0;

private slots:

    void onInvokedServicesChanged(const QVector<QScxmlInvokableService *> &invokedServices) {
        if (!_machine)
            return;

        QSet<InvokerTuple> machines;

        for (const auto it: invokedServices) {
            /* Invokers */
            /* Warning! May be changed in the future! */
            /* See 'https://bugreports.qt.io/browse/QTBUG-58564' */
            QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(it->property("stateMachine"));
            if (submachine && submachine!=_machine) {
                /* supposed 'parent + name + id' to be unique */
                const auto invoker = std::make_tuple(it->parentStateMachine() ?
                                                         it->parentStateMachine()->name() : _machine->name(),
                                                     it->name(), it->id());

                machines.insert(invoker);

                if (!_invokedMachines.contains(invoker)) {

                    processMonitorMessage(std::get<0>(invoker),
                                       getInvokeName(invoker),
                                       smttBeforeInvoke);

                    connectMonitorToMachine(submachine);
                }
            }

        }

        for (const auto &it: _invokedMachines.subtract(machines)) {
            processMonitorMessage(std::get<0>(it),
                               getInvokeName(it),
                               smttBeforeUnInvoke);
            /* we should check if name is not empty otherwise it will clear all in the editor */
            if (!std::get<1>(it).isEmpty()) {
                processClearMonitor(std::get<1>(it));
            }
        }

        _invokedMachines.swap(machines);
    }

private:

    inline QString getInvokeName(const InvokerTuple &invoker) const {
        return std::get<1>(invoker) + QLatin1String("[") + std::get<2>(invoker) + QLatin1String("]");
    }

    inline void connectMonitorToMachine(QScxmlStateMachine *machine) {
        if (!machine || _connectedMachines.contains(machine))
            return;

        _connectedMachines.insert(machine);

        /* OnEnter, OnExit - states */        
        for (const auto &it: machine->stateNames(false)) {
            auto stateconnection = machine->connectToState(it, [=](bool active) {
                processMonitorMessage(machine->name(), it, active ? smttBeforeEnter : smttBeforeExit);
            });
            _scxmlConnections.append(stateconnection);
        }

        /* all events + transitions */
        auto eventconnection = machine->connectToEvent("*", [=](const QScxmlEvent &event) {
            processMonitorMessage(machine->name(), event.name(), smttBeforeTakingTransition);
        });
        _scxmlConnections.append(eventconnection);

        auto invokeconnection = connect(machine, SIGNAL(invokedServicesChanged(const QVector<QScxmlInvokableService *> &)),
                this, SLOT(onInvokedServicesChanged(const QVector<QScxmlInvokableService *> &)));
        _scxmlConnections.append(invokeconnection);

        for (const auto &it : machine->invokedServices()) {
            QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(it->property("stateMachine"));
            if (submachine && submachine!=machine) {
                connectMonitorToMachine(submachine);
            }
        }
    }

    void cleanup(void) {
        _invokedMachines.clear();
        for (auto it : _scxmlConnections) {
            QObject::disconnect(it);
        }
        _scxmlConnections.clear();
        this->processClearAllMonitors();
        _connectedMachines.clear();
    }

    inline void processSyncAllMonitors(QScxmlStateMachine *machine) {
        if (machine) {
            for (const auto &it : machine->activeStateNames(false)) {
                processMonitorMessage(machine->name(), it, smttBeforeEnter);
            }

            for (const auto &it : machine->invokedServices()) {
                QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(it->property("stateMachine"));
                if (submachine && submachine!=machine) {
                    processSyncAllMonitors(submachine);
                }
            }
        }
    }

    QScxmlStateMachine *_machine = nullptr;
    QSet<InvokerTuple> _invokedMachines;
    QSet<QScxmlStateMachine*> _connectedMachines;
    QList<QMetaObject::Connection> _scxmlConnections;
};

class UDPScxmlExternMonitor: public IScxmlExternMonitor {
    Q_OBJECT

    Q_PROPERTY(int remotePort MEMBER _remotePort NOTIFY remotePortChanged)
    Q_PROPERTY(QString remoteHost MEMBER _remoteHost NOTIFY remoteHostChanged)

public:
    inline explicit UDPScxmlExternMonitor(QObject *parent = nullptr): IScxmlExternMonitor(parent) {}

Q_SIGNALS:
    void remotePortChanged(int remotePort);
    void remoteHostChanged(QString remoteHost);

protected:

    int _remotePort = 11005;
    QString _remoteHost = "";

    inline virtual void processMonitorMessage(const QString &sInterpreter, const QString &sName, const TScxmlMsgType AType) override {
        sendStringMessage(QString::number(AType) + QLatin1String("@") + sInterpreter + QLatin1String("@") + sName);
    }

    virtual void processClearMonitor(const QString &sInterpreter) override {
        sendStringMessage(QLatin1String("@@@") + sInterpreter);
    }

    inline virtual void processClearAllMonitors(void) override {
        processClearMonitor("");
    }

private:

    // network
    QUdpSocket _socket;

    void sendStringMessage(const QString &sMsg) {
        QByteArray ba;
        ba+=sMsg;

        _socket.writeDatagram(ba,
                              _remoteHost.isEmpty() ? QHostAddress::LocalHost : QHostAddress(_remoteHost),
                              static_cast<quint16>(_remotePort));
    }
};

} // namespace Scxmlmonitor

inline uint qHash(const Scxmlmonitor::InvokerTuple &key, uint seed){
    return qHash(std::get<0>(key) + QLatin1String("@") + std::get<1>(key) + QLatin1String("@") + std::get<2>(key), seed);
}

#endif // SCXMLEXTERNMONITOR2_H
