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

static const std::size_t SCXML_SVG_MONITOR_ITEM_VERSION = 0x01;

class ScxmlSvgMonitorItem: public IScxmlExternMonitor {

   Q_OBJECT

public:

    enum ItemState { Exited, Entered, Active };

    ScxmlSvgMonitorItem(const QString &svgFileName,
                        const QString machineName = QString(), /* [opt] name to monitor invoked machines */
                        const QString invokeID = QString(), /* [opt] identifier of invoked machine */
                        QObject *parent = nullptr):
         IScxmlExternMonitor(parent), _invokeID(invokeID) {

        _graphicItem = new QGraphicsSvgItem(svgFileName);

        if (!_graphicItem->renderer() || !_graphicItem->renderer()->isValid()) {
            qCritical() << "Invalid renderer for " << svgFileName;
            return;
        }

        _graphicItem->renderer()->setAspectRatioMode(Qt::KeepAspectRatio);
        _graphicItem->setFlags(QGraphicsItem::ItemClipsToShape);

        QString scxmlDocumentName = "";
        _stateIdentifiers = ScxmlSvgMonitorItem::ExtractScxmlStateIdentifiers(svgFileName, scxmlDocumentName);

        _scxmlName = machineName.isEmpty() ? scxmlDocumentName : machineName;

        for (const auto &it : _stateIdentifiers) {
            if (_graphicItem->renderer()->elementExists(it)) {
                auto item = new QGraphicsRectItem;                
                item->setParentItem(_graphicItem);                
                auto staticBounds = _graphicItem->renderer()->boundsOnElement(it);
                item->setRect(staticBounds);

                _monitorRects.insert(it, item);
            }
        }
    }

    inline int selectionBorderWidth() const { return _selectionBorderWidth; }
    inline void setSelectionBorderWidth(int width) { _selectionBorderWidth = width; }

    inline QString scxmlName(void) const { return _scxmlName; }
    inline void setScxmlName(const QString machineName) { _scxmlName = machineName; }

    inline QString scxmlInvokeID(void) const { return _invokeID; }
    inline void setScxmlInvokeID(const QString id) { _invokeID = id; }

    inline QStringList stateIdentifiers(void) { return _stateIdentifiers; }

    QGraphicsSvgItem *graphicItem(void) const { return _graphicItem; }

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

        for (const auto &it : _monitorRects) {
            this->setItemState(it, ItemState::Exited);
        }
    }

    virtual void setItemState(QGraphicsRectItem * item, const ItemState state) {
        if (!item)
            return;

        switch (state) {
        case ItemState::Exited: break;
        case ItemState::Active: {
            QPen pen(Qt::red);
            pen.setWidth(_selectionBorderWidth * 2);
            item->setPen(pen);
        } break;
        case ItemState::Entered: {
            QPen pen(Qt::blue);
            pen.setWidth(_selectionBorderWidth);
            item->setPen(pen);
        } break;
        }

        item->setVisible(state != ItemState::Exited);

        emit repaintNeeded();
    }

    /* IScxmlExternMonitor */
    inline virtual void processMonitorMessage(const QString &sInterpreter, const QString &sID, const QString &sMsg, const TScxmlMsgType AType) override {
        if (sInterpreter == _scxmlName && sID == _invokeID) {
            switch (AType) {
            case smttBeforeEnter: this->enterState(sMsg, true); break;
            case smttBeforeExit: this->enterState(sMsg, false); break;
            }
        }
    }

    virtual void processClearMonitor(const QString & sInterpreter, const QString &sID) override {
        if (sInterpreter == _scxmlName && sID == _invokeID) { this->exitAll(); }
    }

    inline virtual void processClearAllMonitors(void) override {
        this->exitAll();
    }

    inline static QStringList ExtractScxmlStateIdentifiers(const QString &svgFileName,
                                                           QString & /* out */ scxmlName) {

        QStringList outList;
        scxmlName = "";

        QDomDocument doc;
        QFile file(svgFileName);
        if (!file.open(QIODevice::ReadOnly) || !doc.setContent(&file))
                return outList;

        static const QString g_LITERAL_SCXML = "TScxmlShape";

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
                    } else if (g_LITERAL_SCXML_STATES.contains(scxmlClass)) {
                        outList.push_back(id);
                    }
                }
            }
        }

        file.close();

        return outList;
    }

private:

    int _selectionBorderWidth = 2;

    QString _scxmlName;

    QString _invokeID;

    QStringList _stateIdentifiers;

    QGraphicsRectItem *_activeMonitorRect = nullptr;

    QMap<QString, QGraphicsRectItem *> _monitorRects;

    QGraphicsSvgItem *_graphicItem = nullptr;
};

}

#endif // SCXMLSVGITEM_H
