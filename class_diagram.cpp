#include "mainwindow.h"
#include "qapplication.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qlineedit.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "qpolygon.h"
#include "qvariant.h"
#include "ui_mainwindow.h"
#include "class_diagram.h"
#include "sequence_diagram.h"

bool delete_mode = false;
bool connect1 = false;
bool connect2 = false;
myRect *point1;
myRect *point2;
QList<myRect*> rectList;
QList<myLine*> lineList;
extern QList<QList<myMsgLine*>> msgLineList;
extern QList<QList<myTimeline*>> timelineList;

/**
 * @brief MainWindow::resetLineAdd
 * Puts line addition into the initial state
 *
 * Reset global bools connect1 and connect2 that are signalizing
 * that line addition is in progress and
 * uncheck all line addition buttons
 */
void MainWindow::resetLineAdd(){
    connect1 = false;
    connect2 = false;
    ui->AddAsocButton->setChecked(false);
    ui->AddAggrButton->setChecked(false);
    ui->AddCompButton->setChecked(false);
    ui->AddGenerButton->setChecked(false);
}

/**
 * @brief MainWindow::on_AddClassButton_clicked
 * Create new class widget
 */
void MainWindow::on_AddClassButton_clicked(){
    myRect *rectItem = new myRect(ui, scene);
    rectList.append(rectItem);

    rectItem->className = new myLineEdit(100, 20, "class", true);
    connect(rectItem->className, &QLineEdit::textChanged, rectItem, &myRect::rectClassNameChange);
    rectItem->variables = new myTextField(100,60, "variables");
    rectItem->functions = new myTextField(100,60, "functions");

    proxy = scene->addWidget(rectItem->className);
    proxy->setParentItem(rectItem);

    proxy = scene->addWidget(rectItem->variables);
    proxy->setPos(0,20);
    proxy->setParentItem(rectItem);

    proxy = scene->addWidget(rectItem->functions);
    proxy->setPos(0,80);
    proxy->setParentItem(rectItem);

    scene->addItem(rectItem);
    ui->classDiagramView->setScene(scene);
    ui->classDiagramView->show();
}

/**
 * @brief MainWindow::on_AddAsocButton_clicked
 * Prepare for adding asociacion line or reset line addition, if one was in progress before
 */
void MainWindow::on_AddAsocButton_clicked(){
    if(connect1 || connect2){
        resetLineAdd();
    }
    else{
        ui->AddAsocButton->setChecked(true);
        connect1 = true;
    }
}

/**
 * @brief MainWindow::on_AddAsocButton_clicked
 * Prepare for adding aggregation line or reset line addition, if one was in progress before
 */
void MainWindow::on_AddAggrButton_clicked(){
    if(connect1 || connect2){
        resetLineAdd();
    }
    else{
        ui->AddAggrButton->setChecked(true);
        connect1 = true;
    }
}

/**
 * @brief MainWindow::on_AddAsocButton_clicked
 * Prepare for adding composition line or reset line addition, if one was in progress before
 */
void MainWindow::on_AddCompButton_clicked(){
    if(connect1 || connect2){
        resetLineAdd();
    }
    else{
        ui->AddCompButton->setChecked(true);
        connect1 = true;
    }
}

/**
 * @brief MainWindow::on_AddAsocButton_clicked
 * Prepare for adding generalization line or reset line addition, if one was in progress before
 */
void MainWindow::on_AddGenerButton_clicked(){
    if(connect1 || connect2){
        resetLineAdd();
    }
    else{
        ui->AddGenerButton->setChecked(true);
        connect1 = true;
    }
}

/**
 * @brief MainWindow::on_CdDeleteItemButton_clicked
 * Switch global bool delete_mode on/off
 */
void MainWindow::on_CdDeleteItemButton_clicked(){
    delete_mode = !delete_mode;
}

/**
 * @brief myRect::mousePressEvent
 * Delete object or draw line
 * @param event
 *
 * If delete mode is on, this will delete class object and all it's connections
 * If adding line is in first step, this will select first object to draw line between into global pointer point1
 * If adding line is in second step, this will select second object into point2 and draw the line
 */
void myRect::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(delete_mode){
        myLine *line;
        foreach(line, lineList){
            if(line->rect1 == this || line->rect2 == this){
                delete(line);
            }
        }
        rectList.removeAll(this);
        delete(this);
    }
    else if(connect1){
        point1 = this;
        connect1 = false;
        connect2 = true;

    }
    else if(connect2){
        point2 = this;
        connect2 = false;
        drawLine();
    }
}


/**
 * @brief myRect::drawLine
 * Draw line between objects at global pointers point1 and point2
 */
void myRect::drawLine(){
    if(ui->AddAsocButton->isChecked()){
        drawAsocLine();
    }
    else if(ui->AddAggrButton->isChecked()){
        drawAggrLine();
    }
    else if(ui->AddCompButton->isChecked()){
        drawCompLine();
    }
    else if(ui->AddGenerButton->isChecked()){
        drawGenerLine();
    }
    else{
        //this should never happen
        exit(1);
    }
}
/**
 * @brief myRect::drawAsocLine
 * Draw asociation line
 */
void myRect::drawAsocLine(){
    myLine *line;
    line = new myLine(point1, point2, asoc);
    ui->AddAsocButton->setChecked(false);
    lineList.append(line);
    scene->addItem(line);
}

/**
 * @brief myRect::drawAggrLine
 * Draw aggegation line
 */
void myRect::drawAggrLine(){
    myLine *line;
    line = new myLine(point1, point2, aggr);
    ui->AddAggrButton->setChecked(false);
    QPolygonF newPoly = line->createPoly();
    line->poly = scene->addPolygon(newPoly);
    line->poly->setParentItem(line);
    line->poly->setPen(line->p);
    lineList.append(line);
    scene->addItem(line);
}

/**
 * @brief myRect::drawCompLine
 * Draw composition line
 */
void myRect::drawCompLine(){
    myLine *line;
    line = new myLine(point1, point2, comp);
    ui->AddCompButton->setChecked(false);
    QPolygonF newPoly = line->createPoly();
    line->poly = scene->addPolygon(newPoly);
    QBrush brush;
    brush.setColor(Qt::black);
    brush.setStyle(Qt::SolidPattern);
    line->poly->setBrush(brush);
    line->poly->setParentItem(line);
    line->poly->setPen(line->p);
    lineList.append(line);
    scene->addItem(line);
}

/**
 * @brief myRect::drawGenerLine
 * Draw generalisation line
 */
void myRect::drawGenerLine(){
    myLine *line;
    line = new myLine(point1, point2, gener);
    ui->AddGenerButton->setChecked(false);
    QPolygonF newPoly = line->createPoly();
    line->poly = scene->addPolygon(newPoly);
    line->poly->setParentItem(line);
    line->poly->setPen(line->p);
    lineList.append(line);
    scene->addItem(line);
}


/**
 * @brief myRect::itemChange
 * Update all lines connected to this object when it is moved
 * @param change
 * @param value
 * @return
 */
QVariant myRect::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == ItemPositionChange){
        myLine *line;
        foreach(line, lineList){
            if(line->rect1 == this || line->rect2 == this){
                line->updateLine();
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}


/**
 * @brief myLine::createPoly
 * Create polygon that will be used for ending of line
 * @return the polygon
 *
 * Create polygon for end of line based on type of line and direction
 */
QPolygonF myLine::createPoly(){
    QPolygonF newPoly;
    qreal x0 = this->line().p2().x();
    qreal y0 = this->line().p2().y();
    qreal xRect = this->rect2->pos().x()+50;
    qreal yRect = this->rect2->pos().y()+50;
    QPointF p1(x0, y0);
    QPointF p2(x0, y0);
    QPointF p3(x0, y0);
    QPointF p4(x0, y0);
    if(this->type == aggr || this->type == comp){
        if(x0 > xRect && y0 > yRect){ //dole vpravo
            p2.setY(y0-20);
            p3.setX(x0-20);p3.setY(y0-20);
            p4.setX(x0-20);
        }
        else if(x0 > xRect && y0 < yRect){ //nahoře vpravo
            p2.setY(y0+20);
            p3.setX(x0-20);p3.setY(y0+20);
            p4.setX(x0-20);
        }
        else if(x0 < xRect && y0 > yRect){ //dole vlevo
            p2.setY(y0-20);
            p3.setX(x0+20);p3.setY(y0-20);
            p4.setX(x0+20);
        }
        else if(x0 < xRect && y0 < yRect){ //nahoře vlevo
            p2.setY(y0+20);
            p3.setX(x0+20);p3.setY(y0+20);
            p4.setX(x0+20);
        }
        else if(x0 < xRect && y0 == yRect){ //vlevo
            p2.setX(x0+10);p2.setY(y0+10);
            p3.setX(x0+20);
            p4.setX(x0+10);p4.setY(y0-10);
        }
        else if(x0 > xRect && y0 == yRect){ //vpravo
            p2.setX(x0-10);p2.setY(y0+10);
            p3.setX(x0-20);
            p4.setX(x0-10);p4.setY(y0-10);
        }
        else if(x0 == xRect && y0 > yRect){ //dole
            p2.setX(x0-10);p2.setY(y0-10);
            p3.setY(y0-20);
            p4.setX(x0+10);p4.setY(y0-10);
        }
        else if(x0 == xRect && y0 < yRect){ //nahoře
            p2.setX(x0-10);p2.setY(y0+10);
            p3.setY(y0+20);
            p4.setX(x0+10);p4.setY(y0+10);
        }

        newPoly.append(p1);
        newPoly.append(p2);
        newPoly.append(p3);
        newPoly.append(p4);
    }
    else if(this->type == gener){
        if(x0 > xRect && y0 > yRect){ //dole vpravo
            p1.setX(x0-10);p1.setY(y0+10);
            p2.setX(x0-15);p2.setY(y0-15);
            p3.setX(x0+10);p3.setY(y0-10);
        }
        else if(x0 > xRect && y0 < yRect){ //nahoře vpravo
            p1.setX(x0-10);p1.setY(y0-10);
            p2.setX(x0-15);p2.setY(y0+15);
            p3.setX(x0+10);p3.setY(y0+10);
        }
        else if(x0 < xRect && y0 > yRect){ //dole vlevo
            p1.setX(x0-10);p1.setY(y0-10);
            p2.setX(x0+15);p2.setY(y0-15);
            p3.setX(x0+10);p3.setY(y0+10);
        }
        else if(x0 < xRect && y0 < yRect){ //nahoře vlevo
            p1.setX(x0-10);p1.setY(y0+10);
            p2.setX(x0+15);p2.setY(y0+15);
            p3.setX(x0+10);p3.setY(y0-10);
        }
        else if(x0 < xRect && y0 == yRect){ //vlevo
            p1.setY(y0+10);
            p2.setX(x0+15);
            p3.setY(y0-10);
        }
        else if(x0 > xRect && y0 == yRect){ //vpravo
            p1.setY(y0+10);
            p2.setX(x0-15);
            p3.setY(y0-10);
        }
        else if(x0 == xRect && y0 > yRect){ //dole
            p1.setX(x0+10);
            p2.setY(y0-15);
            p3.setX(x0-10);
        }
        else if(x0 == xRect && y0 < yRect){ //nahoře
            p1.setX(x0+10);
            p2.setY(y0+15);
            p3.setX(x0-10);
        }

        newPoly.append(p1);
        newPoly.append(p2);
        newPoly.append(p3);
    }
    return newPoly;
}

/**
 * @brief myLine::mousePressEvent
 * If delete mode is on, delete this line
 * @param event
 */
void myLine::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(delete_mode){
        delete(this);
    }
}

/**
 * @brief myLine::updateLine
 * Update line
 */
void myLine::updateLine(){
    qreal x1 = this->rect1->pos().x()+50;
    qreal y1 = this->rect1->pos().y()+50;
    qreal x2 = this->rect2->pos().x()+50;
    qreal y2 = this->rect2->pos().y()+50;
    qreal x1ofset = 0;
    qreal y1ofset = 0;
    qreal x2ofset = 0;
    qreal y2ofset = 0;
    qreal xdif = x1-x2;
    xdif = xdif<0?-xdif:xdif;
    qreal ydif = y1-y2;
    ydif = ydif<0?-ydif:ydif;
    qreal xydif = xdif-ydif;
    xydif = xydif<0?-xydif:xydif;
    qreal ofset1 = 54;
    qreal ofset2 = 54;
    if(this->type == asoc){
        ofset2 = 54;
    }
    else if(this->type == aggr){
        ofset2 = 74;
    }
    else if(this->type == comp){
        ofset2 = 74;
    }
    else if(this->type == gener){
        ofset2 = 69;
    }

    if(xydif < 100){
        if(x1 < x2){
            x1ofset = ofset1;
            x2ofset = -ofset2;
        }
        else{
            x1ofset = -ofset1;
            x2ofset = ofset2;
        }
        if(y1 < y2){
            y1ofset = ofset1+40;
            y2ofset = -ofset2;
        }
        else{
            y1ofset = -ofset1;
            y2ofset = ofset2+40;
        }
    }
    else if(xdif > ydif){
        if(x1 < x2){
            x1ofset = ofset1;
            x2ofset = -ofset2;
        }
        else{
            x1ofset = -ofset1;
            x2ofset = ofset2;
        }
    }
    else{
        if(y1 < y2){
            y1ofset = ofset1+40;
            y2ofset = -ofset2;
        }
        else{
            y1ofset = -ofset1;
            y2ofset = ofset2+40;
        }
    }

    setLine(x1+x1ofset, y1+y1ofset, x2+x2ofset, y2+y2ofset);

    if(this->polyExists && this->type != asoc){
        QPolygonF updatedPoly = createPoly();
        this->poly->setPolygon(updatedPoly);
    }
}


/**
 * @brief myRect::rectClassNameChange
 * Check for name conflicts and update all timelines in sequence diagrams connected to this object
 * @param text new text
 */
void myRect::rectClassNameChange(QString text){
    if(text == ""){
        this->className->setStyleSheet("background-color: red;");
        return;
    }
    foreach(auto rect, rectList){
        if(rect != this && rect->className->text() == text){
            //rect->className->setStyleSheet("background-color: red;");
            this->className->setStyleSheet("background-color: red;");
            return;
        }
    }
    foreach(auto list, timelineList){
        foreach(auto timeline, list){
            if(timeline->connectedClass == this){
                timelineList[timelineList.indexOf(list)][list.indexOf(timeline)]->name->setText(text);
            }
        }
    }
    this->className->setStyleSheet("");
}
