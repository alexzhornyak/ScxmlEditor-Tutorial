#ifndef SCXMLEXTERNMONITOR2_H
#define SCXMLEXTERNMONITOR2_H

#include <QLoggingCategory>
#include <QScxmlStateMachine>
#include <QScxmlInvokableService>
#include <QUdpSocket>
#include <QNetworkDatagram>

namespace Scxmlmonitor {

static const std::size_t SCXML_MONITOR_VERSION = 0x07;

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

typedef std::tuple<QString /*parent*/, QString /*invoked*/, QString /*id*/> InvokedTuple;

/* Interface fot Qt Scxml monitors */
class IScxmlExternMonitor: public QObject {
    Q_OBJECT

    Q_PROPERTY(QScxmlStateMachine *scxmlStateMachine READ scxmlStateMachine WRITE setScxmlStateMachine NOTIFY scxmlStateMachineChanged)

public:
    inline explicit IScxmlExternMonitor(QObject *parent = nullptr): QObject(parent) {}
    virtual ~IScxmlExternMonitor() override { cleanup(); }

    enum InvType { ParentMachine, Machine, Id };

    /* sends all active states of state machine and invoked sessions */
    Q_INVOKABLE void synchronizeAllMonitors(void) {
        processClearAllMonitors();

        processSyncAllMonitors(_machine);
    }

    /* sends all active states of state machine by ScxmlName or InvokeID */
    Q_INVOKABLE void synchronizeMonitor(const QString sInterpreter = "", const QString sID = "") {

        /* 1) we do not use stored '_invokedMachines' because we need 100% valid pointer */
        /* 2) we do not use 'break' becase there maybe a couple of same interpreters */

        QHash<QScxmlStateMachine*, InvokedTuple> allMachines;
        iterateAllMachines(_machine, "", "", allMachines);

        for (auto it = allMachines.cbegin();it!=allMachines.cend();++it) {
            if (sInterpreter.isEmpty() || std::get<InvType::Machine>(it.value())==sInterpreter) {
                if (sID.isEmpty() || sID == std::get<InvType::Id>(it.value())) {
                    processClearMonitor(sInterpreter, sID);
                    processSyncAllMonitors(it.key());
                }
            }
        }
    }

    /* saves all active states in format of 'https://alexzhornyak.github.io/ScxmlEditor-Tutorial/' */
    /* for using in menu 'Import states configuration' */
    Q_INVOKABLE QStringList dumpAllActiveStates() {
        QStringList out;
        iterateAllActiveStates(_machine, "", [&out](QScxmlStateMachine *itMachine,
                               const QString &id,
                               const QString &itState){

            out.append(itMachine->name() +
                       (id.isEmpty() ? id : QString("[%1]").arg(id)) +
                       "=" + itState);
        });
        return out;
    }

    inline QScxmlStateMachine *scxmlStateMachine(void) { return _machine; }
    inline void setScxmlStateMachine(QScxmlStateMachine *machine) {
        if (_machine != machine) {

            this->processClearAllMonitors();

            /* cleanup section */
            this->cleanup();

            _machine = machine;

            /* setup section */
            if (_machine) {

                const auto runningconnection = connect(_machine, &QScxmlStateMachine::runningChanged, this,
                        [this](bool) {
                    this->processClearAllMonitors();
                });
                _scxmlConnections.append(runningconnection);

                this->iterateAllMachines(_machine, "", "", _invokedMachines);

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

signals:
    void scxmlStateMachineChanged(QScxmlStateMachine *scxmlStateMachine);

protected:

    virtual void processMonitorMessage(const QString &sInterpreter,
                                       const QString &sID,
                                       const QString &sMsg,
                                       const TScxmlMsgType AType) = 0;
    virtual void processClearMonitor(const QString &sInterpreter, const QString &sID) = 0;
    virtual void processClearAllMonitors(void) = 0;

    inline void iterateAllActiveStates(QScxmlStateMachine *machine, const QString &id,
                                 std::function<void (QScxmlStateMachine *, const QString &, const QString&)> func) {
        if (machine) {
            const auto allActive = machine->activeStateNames(false);
            for (const auto &itState: allActive) {
                func(machine, id, itState);
            }

            const auto allInvoked = machine->invokedServices();
            for (const auto itInvoked: allInvoked) {
                QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(itInvoked->property("stateMachine"));
                if (submachine && submachine!=machine) {
                    iterateAllActiveStates(submachine, itInvoked->id(), func);
                }
            }
        }
    }

private slots:

    inline void onInvokedServicesChanged(const QVector<QScxmlInvokableService *> &) {
        if (!_machine)
            return;

        QHash<QScxmlStateMachine*, InvokedTuple> allMachines;

        iterateAllMachines(_machine, "", "", allMachines);

        for (auto it = allMachines.cbegin();it!=allMachines.cend();++it) {
            auto itMachine = _invokedMachines.find(it.key());
            if (itMachine == _invokedMachines.end()) {
                processMonitorMessage(std::get<InvType::ParentMachine>(it.value()),
                                   std::get<InvType::Id>(it.value()),
                                   std::get<InvType::Machine>(it.value()),
                                   smttBeforeInvoke);

                connectMonitorToMachine(it.key());
            } else {
                _invokedMachines.erase(itMachine);
            }
        }

        /* left machines are uninvoked */
        for (auto it = _invokedMachines.cbegin();it!=_invokedMachines.cend();++it) {
            processMonitorMessage(std::get<InvType::ParentMachine>(it.value()),
                               std::get<InvType::Id>(it.value()),
                               std::get<InvType::Machine>(it.value()),
                               smttBeforeUnInvoke);
            /* we should check if name is not empty otherwise it will clear all in the editor */
            if (!std::get<InvType::Machine>(it.value()).isEmpty()) {
                processClearMonitor(std::get<InvType::Machine>(it.value()),
                                    std::get<InvType::Id>(it.value()));
            }
        }

        _invokedMachines.swap(allMachines);
    }

private:

    inline void iterateAllMachines(QScxmlStateMachine *machine, const QString &parent, const QString &id,
                                   QHash<QScxmlStateMachine *, InvokedTuple> &machinesMap) {
        if (!machine)
            return;

        if (machinesMap.find(machine)==machinesMap.end()) {
            machinesMap.insert(machine, std::make_tuple(parent, machine->name(), id));

            const auto allInvoked = machine->invokedServices();
            for (const auto &itInvoked : allInvoked) {
                /* Warning! May be changed in the future! */
                /* See 'https://bugreports.qt.io/browse/QTBUG-58564' */
                QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(itInvoked->property("stateMachine"));
                if (submachine && submachine!=machine) {
                    iterateAllMachines(submachine, itInvoked->parentStateMachine() ?
                                           itInvoked->parentStateMachine()->name() : _machine->name(),
                                       itInvoked->id(), machinesMap);
                }
            }
        }
    }

    inline void connectMonitorToMachine(QScxmlStateMachine *machine) {
        if (!machine || _connectedMachines.contains(machine))
            return;

        _connectedMachines.insert(machine);

        /* OnEnter, OnExit - states */
        const auto allStates = machine->stateNames(false);
        for (const auto &itState : allStates) {
            auto stateconnection = machine->connectToState(itState, [=](bool active) {
                auto itInvoked = _invokedMachines.constFind(machine);
                const QString id = itInvoked == _invokedMachines.cend() ?
                            QString("") : std::get<InvType::Id>(itInvoked.value());
                processMonitorMessage(machine->name(), id, itState, active ? smttBeforeEnter : smttBeforeExit);
            });
            _scxmlConnections.append(stateconnection);
        }

        /* all events + transitions */
        auto eventconnection = machine->connectToEvent("*", [=](const QScxmlEvent &event) {
            auto itInvoked = _invokedMachines.constFind(machine);
            const QString id = itInvoked == _invokedMachines.cend() ?
                        QString("") : std::get<InvType::Id>(itInvoked.value());
            processMonitorMessage(machine->name(), id, event.name(), smttBeforeTakingTransition);
        });
        _scxmlConnections.append(eventconnection);

        const auto invokeconnection = connect(machine, &QScxmlStateMachine::invokedServicesChanged,
                this, &IScxmlExternMonitor::onInvokedServicesChanged);
        _scxmlConnections.append(invokeconnection);

        const auto allInvoked = machine->invokedServices();
        for (const auto it: allInvoked) {
            QScxmlStateMachine *submachine = qvariant_cast<QScxmlStateMachine *>(it->property("stateMachine"));
            if (submachine && submachine!=machine) {
                connectMonitorToMachine(submachine);
            }
        }
    }

    inline void cleanup(void) {        
        _invokedMachines.clear();
        for (auto &it : _scxmlConnections) {
            QObject::disconnect(it);
        }
        _scxmlConnections.clear();        
        _connectedMachines.clear();
        _machine = nullptr;
    }

    inline void processSyncAllMonitors(QScxmlStateMachine *machine) {

        iterateAllActiveStates(machine, "",
                               [this](QScxmlStateMachine *itMachine, const QString &id, const QString& itState){
            this->processMonitorMessage(itMachine->name(), id, itState, smttBeforeEnter);
        });
    }

    QScxmlStateMachine *_machine = nullptr;
    QHash<QScxmlStateMachine *, InvokedTuple> _invokedMachines;
    QSet<QScxmlStateMachine*> _connectedMachines;
    QList<QMetaObject::Connection> _scxmlConnections;
};

class UDPScxmlExternMonitor: public IScxmlExternMonitor {
    Q_OBJECT

    Q_PROPERTY(int remotePort READ remotePort WRITE setRemotePort NOTIFY remotePortChanged)
    Q_PROPERTY(QString remoteHost READ remoteHost WRITE setRemoteHost NOTIFY remoteHostChanged)

public:
    inline explicit UDPScxmlExternMonitor(QObject *parent = nullptr): IScxmlExternMonitor(parent) {}

    inline int remotePort(void) { return _remotePort; }
    inline void setRemotePort(int port) {
        if (_remotePort!=port) {
            _remotePort = port;
            emit remotePortChanged(_remotePort);
        }
    }

    inline QString remoteHost() { return _remoteHost; }
    inline void setRemoteHost(QString host) {
        if (_remoteHost!=host) {
            _remoteHost = host;
            emit remoteHostChanged(_remoteHost);
        }
    }

signals:
    void remotePortChanged(int remotePort);
    void remoteHostChanged(QString remoteHost);

protected:

    int _remotePort = 11005;
    QString _remoteHost = "";

    inline virtual void processMonitorMessage(const QString &sInterpreter, const QString &sID, const QString &sMsg, const TScxmlMsgType AType) override {
        sendStringMessage(QString("%1@%2@%3@%4")
                          .arg(AType)
                          .arg(sInterpreter, sMsg, sID)
                          );
    }

    virtual void processClearMonitor(const QString &sInterpreter, const QString &sID) override {
        sendStringMessage(QString("@@@%1@%2").arg(sInterpreter, sID));
    }

    inline virtual void processClearAllMonitors(void) override {
        processClearMonitor("", "");
    }

private:

    // network
    QUdpSocket _socket;

    void sendStringMessage(const QString &sMsg) {
        _socket.writeDatagram(sMsg.toUtf8(),
                              _remoteHost.isEmpty() ? QHostAddress::LocalHost : QHostAddress(_remoteHost),
                              static_cast<quint16>(_remotePort));
    }
};

} // namespace Scxmlmonitor

#endif // SCXMLEXTERNMONITOR2_H
