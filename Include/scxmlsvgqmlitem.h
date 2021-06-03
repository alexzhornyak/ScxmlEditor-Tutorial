#ifndef SCXMLSVGQMLITEM_H
#define SCXMLSVGQMLITEM_H

/***************/
/* How to use: */
/***************/
/* 1. Load 'scxml' file with ScxmlEditor(https://github.com/alexzhornyak/ScxmlEditor-Tutorial) */
/* 2. Export to 'svg' */
/* 3. Call qmlRegisterType<Scxmlmonitor::ScxmlSvgQmlMonitor> */
/* 4. Create instance of 'ScxmlSvgMonitorWindow.qml' */
/* 5. Set 'scxmlStateMachine = machine' */
/* 6. Enjoy your state chart flow from state to state without any other dependecies :) */

/* P.S. Take a look at */
/* 'https://github.com/alexzhornyak/SCXML-tutorial/tree/master/Examples/Qt/StopWatch' example for details */

#include <QtQuick/QQuickPaintedItem>
#include <QQmlFile>
#include <QStyleOptionGraphicsItem>
#include <QTransform>
#include <QPainter>
#include <QtMath>

#include "scxmlsvgitem.h"

namespace Scxmlmonitor {

static const std::size_t SCXML_SVG_MONITOR_QML_ITEM_VERSION = 0x02;

class ScxmlSvgQmlMonitor : public QQuickPaintedItem
{
    Q_OBJECT

    /* this property initialize svg and monitor */
    Q_PROPERTY(QScxmlStateMachine *scxmlStateMachine READ scxmlStateMachine WRITE setScxmlStateMachine NOTIFY scxmlStateMachineChanged)

    /* will be applied after 'setScxmlStateMachine' */    
    Q_PROPERTY(QUrl svgUrl READ svgUrl WRITE setSvgUrl NOTIFY svgUrlChanged)

    /* optional */
    Q_PROPERTY(QString scxmlName READ scxmlName WRITE setScxmlName NOTIFY scxmlNameChanged) /* for monitoring submachines or virtual machines */
    Q_PROPERTY(QString scxmlInvokeID READ scxmlInvokeID WRITE setScxmlInvokeID NOTIFY scxmlInvokeIDChanged)
    Q_PROPERTY(int selectionBorderWidth READ selectionBorderWidth WRITE setSelectionBorderWidth NOTIFY selectionBorderWidthChanged)

    /* read-only */
    Q_PROPERTY(QRectF svgBounds READ svgBounds NOTIFY svgBoundsChanged)
    Q_PROPERTY(QRectF paintedBounds READ paintedBounds NOTIFY paintedBoundsChanged)
    Q_PROPERTY(qreal zoomFloat READ zoomFactor NOTIFY zoomChanged)
    Q_PROPERTY(int zoomPercent READ zoomPercent NOTIFY zoomChanged)

public:
    explicit ScxmlSvgQmlMonitor(QQuickItem *parent = nullptr): QQuickPaintedItem(parent) {
        connect(this, &ScxmlSvgQmlMonitor::zoomChanged, [this]() {
            emit this->paintedBoundsChanged(this->paintedBounds());
            this->update();
        });
    }

    virtual void paint(QPainter *painter) override {
        if (_svgMonitorItem && _svgMonitorItem->graphicItem() &&
                _svgMonitorItem->graphicItem()->renderer() &&
                _svgMonitorItem->graphicItem()->renderer()->isValid()) {

            const auto mRect = _svgTransform.mapRect(_svgMonitorItem->graphicItem()->boundingRect());
            _svgMonitorItem->graphicItem()->renderer()->render(painter, mRect);

            painter->resetTransform();
            painter->setTransform(_svgTransform);

            for (auto item : _svgMonitorItem->graphicItem()->childItems()) {
                if (item->isVisible()) {
                    QStyleOptionGraphicsItem opt;
                    item->paint(painter, &opt);
                }

            }
        }
    }

    inline QScxmlStateMachine *scxmlStateMachine(void) { return _svgMonitorItem ? _svgMonitorItem->scxmlStateMachine() : nullptr; }
    inline void setScxmlStateMachine(QScxmlStateMachine *machine) {
        if (machine) {
            /* nothing changed */
            if (_svgMonitorItem && machine == _svgMonitorItem->scxmlStateMachine())
                return;

            if (_svgMonitorItem) {
                delete _svgMonitorItem;
                _svgMonitorItem = nullptr;
            }

            _svgMonitorItem = new ScxmlSvgMonitorItem(QQmlFile::urlToLocalFileOrQrc(_svgUrl),
                                                      scxmlName(), scxmlInvokeID(), this);
            _svgTransform.reset();

            if (_svgMonitorItem->graphicItem()) {

                connect(_svgMonitorItem, &ScxmlSvgMonitorItem::repaintNeeded, this, [this]() {
                    this->update();
                });

                emit svgBoundsChanged(_svgMonitorItem->graphicItem()->boundingRect());
                emit zoomChanged();
            }

            _svgMonitorItem->setScxmlStateMachine(machine);

            emit scxmlStateMachineChanged(machine);
        } else {
            if (_svgMonitorItem) {
                delete _svgMonitorItem;
                _svgMonitorItem = nullptr;

                emit scxmlStateMachineChanged(machine);
            }            
        }
    }

    inline int selectionBorderWidth() const { return _svgMonitorItem ? _svgMonitorItem->selectionBorderWidth() : 0; }
    inline void setSelectionBorderWidth(int width) {
        if (_svgMonitorItem && _svgMonitorItem->selectionBorderWidth() != width) {
            _svgMonitorItem->setSelectionBorderWidth(width);
            emit selectionBorderWidthChanged(width);
        }
    }

    inline QString scxmlName(void) const { return _svgMonitorItem ? _svgMonitorItem->scxmlName() : QString(); }
    inline void setScxmlName(const QString machineName) {
        if (_svgMonitorItem && _svgMonitorItem->scxmlName() != machineName) {
            _svgMonitorItem->setScxmlName(machineName);
            emit scxmlNameChanged(machineName);
        }
    }

    inline QString scxmlInvokeID(void) const { return _svgMonitorItem ? _svgMonitorItem->scxmlInvokeID() : QString(); }
    inline void setScxmlInvokeID(const QString invokeID) {
        if (_svgMonitorItem && _svgMonitorItem->scxmlInvokeID() != invokeID) {
            _svgMonitorItem->setScxmlInvokeID(invokeID);
            emit scxmlInvokeIDChanged(invokeID);
        }
    }

    inline QUrl svgUrl(void) const { return _svgUrl; }
    inline void setSvgUrl(const QUrl source) {
        /* this will not be applied unless 'scxmlStateMachine' is changed */
        if (_svgUrl != source) {
            _svgUrl = source;
            emit svgUrlChanged(_svgUrl);
        }
    }

    inline QRectF svgBounds(void) const {
        if (_svgMonitorItem && _svgMonitorItem->graphicItem()) {            
            return _svgMonitorItem->graphicItem()->boundingRect();
        }
        return QRectF(0,0,0,0);
    }

    inline QRectF paintedBounds(void) const {
        if (_svgMonitorItem && _svgMonitorItem->graphicItem()) {
            return _svgTransform.mapRect(_svgMonitorItem->graphicItem()->boundingRect());
        }
        return QRectF(0,0,0,0);
    }

    qreal zoomFactor() const { return _svgTransform.m11(); }

    int zoomPercent() const { return static_cast<int>(zoomFactor() * static_cast<double>(100)); }

    Q_INVOKABLE void zoomIn() { zoomBy(1.25); }

    Q_INVOKABLE void zoomOut() { zoomBy(0.75); }

    Q_INVOKABLE void resetZoom() {
        if (!qFuzzyCompare(zoomFactor(), qreal(1))) {
            _svgTransform.reset();
            emit zoomChanged();
        }
    }

    Q_INVOKABLE void fitInView(const QRectF view, Qt::AspectRatioMode aspectRatioMode) {
        if (view.isEmpty())
            return;

        const auto svgRect = svgBounds();
        if (svgRect.isEmpty())
            return;

        double xRatio = view.width() / svgRect.width();
        double yRatio = view.height() / svgRect.height();

        switch (aspectRatioMode) {
        case Qt::KeepAspectRatio:
            xRatio = yRatio = qMin(xRatio, yRatio);
            break;
        case Qt::KeepAspectRatioByExpanding:
            xRatio = yRatio = qMax(xRatio, yRatio);
            break;
        case Qt::IgnoreAspectRatio:
            break;
        }

        _svgTransform.reset();
        _svgTransform.scale(xRatio, yRatio);

        emit zoomChanged();
    }

signals:
    void selectionBorderWidthChanged(int);
    void scxmlNameChanged(QString);
    void scxmlInvokeIDChanged(QString);
    void scxmlStateMachineChanged(QScxmlStateMachine *);
    void svgUrlChanged(QUrl);
    void svgBoundsChanged(QRectF);
    void paintedBoundsChanged(QRectF);
    void zoomChanged();

protected:

    virtual void wheelEvent(QWheelEvent *event) override {
        zoomBy(qPow(1.2, event->angleDelta().y() / 240.0));
    }

private:
    ScxmlSvgMonitorItem *_svgMonitorItem = nullptr;
    QUrl _svgUrl;

    QTransform _svgTransform;

    void zoomBy(qreal factor) {
        const qreal currentZoom = zoomFactor();
        if ((factor < 1 && currentZoom < 0.1) || (factor > 1 && currentZoom > 10))
            return;

        _svgTransform.scale(factor, factor);

        emit zoomChanged();
    }
}; // class ScxmlSvgQmlMonitor

} // namespace Scxmlmonitor

#endif // SCXMLSVGQMLITEM_H
