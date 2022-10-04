#ifndef SCXMLSVGITEM_H
#define SCXMLSVGITEM_H

/***************/
/* How to use: */
/***************/
/* 1. Load 'scxml' file with ScxmlEditor(https://github.com/alexzhornyak/ScxmlEditor-Tutorial) */
/* 2. Export to 'svg' */
/* 3. Create ScxmlSvgMonitorItem(const QString &svgFileName) */
/* 4. Set ScxmlSvgMonitorItem->setScxmlStateMachine(machine) */
/* 5. Connect to signal 'repaintNeeded' and paint data */
/* 6. Enjoy your state chart flowing from state to state without any other dependecies :) */

#include <QSet>
#include <QMap>
#include <QSvgRenderer>
#include <QGraphicsSvgItem>
#include <QDomDocument>
#include <QFile>
#include <QPen>

#include "scxmlexternmonitor2.h"

namespace Scxmlmonitor {

static const std::size_t SCXML_SVG_MONITOR_ITEM_VERSION = 3;

class ScxmlSvgMonitorItem: public IScxmlExternMonitor {

   Q_OBJECT

public:

    enum ItemState { Exited, Entered, Active };

    ScxmlSvgMonitorItem(const QString &svgFileName,
                        const QString machineName = QString(), /* [opt] name to monitor invoked machines */
                        const QString invokeID = QString(), /* [opt] identifier of invoked machine */
                        QObject *parent = nullptr):
         IScxmlExternMonitor(parent), _invokeID(invokeID) {

        _penActiveState.setColor(Qt::red);
        _penActiveState.setWidth(3);

        _penEnteredState.setColor(Qt::blue);
        _penEnteredState.setWidth(2);

        _penActiveTransition.setColor(Qt::red);
        _penActiveTransition.setWidth(2);

        _transitionMargins.setRect(-10.0, -10.0, 20.0, 20.0);

        _graphicItem = new QGraphicsSvgItem(svgFileName);

        if (!_graphicItem->renderer() || !_graphicItem->renderer()->isValid()) {
            qCritical() << "Invalid renderer for " << svgFileName;
            return;
        }
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        _graphicItem->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
#endif
        _graphicItem->setFlags(QGraphicsItem::ItemClipsToShape);

        QString scxmlDocumentName = "";
        ScxmlSvgMonitorItem::ExtractScxmlIdentifiers(
                    svgFileName, scxmlDocumentName, _stateIdentifiers, _transitionIdentifiers);

        _scxmlName = machineName.isEmpty() ? scxmlDocumentName : machineName;

        for (const auto &it : qAsConst(_stateIdentifiers)) {
            if (_graphicItem->renderer()->elementExists(it)) {
                auto item = new QGraphicsRectItem;                
                item->setParentItem(_graphicItem);                
                auto staticBounds = _graphicItem->renderer()->boundsOnElement(it);
                item->setRect(staticBounds);
                item->setVisible(false);

                _monitorRects.insert(it, item);
            }
        }

        for (const auto &it : _transitionIdentifiers.toStdMap()) {
            if (_graphicItem->renderer()->elementExists(it.second)) {
                auto item = new QGraphicsRectItem;
                item->setParentItem(_graphicItem);

                const auto staticBounds = _graphicItem->renderer()->boundsOnElement(it.second);
                const auto adjustedBounds = staticBounds.adjusted(_transitionMargins.left(), _transitionMargins.top(),
                                                       _transitionMargins.right(), _transitionMargins.bottom());

                item->setRect(adjustedBounds);

                _transitionRects.insert(it.first, qMakePair(item, staticBounds));
                item->setVisible(false);
            }
        }
    }

    inline QPen penActiveState(void) const { return _penActiveState; }
    inline void setPenActiveState(const QPen &val) { _penActiveState = val; }

    inline QPen penEnteredState(void) const { return _penEnteredState; }
    inline void setPenEnteredState(const QPen &val) { _penEnteredState = val; }

    inline QPen penActiveTransition(void) const { return _penActiveTransition; }
    inline void setPenActiveTransition(const QPen &val) { _penActiveTransition = val; }

    inline QRectF transitionMargins(void) const { return _transitionMargins; }
    inline void setTransitionMargins(const QRectF &val) {
        if (_transitionMargins != val) {
            _transitionMargins = val;

            for (auto it: _transitionRects.values()) {
                it.first->setRect(it.second.adjusted(
                                      _transitionMargins.left(),
                                      _transitionMargins.top(),
                                      _transitionMargins.right(),
                                      _transitionMargins.bottom()));
            }

            emit repaintNeeded();
        }
    }

    inline QString scxmlName(void) const { return _scxmlName; }
    inline void setScxmlName(const QString machineName) { _scxmlName = machineName; }

    inline QString scxmlInvokeID(void) const { return _invokeID; }
    inline void setScxmlInvokeID(const QString id) { _invokeID = id; }

    inline QStringList stateIdentifiers(void) const { return _stateIdentifiers; }

    inline QGraphicsSvgItem *graphicItem(void) const { return _graphicItem; }

signals:

    void repaintNeeded(void);

protected:

    inline void enterState(const QString &id, const bool enter) {
        auto monitorRect = _monitorRects.find(id);
        if (monitorRect != _monitorRects.end()) {
            if (enter) {
                this->setItemState(monitorRect.value(), ItemState::Active);

                if (_activeMonitorRect && _activeMonitorRect != monitorRect.value()) {
                    this->setItemState(_activeMonitorRect, ItemState::Entered);
                }

                _activeMonitorRect = monitorRect.value();
            } else {
                this->setItemState(monitorRect.value(), ItemState::Exited);

                if (_activeMonitorRect == monitorRect.value()) {
                    _activeMonitorRect = nullptr;
                }
            }
        }
    }

    inline void exitAll(void) {
        _activeMonitorRect = nullptr;

        for (const auto &it : _monitorRects.values()) {
            it->setVisible(false);
        }

        for (const auto &it : _transitionRects.values()) {
            it.first->setVisible(false);
        }

        emit repaintNeeded();
    }

    virtual void setItemState(QGraphicsRectItem * item, const ItemState state) {
        if (!item)
            return;

        switch (state) {
        case ItemState::Exited: break;
        case ItemState::Active: {            
            item->setPen(this->_penActiveState);
        } break;
        case ItemState::Entered: {
            item->setPen(this->_penEnteredState);
        } break;
        }

        item->setVisible(state != ItemState::Exited);

        emit repaintNeeded();
    }

    inline virtual void takingTransition(const QString &sId) {
        auto it = _transitionRects.find(sId);
        QGraphicsRectItem *activeItem = it != _transitionRects.end() ? it.value().first : nullptr;

        /* can trigger the same transition, so prevent useless redraw */
        bool requireRepaint = false;
        for (auto itPair : _transitionRects.values()) {
            const bool newVisible = itPair.first == activeItem;
            if (itPair.first->isVisible() != newVisible) {
                if (newVisible) {
                    itPair.first->setPen(_penActiveTransition);
                }
                itPair.first->setVisible(newVisible);
                requireRepaint = true;
            }
        }

        if (requireRepaint)
            emit repaintNeeded();
    }

    /* IScxmlExternMonitor */
    inline virtual void processMonitorMessage(const QString &sInterpreter, const QString &sID, const QString &sMsg, const TScxmlMsgType AType) override {
        if (sInterpreter == _scxmlName && sID == _invokeID) {
            switch (AType) {
            case smttBeforeEnter: this->enterState(sMsg, true); break;
            case smttBeforeExit: this->enterState(sMsg, false); break;
            case smttBeforeTakingTransition: this->takingTransition(sMsg); break;
            default:
                break;
            }
        }
    }

    virtual void processClearMonitor(const QString & sInterpreter, const QString &sID) override {
        if (sInterpreter == _scxmlName && sID == _invokeID) { this->exitAll(); }
    }

    inline virtual void processClearAllMonitors(void) override {
        this->exitAll();
    }

    inline static void ExtractScxmlIdentifiers(const QString &svgFileName,
                                                           QString & /* out */ scxmlName,
                                                           QStringList & /* out */outStates,
                                                           QMap<QString, QString> &outTransitions
                                               ) {
        scxmlName = "";
        outStates.clear();
        outTransitions.clear();

        QDomDocument doc;
        QFile file(svgFileName);
        if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
                return;

        static const QString g_LITERAL_SCXML = "TScxmlShape";

        static const QString g_LITERAL_SCXML_TRANSITION = "TStateMachineConnection";

        static const QSet<QString> g_LITERAL_SCXML_STATES {
            "TStateShape", "TParallelShape", "TVirtualShape", "TFinalShape" };

        QDomNodeList gList = doc.elementsByTagName("g");
        for (int i = 0; i < gList.size(); i++) {
            QDomNode gNode = gList.item(i);

            if (gNode.isElement() && gNode.hasAttributes()) {
                auto gElement = gNode.toElement();
                const QString id = gElement.attribute("id");
                const QString scxmlClass = gElement.attribute("class");

                if (!id.isEmpty() && !scxmlClass.isEmpty()) {
                    if (scxmlClass == g_LITERAL_SCXML) {
                        scxmlName = id;
                    }
                    else if (scxmlClass == g_LITERAL_SCXML_TRANSITION) {
                        auto descNode = gNode.firstChildElement("desc");
                        if (!descNode.isNull() && descNode.nodeName() == "desc") {
                            for (auto itStr : descNode.text().split("\n")) {
                                itStr = itStr.trimmed();
                                if (!itStr.isEmpty()) {
                                    outTransitions.insert(itStr, id);
                                }
                            }
                        }
                    }
                    else if (g_LITERAL_SCXML_STATES.contains(scxmlClass)) {
                        outStates.push_back(id);
                    }
                }
            }
        }

        file.close();
    }

private:

    QPen _penActiveState;

    QPen _penEnteredState;

    QPen _penActiveTransition;

    QRectF _transitionMargins;

    QString _scxmlName;

    QString _invokeID;

    QStringList _stateIdentifiers;

    QMap<QString, QString> _transitionIdentifiers;

    QGraphicsRectItem *_activeMonitorRect = nullptr;

    QMap<QString, QGraphicsRectItem *> _monitorRects;

    QMap<QString, QPair<QGraphicsRectItem *, QRectF> > _transitionRects;

    QGraphicsSvgItem *_graphicItem = nullptr;
};

}

#endif // SCXMLSVGITEM_H
