#ifndef SCXMLEXTERNMONITOR2_H
#define SCXMLEXTERNMONITOR2_H

#include <QLoggingCategory>
#include <QScxmlStateMachine>
#include <QScxmlInvokableService>
#include <QUdpSocket>
#include <QNetworkDatagram>

#ifdef USE_SCXML_TRIGGERED_TRANSITIONS
    /* This is the only way to monitor triggered transitions */
    /* QT += scxml-private */
    #include <algorithm>
    #include <map>
    #include <set>
    #include <QtScxml/private/qscxmlstatemachineinfo_p.h>
    #include <QtScxml/private/qscxmlstatemachine_p.h>
#endif

namespace Scxmlmonitor {

static const std::size_t SCXML_MONITOR_VERSION = 10;

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
    /* Is used for multiple instances of the same State Chart */
    /* Must be set before 'scxmlStateMachine' */
    Q_PROPERTY(QString machineID READ machineID WRITE setMachineID NOTIFY machineIDChanged)

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
        iterateAllMachines(_machine, "", this->_machineID, allMachines);

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
        iterateAllActiveStates(_machine, this->_machineID, [&out](QScxmlStateMachine *itMachine,
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

                this->iterateAllMachines(_machine, "", this->_machineID, _invokedMachines);

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

    inline void setScxmlStateMachineWithID(QScxmlStateMachine *machine, const QString &sID) {
        this->setMachineID(sID);
        this->setScxmlStateMachine(machine);
    }

    inline void setScxmlStateMachineWithSessionID(QScxmlStateMachine *machine) {
        this->setScxmlStateMachineWithID(machine,
                                         machine ? machine->sessionId() : QString(""));
    }

    inline QString machineID() { return _machineID; }
    inline void setMachineID(QString machineID) {
        if (_machineID!=machineID) {
            _machineID = machineID;
            emit machineIDChanged(_machineID);
        }
    }

signals:
    void scxmlStateMachineChanged(QScxmlStateMachine *scxmlStateMachine);
    void machineIDChanged(QString machineID);

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

    inline virtual void processEventMessage(QScxmlStateMachine *machine, const QString &id, const QScxmlEvent &event) {
        processMonitorMessage(machine->name(), id, event.name(), smttBeforeProcessingEvent);
    }

private slots:

    inline void onInvokedServicesChanged(const QVector<QScxmlInvokableService *> &) {
        if (!_machine)
            return;

        QHash<QScxmlStateMachine*, InvokedTuple> allMachines;

        iterateAllMachines(_machine, "", this->_machineID, allMachines);

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

    inline QString monitorID(QScxmlStateMachine *machine) const {
        auto itInvoked = _invokedMachines.constFind(machine);
        const QString id = itInvoked == _invokedMachines.cend() ?
                    this->_machineID : std::get<InvType::Id>(itInvoked.value());
        return id;
    }

    inline void connectMonitorToMachine(QScxmlStateMachine *machine) {
        if (!machine || _connectedMachines.contains(machine))
            return;

        _connectedMachines.insert(machine);

        /* OnEnter, OnExit - states */
        const auto allStates = machine->stateNames(false);
        for (const auto &itState : allStates) {
            auto stateconnection = machine->connectToState(itState, [=](bool active) {
                processMonitorMessage(machine->name(), monitorID(machine), itState, active ? smttBeforeEnter : smttBeforeExit);
            });
            _scxmlConnections.append(stateconnection);
        }

        /* all events + transitions */
        auto eventconnection = machine->connectToEvent("*", [=](const QScxmlEvent &event) {
            processEventMessage(machine, monitorID(machine), event);
        });
        _scxmlConnections.append(eventconnection);

        const auto invokeconnection = connect(machine, &QScxmlStateMachine::invokedServicesChanged,
                this, &IScxmlExternMonitor::onInvokedServicesChanged);
        _scxmlConnections.append(invokeconnection);

#ifdef USE_SCXML_TRIGGERED_TRANSITIONS
        QScxmlStateMachineInfo *scxmlInfo = new QScxmlStateMachineInfo(machine);

        std::map<QScxmlStateMachineInfo::StateId, std::set<QScxmlStateMachineInfo::TransitionId>> transitionsMap;
        const auto allTrans = scxmlInfo->allTransitions();
        for (auto transitionId : allTrans) {
            const auto stateFromId = scxmlInfo->transitionSource(transitionId);
            auto itStateTransitions = transitionsMap.find(stateFromId);
            if (itStateTransitions == transitionsMap.cend()) {
                transitionsMap.insert(std::make_pair(stateFromId, std::set<QScxmlStateMachineInfo::TransitionId>{ transitionId }));
            } else {
                itStateTransitions->second.insert(transitionId);
            }
        }

        const auto connectionInfo = QObject::connect(scxmlInfo, &QScxmlStateMachineInfo::transitionsTriggered, scxmlInfo,
                         [=](const QVector<QScxmlStateMachineInfo::TransitionId> &transitions) {
            if (scxmlInfo->stateMachine()) {
                for (auto transitionId : transitions) {
                    auto fromID = scxmlInfo->transitionSource(transitionId);
                    const QString sFrom = scxmlInfo->stateName(fromID);

                    const auto itStateTransitions = transitionsMap.find(fromID);
                    if (itStateTransitions != transitionsMap.cend()) {
                        const auto itTransition = itStateTransitions->second.find(transitionId);
                        if (itTransition != itStateTransitions->second.end()) {
                            const auto pos = std::distance(itStateTransitions->second.begin(), itTransition);
                            this->processMonitorMessage(machine->name(), this->monitorID(machine),
                                                        QString("%1|%2").arg(
                                                            sFrom).arg(pos),
                                                        TScxmlMsgType::smttBeforeTakingTransition);
                        }
                    }
                }
            }
        });
        _scxmlInfoConnections.append(connectionInfo);

#endif

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

#ifdef USE_SCXML_TRIGGERED_TRANSITIONS
        for (auto &it : _scxmlInfoConnections) {
            QObject::disconnect(it);
        }
        _scxmlInfoConnections.clear();
#endif

        for (auto &it : _scxmlConnections) {
            QObject::disconnect(it);
        }
        _scxmlConnections.clear();

        _connectedMachines.clear();
        _machine = nullptr;
    }

    inline void processSyncAllMonitors(QScxmlStateMachine *machine) {

        iterateAllActiveStates(machine, this->_machineID,
                               [this](QScxmlStateMachine *itMachine, const QString &id, const QString& itState){
            this->processMonitorMessage(itMachine->name(), id, itState, smttBeforeEnter);
        });
    }

    QScxmlStateMachine *_machine = nullptr;
    QHash<QScxmlStateMachine *, InvokedTuple> _invokedMachines;
    QSet<QScxmlStateMachine*> _connectedMachines;
    QList<QMetaObject::Connection> _scxmlConnections;
    QString _machineID = "";

#ifdef USE_SCXML_TRIGGERED_TRANSITIONS
    QList<QMetaObject::Connection> _scxmlInfoConnections;
#endif
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

    inline virtual void processEventMessage(QScxmlStateMachine *machine, const QString &id, const QScxmlEvent &event) override {
        QString sEventData = event.data().toString();
        if (!sEventData.isEmpty()) {
            if (sEventData.length() > 40) {
                sEventData = sEventData.left(40) + "...";
            }
        }
        processMonitorMessage(machine->name(),
                              id,
                              sEventData.isEmpty() ? event.name() : (event.name() + ": [" + sEventData + "]"),
                              Scxmlmonitor::smttBeforeProcessingEvent);
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
