#ifndef CLASS_DIAGRAM_H
#define CLASS_DIAGRAM_H

#include "mainwindow.h"

class myRect: public QObject, public QGraphicsRectItem{
    Q_OBJECT
public:
    myRect(Ui::MainWindow *ui, QGraphicsScene *scene){
        this->ui = ui;
        this->scene = scene;
        setFlag(QGraphicsRectItem::ItemIsMovable);
        setFlag(QGraphicsRectItem::ItemSendsScenePositionChanges);
        setRect(-4,-4,108,148);
    }
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    myLineEdit *className;
    myTextField *variables;
    myTextField *functions;
    void drawLine();
    void drawAsocLine();
    void drawAggrLine();
    void drawCompLine();
    void drawGenerLine();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
public slots:
    void rectClassNameChange(QString text);
};

class myLine: public QGraphicsLineItem{
public:
    myLine();
    myLine(myRect *rect1, myRect *rect2, lineType t){
        this->rect1 = rect1;
        this->rect2 = rect2;
        this->type = t;
        updateLine();
        polyExists = true;
        p.setWidth(2);
        setPen(p);
    }

    void updateLine();
    QPolygonF createPoly();
    myRect *rect1;
    myRect *rect2;
    lineType type;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    bool polyExists = false;
    QGraphicsPolygonItem *poly;
    QPen p;
};




#endif // CLASS_DIAGRAM_H
