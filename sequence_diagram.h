#ifndef SEQUENCE_DIAGRAM_H
#define SEQUENCE_DIAGRAM_H

#include "mainwindow.h"
#include "class_diagram.h"


class myTimeline: public QObject, public QGraphicsRectItem{
    Q_OBJECT
public:
    myTimeline(Ui::MainWindow *ui, QGraphicsScene *scene, QMainWindow *mainWindow){
        this->ui = ui;
        this->scene = scene;
        this->mainWindow = mainWindow;
        setFlag(QGraphicsRectItem::ItemIsMovable);
        setFlag(QGraphicsRectItem::ItemSendsScenePositionChanges);
        setRect(0,-700,20,700);
        brush.setColor(Qt::cyan);
        brush.setStyle(Qt::SolidPattern);
        setBrush(brush);
        name = new myLineEdit(120,20,"name", true);
        auto a = scene->addWidget(name);
        a->setParentItem(this);
        a->setPos(-50,-740);
    }
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QMainWindow *mainWindow;
    ~myTimeline();
    myLineEdit *name;
    void drawMessage(bool askForName = true);
    bool justDoIt = false;
    myRect *connectedClass = nullptr;

public slots:
    void seqClassNameChange(QString text);

protected:
    QBrush brush;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};



class myMsgLine: public QGraphicsLineItem{
public:
    myMsgLine();
    myMsgLine(myTimeline *timeline1, myTimeline *timeline2, lineType t){
        this->timeline1 = timeline1;
        this->timeline2 = timeline2;
        this->type = t;
        updateLine();
        lineExists = true;
        polyExists = true;
        pen1.setWidth(2);
        pen2.setWidth(2);
        if(t == creat || t == ret){
            pen1.setStyle(Qt::DashLine);
        }
        setFlag(QGraphicsRectItem::ItemSendsScenePositionChanges);
        setFlag(QGraphicsLineItem::ItemIsMovable);
        setPen(pen1);

    }
    void updateLine();
    myTimeline *timeline1;
    myTimeline *timeline2;
    QPolygonF createPoly();
    bool polyExists = false;
    bool lineExists = false;
    QGraphicsPolygonItem *poly;
    QPen pen1;
    QPen pen2;
    lineType type;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    myLineEdit *text;
    QGraphicsProxyWidget *textObject;
    void modifyText();
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    bool justDoIt = false;
};


#endif // SEQUENCE_DIAGRAM_H
