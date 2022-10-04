#ifndef SCXMLSVGVIEW_H
#define SCXMLSVGVIEW_H

/***************/
/* How to use: */
/***************/
/* 1. Load 'scxml' file with ScxmlEditor(https://github.com/alexzhornyak/ScxmlEditor-Tutorial) */
/* 2. Export to 'svg' */
/* 3. Create Scxmlmonitor::ScxmlSvgView(_machine, "Path/To/.svg" */
/* 4. Enjoy your state chart flowing from state to state without any other dependecies :) */

/* P.S. Example: 'https://github.com/alexzhornyak/SCXML-tutorial/tree/master/Examples/Qt/DiningPhilosophers' */

#include <QGraphicsView>
#include <QWheelEvent>
#include <QtMath>

#include "scxmlsvgitem.h"

namespace Scxmlmonitor {

const std::size_t SCXML_SVG_MONITOR_VIEW_VERSION = 0x03;

class ScxmlSvgView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(QString scxmlName READ scxmlName WRITE setScxmlName NOTIFY scxmlNameChanged)
    Q_PROPERTY(QString scxmlInvokeID READ scxmlInvokeID WRITE setScxmlInvokeID NOTIFY scxmlInvokeIDChanged)
    Q_PROPERTY(QPen penActiveState READ penActiveState WRITE setPenActiveState NOTIFY penActiveStateChanged)
    Q_PROPERTY(QPen penEnteredState READ penEnteredState WRITE setPenEnteredState NOTIFY penEnteredStateChanged)
    Q_PROPERTY(QPen penActiveTransition READ penActiveTransition WRITE setPenActiveTransition NOTIFY penActiveTransitionChanged)
    Q_PROPERTY(QRectF transitionMargins READ transitionMargins WRITE setTransitionMargins NOTIFY transitionMarginsChanged)

public:

    explicit ScxmlSvgView(QScxmlStateMachine *machine, const QString &svgFileName,
                          const QString machineName = QString(),
                          const QString invokeID = QString(),
                          QWidget *parent = nullptr): QGraphicsView(parent) {
        setScene(new QGraphicsScene(this));
        setTransformationAnchor(AnchorUnderMouse);
        setDragMode(ScrollHandDrag);
        setViewportUpdateMode(FullViewportUpdate);

        _svgScxmlItem = new ScxmlSvgMonitorItem(svgFileName, machineName, invokeID, this);
        auto svgItem = _svgScxmlItem->graphicItem();
        if (svgItem && svgItem->renderer()->isValid()) {
            auto viewScene = this->scene();
            viewScene->clear();
            this->resetTransform();
            svgItem->setVisible(true);
            viewScene->addItem(svgItem);
            viewScene->setSceneRect(svgItem->boundingRect());
            _svgScxmlItem->setScxmlStateMachine(machine);
        }
    }

    inline QSize svgSize() const { return _svgScxmlItem && _svgScxmlItem->graphicItem() ? _svgScxmlItem->graphicItem()->boundingRect().size().toSize() : QSize(); }

    inline qreal zoomFactor() const { return transform().m11(); }

    inline QPen penActiveState(void) const {
        return _svgScxmlItem ? _svgScxmlItem->penActiveState() : QPen(); }
    inline void setPenActiveState(const QPen &val) {
        if (_svgScxmlItem) {
            _svgScxmlItem->setPenActiveState(val);
            emit penActiveStateChanged(val);
        }
    }

    inline QPen penEnteredState(void) const {
        return _svgScxmlItem ? _svgScxmlItem->penEnteredState() : QPen(); }
    inline void setPenEnteredState(const QPen &val) {
        if (_svgScxmlItem) {
            _svgScxmlItem->setPenEnteredState(val);
            emit penEnteredStateChanged(val);
        }
    }

    inline QPen penActiveTransition(void) const {
        return _svgScxmlItem ? _svgScxmlItem->penActiveTransition() : QPen(); }
    inline void setPenActiveTransition(const QPen &val) {
        if (_svgScxmlItem) {
            _svgScxmlItem->setPenActiveTransition(val);
            emit penActiveTransitionChanged(val);
        }
    }

    inline QRectF transitionMargins(void) const {
        return _svgScxmlItem ? _svgScxmlItem->transitionMargins() : QRectF(-10.0, -10.0, 20.0, 20.0); }
    inline void setTransitionMargins(const QRectF &val) {
        if (_svgScxmlItem) {
            _svgScxmlItem->setTransitionMargins(val);
            emit transitionMarginsChanged(val);
        }
    }

    inline QString scxmlName(void) const { return _svgScxmlItem ? _svgScxmlItem->scxmlName() : QString(); }
    inline void setScxmlName(const QString machineName) {
        if (_svgScxmlItem && _svgScxmlItem->scxmlName() != machineName) {
            _svgScxmlItem->setScxmlName(machineName);
            emit scxmlNameChanged(machineName);
        }
    }

    inline QString scxmlInvokeID(void) const { return _svgScxmlItem ? _svgScxmlItem->scxmlInvokeID() : QString(); }
    inline void setScxmlInvokeID(const QString invokeID) {
        if (_svgScxmlItem && _svgScxmlItem->scxmlInvokeID() != invokeID) {
            _svgScxmlItem->setScxmlInvokeID(invokeID);
            emit scxmlInvokeIDChanged(invokeID);
        }
    }

    inline ScxmlSvgMonitorItem *getSvgMonitorItem(void) {
        return _svgScxmlItem;
    }

public slots:

    inline void zoomIn() { zoomBy(1.25); }
    inline void zoomOut() { zoomBy(0.75); }
    inline void resetZoom() {
        if (!qFuzzyCompare(zoomFactor(), qreal(1))) {
            resetTransform();
            emit zoomChanged();
        }
    }

signals:
    void zoomChanged();
    void penActiveStateChanged(QPen);
    void penEnteredStateChanged(QPen);
    void penActiveTransitionChanged(QPen);
    void transitionMarginsChanged(QRectF);
    void scxmlNameChanged(QString);
    void scxmlInvokeIDChanged(QString);

protected:

    inline void wheelEvent(QWheelEvent *event) override {
        zoomBy(qPow(1.2, event->angleDelta().y() / 240.0));
    }

private:

    inline void zoomBy(qreal factor) {
        const qreal currentZoom = zoomFactor();
        if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10))
            return;
        scale(factor, factor);
        emit zoomChanged();
    }

    ScxmlSvgMonitorItem *_svgScxmlItem = nullptr;
};

}

#endif // SCXMLSVGVIEW_H
