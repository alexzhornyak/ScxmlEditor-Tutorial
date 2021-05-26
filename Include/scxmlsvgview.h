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

const std::size_t SCXML_SVG_MONITOR_VIEW_VERSION = 0x01;

class ScxmlSvgView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(QString scxmlName READ scxmlName WRITE setScxmlName NOTIFY scxmlNameChanged)
    Q_PROPERTY(QString scxmlInvokeID READ scxmlInvokeID WRITE setScxmlInvokeID NOTIFY scxmlInvokeIDChanged)
    Q_PROPERTY(int selectionBorderWidth READ selectionBorderWidth WRITE setSelectionBorderWidth NOTIFY selectionBorderWidthChanged)

public:

    explicit ScxmlSvgView(QScxmlStateMachine *machine, const QString &svgFileName,
                          const QString machineName = QString(),
                          const QString invokeID = QString(),
                          QWidget *parent = nullptr): QGraphicsView(parent) {
        setScene(new QGraphicsScene(this));
        setTransformationAnchor(AnchorUnderMouse);
        setDragMode(ScrollHandDrag);
        setViewportUpdateMode(FullViewportUpdate);

        if (machine) {

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
    }

    inline QSize svgSize() const { return _svgScxmlItem && _svgScxmlItem->graphicItem() ? _svgScxmlItem->graphicItem()->boundingRect().size().toSize() : QSize(); }

    inline qreal zoomFactor() const { return transform().m11(); }

    inline int selectionBorderWidth() const { return _svgScxmlItem ? _svgScxmlItem->selectionBorderWidth() : 0; }
    inline void setSelectionBorderWidth(int width) {
        if (_svgScxmlItem && _svgScxmlItem->selectionBorderWidth() != width) {
            _svgScxmlItem->setSelectionBorderWidth(width);
            emit selectionBorderWidthChanged(width);
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
    void selectionBorderWidthChanged(int);
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
