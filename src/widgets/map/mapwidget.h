#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGraphicsItem>
#include <QList>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "map2d.h"
#include "tileprovider.h"
#include "maplayercontrol.h"
#include "imagebutton.h"
#include <QCursor>
#include "configurable.h"
#include "lock_button.h"
#include "papget.h"

class ACItemManager;
class ItemEditStateMachine;
class MapItem;
class LayerCombo;

enum InteractionState {
    PMIS_FLIGHT_PLAN_EDIT,
    PMIS_FROZEN,
    PMIS_OTHER,
};

enum PanState {
    PAN_IDLE,
    PAN_PRESSED,
    PAN_MOVE
};

class MapWidget : public Map2D, public Configurable
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);

    void addItem(MapItem* map_item);
    void removeItem(MapItem* item);
    virtual void setCursor(const QCursor &);
    void setPanMask(int mask) {pan_mouse_mask = mask; setMouseLoadTileMask(mask);}
    void itemsForbidHighlight(bool fh);
    void itemsEditable(bool ed);
    void updateHighlights(QString ac_id);
    MapScene* scene() {return _scene;}
    void centerLatLon(Point2DLatLon latLon) override;
    void setZoom(double z) override;

    void configure(QDomElement) override;

signals:
    void mouseMoved(QPointF scenePos);
    void itemAdded(MapItem* map_item);
    void itemRemoved(MapItem* item);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void changeCurrentAC(QString id);
    void updateAircraftItem(pprzlink::Message msg);
    void moveWaypoint(pprzlink::Message msg);
    void updateTarget(pprzlink::Message msg);
    void updateNavShape(pprzlink::Message msg);

private:

    enum WidgetContainer {
        WIDGETS_LEFT,
        WIDGETS_RIGHT,
    };

    void handleNewAC(QString ac_id);
    void removeAC(QString ac_id);
    LayerCombo* makeLayerCombo();
    void addWidget(QWidget* w, LockButton* button, WidgetContainer side);
    void setEditorMode();
    void registerWaypoint(WaypointItem* waypoint);
    void updateGraphics();

    QMap<QString, ACItemManager*> ac_items_managers;
    InteractionState interaction_state;
    int drawState;
    ItemEditStateMachine* fp_edit_sm;
    QString current_ac;

    QList<MapItem*> _items;
    QList<Papget*> papgets;

    QHBoxLayout* horizontalLayout;

    //ButtonBand
    QVBoxLayout* buttonsLeftLayout;
    QVBoxLayout* columnLeft;
    QVBoxLayout* columnRight;
    QVBoxLayout* buttonsRightLayout;

    QList<LockButton*> buttonsLeft;
    QList<LockButton*> buttonsRight;

    QPoint lastPos;
    PanState pan_state;
    int pan_mouse_mask;

};

#endif // MAPWIDGET_H
