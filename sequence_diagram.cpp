#include "mainwindow.h"
#include "qapplication.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qinputdialog.h"
#include "qlist.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "qpolygon.h"
#include "qvariant.h"
#include "qvector.h"
#include "qvector2d.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include "sequence_diagram.h"


bool seq_delete_mode = false;
bool seq_connect1 = false;
bool seq_connect2 = false;
myTimeline *timeline1;
myTimeline *timeline2;
QList<QGraphicsScene*> sceneList;
QList<QList<myMsgLine*>> msgLineList;
QList<QList<myTimeline*>> timelineList;
int linePos;
extern QList<myRect*> rectList;

myTimeline::~myTimeline(){

}

/**
 * @brief MainWindow::resetMessageAdd
 * Puts message line addition into initial state
 *
 * Reset global bools seq_connect1 and seq_connect2 that are signalizing
 * that line addition is in progress and
 * uncheck all line addition buttons
 */
void MainWindow::resetMessageAdd(){
    ui->SyncMessageButton->setChecked(false);
    ui->AsyncMessageButton->setChecked(false);
    ui->ReturnMessageButton->setChecked(false);
    ui->CreateMessageButton->setChecked(false);
    ui->DeleteMessageButton->setChecked(false);
    seq_connect1 = false;
    seq_connect2 = false;
}

/**
 * @brief MainWindow::allowButtons
 * Enable/Disable all diagram editing buttons
 *
 * Enable/Disable all diagram editing buttons in sequence diagram based on whether any sequence diagrams exist
 */
void MainWindow::allowButtons(){
    if(ui->DiagramList->count() == 0){
        ui->seqDiagramView->setVisible(false);
        ui->DeleteDiagramButton->setEnabled(false);
        ui->AddTimelineButton->setEnabled(false);
        ui->SyncMessageButton->setEnabled(false);
        ui->AsyncMessageButton->setEnabled(false);
        ui->ReturnMessageButton->setEnabled(false);
        ui->CreateMessageButton->setEnabled(false);
        ui->DeleteMessageButton->setEnabled(false);
        ui->SqDeleteItemButton->setEnabled(false);
    }
    else{
        ui->seqDiagramView->setVisible(true);
        ui->DeleteDiagramButton->setEnabled(true);
        ui->AddTimelineButton->setEnabled(true);
        ui->SyncMessageButton->setEnabled(true);
        ui->AsyncMessageButton->setEnabled(true);
        ui->ReturnMessageButton->setEnabled(true);
        ui->CreateMessageButton->setEnabled(true);
        ui->DeleteMessageButton->setEnabled(true);
        ui->SqDeleteItemButton->setEnabled(true);
    }
}

/**
 * @brief MainWindow::on_NewDiagramButton_clicked
 * Create new sequence diagram
 */
void MainWindow::on_NewDiagramButton_clicked(){
    QString name = QInputDialog::getText(this, "New diagram", "Enter name of new diagram:");
    QGraphicsScene *newScene = new QGraphicsScene();
    sceneList.append(newScene);
    QList<myTimeline*> listT;
    QList<myMsgLine*> listM;
    timelineList.append(listT);
    msgLineList.append(listM);
    ui->DiagramList->addItem(name);
    allowButtons();
    ui->DiagramList->setCurrentIndex(ui->DiagramList->count()-1);
}

/**
 * @brief MainWindow::on_DeleteDiagramButton_clicked
 * Delete current sequence diagram
 */
void MainWindow::on_DeleteDiagramButton_clicked(){
    sceneList.removeAt(ui->DiagramList->currentIndex());
    ui->DiagramList->removeItem(ui->DiagramList->currentIndex());
    allowButtons();
}

/**
 * @brief MainWindow::on_DiagramList_currentIndexChanged
 * Set appropriate scene, when new diagram is selected
 * @param i new diagram index
 */
void MainWindow::on_DiagramList_currentIndexChanged(int i){
    if(i != -1){
        ui->seqDiagramView->setScene(sceneList[ui->DiagramList->currentIndex()]);
        seqScene = sceneList[ui->DiagramList->currentIndex()];
    }
}


/**
 * @brief MainWindow::on_AddTimelineButton_clicked
 * Create new timeline
 *
 * Ask for class name and if there exists class with such name connect that class with the new timeline
 * id that class does not exist, show a popup dialog window with further options to create the timeline anyway,
 * cancel creation of timeline or create new class
 */
void MainWindow::on_AddTimelineButton_clicked(){
    bool isOk;
    QString name = QInputDialog::getText(this, "Timeline creation", "Please enter class name", QLineEdit::Normal, QString(), &isOk);
    if(!isOk){
        return;
    }
    bool classExists = false;

    foreach(auto rect, rectList){
        if(rect->className->text() == name){
            classExists = true;
        }
    }
    bool justDoIt = false;
    if(!classExists){
        QMessageBox warning;
        warning.setText("No class with that name exists!");
        warning.setInformativeText("What do you want to do next?");
        QPushButton *doItAnyway = warning.addButton("Just do it!", QMessageBox::ActionRole);
        QPushButton *cancel = warning.addButton("Cancel", QMessageBox::ActionRole);
        QPushButton *createClass = warning.addButton("Create class", QMessageBox::ActionRole);
        warning.exec();
        if(warning.clickedButton() == doItAnyway){
            justDoIt = true;
        }
        else if(warning.clickedButton() == cancel){
            return;
        }
        else if(warning.clickedButton() == createClass){
            on_AddClassButton_clicked();
            rectList.last()->className->setText(name);
        }
    }

    myTimeline *timeline = new myTimeline(ui, seqScene, this);
    connect(timeline->name, &QLineEdit::textChanged, timeline, &myTimeline::seqClassNameChange);
    seqScene->addItem(timeline);
    timelineList[ui->DiagramList->currentIndex()].append(timeline);
    timeline->name->setText(name);
    if(justDoIt){
        timeline->justDoIt = true;
        timeline->name->setStyleSheet("background-color:red;");
    }
    foreach(auto rect, rectList){
        if(rect->className->text() == name){
            timeline->connectedClass = rect;
            return;
        }
    }


}

/**
 * @brief MainWindow::on_SyncMessageButton_clicked
 * Prepare for adding synchronized message line or reset line addition, if one was in progress before
 */
void MainWindow::on_SyncMessageButton_clicked(){
    if(seq_connect1 || seq_connect2){
        resetMessageAdd();
    }
    else{
        ui->SyncMessageButton->setChecked(true);
        seq_connect1 = true;
    }
}

/**
 * @brief MainWindow::on_AsyncMessageButton_clicked
 * Prepare for adding asynchronized message line or reset line addition, if one was in progress before
 */
void MainWindow::on_AsyncMessageButton_clicked(){
    if(seq_connect1 || seq_connect2){
        resetMessageAdd();
    }
    else{
        ui->AsyncMessageButton->setChecked(true);
        seq_connect1 = true;
    }
}

/**
 * @brief MainWindow::on_ReturnMessageButton_clicked
 * Prepare for adding return message line or reset line addition, if one was in progress before
 */
void MainWindow::on_ReturnMessageButton_clicked(){
    if(seq_connect1 || seq_connect2){
        resetMessageAdd();
    }
    else{
        ui->ReturnMessageButton->setChecked(true);
        seq_connect1 = true;
    }
}

/**
 * @brief MainWindow::on_CreateMessageButton_clicked
 * Prepare for adding create message line or reset line addition, if one was in progress before
 */
void MainWindow::on_CreateMessageButton_clicked(){
    if(seq_connect1 || seq_connect2){
        resetMessageAdd();
    }
    else{
        ui->CreateMessageButton->setChecked(true);
        seq_connect1 = true;
    }
}

/**
 * @brief MainWindow::on_DeleteMessageButton_clicked
 * Prepare for adding delete message line or reset line addition, if one was in progress before
 */
void MainWindow::on_DeleteMessageButton_clicked(){
    if(seq_connect1 || seq_connect2){
        resetMessageAdd();
    }
    else{
        ui->DeleteMessageButton->setChecked(true);
        seq_connect1 = true;
    }
}

/**
 * @brief MainWindow::on_SqDeleteItemButton_clicked
 * Switch seq_delete_mode on/off
 */
void MainWindow::on_SqDeleteItemButton_clicked(){
    seq_delete_mode = !seq_delete_mode;
}

/**
 * @brief myTimeline::itemChange
 * Update all message lines connected to this timeline
 * @param change
 * @param value
 * @return
 */
QVariant myTimeline::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == ItemPositionChange){
        myMsgLine *line;
        foreach(line, msgLineList[ui->DiagramList->currentIndex()]){
            if(line->timeline1 == this || line->timeline2 == this){
                line->updateLine();
            }
        }
        QPointF newPos = value.toPointF();
        return QPointF(newPos.x(), 0);
    }
    return QGraphicsItem::itemChange(change, value);
}

/**
 * @brief myTimeline::mousePressEvent
 * Delete clicked timeline or draw line
 * @param event
 *
 * If delete mode is on, this will delete timeline and all it's connections
 * If adding message line is in first step, this will select first timeline to draw line between into global pointer timeline1
 * If adding message line is in second step, this will select second timeline into timeline2 and draw the line
 */
void myTimeline::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(seq_delete_mode){
        myMsgLine *line;
        foreach(line, msgLineList[sceneList.indexOf(scene)]){
            if(line->timeline1 == this || line->timeline2 == this){
                msgLineList[sceneList.indexOf(scene)].removeAll(line);
                delete line;
            }
        }
        timelineList[sceneList.indexOf(scene)].removeAll(this);
        delete(this);
    }
    else if(seq_connect1){
        seq_connect1 = false;
        seq_connect2 = true;
        timeline1 = this;
        linePos = event->pos().y();
    }
    else if(seq_connect2){
        seq_connect2 = false;
        timeline2 = this;
        drawMessage();
    }
}

/**
 * @brief myTimeline::drawMessage
 * Draw message line between two timelines
 *
 * Draw message line between timelines pointed to by global pointer timeline1 and timeline2
 * If message is of return on delete type, preset text will be set to the line and the text will be set to read only
 * Else message will be
 */
void myTimeline::drawMessage(bool askForName){
    myMsgLine *line;
    if(ui->SyncMessageButton->isChecked()){
        ui->SyncMessageButton->setChecked(false);
        line = new myMsgLine(timeline1, timeline2, sync);
    }
    else if(ui->AsyncMessageButton->isChecked()){
        ui->AsyncMessageButton->setChecked(false);
        line = new myMsgLine(timeline1, timeline2, async);
    }
    else if(ui->ReturnMessageButton->isChecked()){
        ui->ReturnMessageButton->setChecked(false);
        line = new myMsgLine(timeline1, timeline2, ret);
    }
    else if(ui->CreateMessageButton->isChecked()){
        ui->CreateMessageButton->setChecked(false);
        line = new myMsgLine(timeline1, timeline2, creat);
    }
    else if(ui->DeleteMessageButton->isChecked()){
        ui->DeleteMessageButton->setChecked(false);
        line = new myMsgLine(timeline1, timeline2, del);
    }
    else{
        exit(1);
    }


    line->text = new myLineEdit(timeline2->pos().x() - timeline1->pos().x() - 30, 20, "text", false);
    if(line->type == del){
        line->text->setText("<<remove>>");
        line->text->setReadOnly(true);
    }
    else if(line->type == ret){
        line->text->setText("<<reply>>");
        line->text->setReadOnly(true);
    }
    else if(askForName){
        bool isOk;
        QString name = QInputDialog::getText(this->mainWindow, "Message creation", "Please enter function name", QLineEdit::Normal, QString(), &isOk);
        if(isOk == false){
            return;
        }
        bool functionExists = false;

        if(!timeline2->justDoIt){
            if(timeline2->connectedClass->functions->toPlainText().contains(name)){
                functionExists = true;
            }
        }

        if(!functionExists){
            QMessageBox warning;
            QPushButton *createFunction;
            warning.setText("No function with that name exists!");
            warning.setInformativeText("What do you want to do next?");
            QPushButton *doItAnyway = warning.addButton("Just do it!", QMessageBox::ActionRole);
            QPushButton *cancel = warning.addButton("Cancel", QMessageBox::ActionRole);
            if(!timeline2->justDoIt){
                createFunction = warning.addButton("Create function", QMessageBox::ActionRole);
            }
            warning.exec();
            if(warning.clickedButton() == doItAnyway){
                line->justDoIt = true;
                line->text->setStyleSheet("background-color:red;");
            }
            else if(warning.clickedButton() == cancel){
                delete(line);
                return;
            }
            else if(warning.clickedButton() == createFunction){
                timeline2->connectedClass->functions->appendPlainText(name);
            }
        }
        line->text->setText(name);

    }
    line->textObject = scene->addWidget(line->text);
    line->textObject->setParentItem(line);
    line->modifyText();

    QPolygonF newPoly = line->createPoly();
    line->poly = scene->addPolygon(newPoly);
    QBrush brush;
    brush.setColor(Qt::black);
    brush.setStyle(Qt::SolidPattern);
    line->poly->setBrush(brush);
    line->poly->setParentItem(line);
    line->poly->setPen(line->pen2);
    scene->addItem(line);
    msgLineList[ui->DiagramList->currentIndex()].append(line);

    if(timeline2->justDoIt){
        line->text->setStyleSheet("background-color:red;");
    }
}

/**
 * @brief myMsgLine::modifyText
 * Update text box above line when line changes
 */
void myMsgLine::modifyText(){
    qreal size = this->timeline2->pos().x() - this->timeline1->pos().x();
    size = size<0?-size:size;
    size -= 100;
    this->textObject->resize(size,20);
    qreal xpos = std::min(this->line().p1().x(), this->line().p2().x());;
    this->textObject->setPos(xpos+40, this->line().p1().y()-24);
    if(this->justDoIt){
        this->text->setStyleSheet("background-color: red;");
    }
}

/**
 * @brief myMsgLine::createPoly
 * Create polygon for end of message line
 * @return created polygon
 */
QPolygonF myMsgLine::createPoly(){
    QPolygonF newPoly;
    qreal x0 = this->line().p2().x();
    qreal y0 = this->line().p2().y();
    qreal xTimeline = this->timeline2->pos().x()+10;
    QPointF p1(x0, y0);
    QPointF p2(x0, y0);
    QPointF p3(x0, y0);
    QPointF p4(x0, y0);

    if(this->type == sync || this->type == del){
        if(x0 < xTimeline){
            p2.setX(x0-20);p2.setY(y0-10);
            p3.setX(x0-20);p3.setY(y0+10);
        }
        else{
            p2.setX(x0+20);p2.setY(y0-10);
            p3.setX(x0+20);p3.setY(y0+10);
        }
        newPoly.append(p1);
        newPoly.append(p2);
        newPoly.append(p3);

    }
    else if(this->type == async || this->type == creat || this->type == ret){
        if(x0 < xTimeline){
            p2.setX(x0-20);p2.setY(y0-10);
            p3.setX(x0-2);
            p4.setX(x0-20);p4.setY(y0+10);
        }
        else{
            p2.setX(x0+20);p2.setY(y0-10);
            p3.setX(x0+2);
            p4.setX(x0+20);p4.setY(y0+10);
        }
        newPoly.append(p1);
        newPoly.append(p2);
        newPoly.append(p3);
        newPoly.append(p4);
    }

    return newPoly;
}

/**
 * @brief myMsgLine::mousePressEvent
 * If seq delete mode is on, this will delete clicked line
 * @param event
 */
void myMsgLine::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(seq_delete_mode){
        foreach(auto list, msgLineList){
            if(int i = list.indexOf(this) != -1){
                msgLineList[msgLineList.indexOf(list)].removeAll(this);
            }
        }
        delete(this);
    }
}

/**
 * @brief myMsgLine::updateLine
 * Update line when position of one of connected timelines changes
 */
void myMsgLine::updateLine(){
    qreal x1 = this->timeline1->pos().x();
    qreal x2 = this->timeline2->pos().x();
    qreal y;

    if(this->lineExists){
        y = this->line().p1().y();
    }
    else{
        y = linePos;
    }

    if(x1 < x2){
        x1 += 20;
    }
    else{
        x2 += 20;
    }

    setLine(x1, y, x2, y);

    if(this->polyExists){
        QPolygonF updatedPoly = createPoly();
        this->poly->setPolygon(updatedPoly);
        this->modifyText();
    }
}

/**
 * @brief myMsgLine::itemChange
 * Allow lines to be dragged only on Y axis
 * @param change
 * @param value
 * @return
 */
QVariant myMsgLine::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value){
    if(change == ItemPositionChange){
        QPointF newPos = value.toPointF();
        return QPointF(this->pos().x(), newPos.y());
    }
    return QGraphicsItem::itemChange(change, value);
}

/**
 * @brief myTimeline::seqClassNameChange
 * When class name in sequence diagram changes, change the name in class daigram aswell
 * @param text new name
 */
void myTimeline::seqClassNameChange(QString text){

    if(this->connectedClass != nullptr){
        this->connectedClass->className->setText(text);
    }
}
