#include "mainwindow.h"
#include "qapplication.h"
#include "qdebug.h"
#include "qglobal.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qjsonvalue.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qpoint.h"
#include "qpolygon.h"
#include "qvariant.h"
#include "ui_mainwindow.h"
#include "class_diagram.h"
#include "sequence_diagram.h"

extern QList<myRect*> rectList;
extern QList<myLine*> lineList;
extern QList<QGraphicsScene*> sceneList;
extern QList<QList<myMsgLine*>> msgLineList;
extern QList<QList<myTimeline*>> timelineList;
extern myRect *point1;
extern myRect *point2;
extern myTimeline *timeline1;
extern myTimeline *timeline2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    MainWindow::setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    ui->buttons->setCurrentIndex(1);
    ui->views->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_SaveButton_clicked
 * Creates a file and saves all the diagrams in json format
 */
void MainWindow::on_SaveButton_clicked(){
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save diagram"), "diagram.json", tr("Diagram (*.json);;All Files (*)"));
    if(fileName.isEmpty()){
        return;
    }
    else{
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QJsonDocument jsonDoc;
        QJsonObject mainObject;

        QJsonArray arrayOfRects = jsonRects();
        mainObject.insert("rects", arrayOfRects);

        QJsonArray arrayOfLines = jsonLines();
        mainObject.insert("lines", arrayOfLines);

        QJsonArray arrayOfScenes = jsonScenes();
        mainObject.insert("scenes", arrayOfScenes);

        jsonDoc.setObject(mainObject);
        file.write(jsonDoc.toJson());
    }

}

/**
 * @brief MainWindow::jsonRects
 * Parses all class boject into json array
 * @return json array of all class objects
 */
QJsonArray MainWindow::jsonRects(){
    QJsonArray array;
    myRect *rect;
    int rectCnt = 0;
    foreach(rect, rectList){
        QJsonObject rectObject;
        rectObject.insert("class name", rect->className->text());
        rectObject.insert("x", rect->pos().x());
        rectObject.insert("y", rect->pos().y());
        rectObject.insert("variables", rect->variables->toPlainText());
        rectObject.insert("functions", rect->functions->toPlainText());
        array.insert(rectCnt, rectObject);
        rectCnt++;
    }
    return array;
}

/**
 * @brief MainWindow::jsonLines
 * Parses all lines in class diagram into json array
 * @return json array of all lines connecting classes
 */
QJsonArray MainWindow::jsonLines(){
    QJsonArray array;
    myLine *line;
    int lineCnt = 0;
    foreach(line, lineList){
        QJsonObject lineObject;
        lineObject.insert("rect 1", rectList.indexOf(line->rect1));
        lineObject.insert("rect 2", rectList.indexOf(line->rect2));
        lineObject.insert("type", line->type);
        array.insert(lineCnt, lineObject);
        lineCnt++;
    }
    return array;
}

/**
 * @brief MainWindow::jsonScenes
 * Parse all scenes and they're contents in sequence diagrams into json array
 * @return json array of all scenes
 */
QJsonArray MainWindow::jsonScenes(){
    QJsonArray array;
    int sceneCnt = 0;
    for(int i = 0; i < sceneList.count(); i++){
        ui->seqDiagramView->setScene(sceneList[i]);
        QJsonObject sceneObject;
        sceneObject.insert("name", ui->DiagramList->itemText(i));

        QJsonArray timelineArray = jsonTimelines(i);
        sceneObject.insert("timelines", timelineArray);

        QJsonArray lineArray = jsonMsgs(i);
        sceneObject.insert("lines", lineArray);

        array.insert(sceneCnt, sceneObject);
        sceneCnt++;
    }
    return array;
}

/**
 * @brief MainWindow::jsonTimelines
 * Parse all timelines in current scenes into json array
 * @param scenePos scene to parse
 * @return json array of timelines
 */
QJsonArray MainWindow::jsonTimelines(int scenePos){
    QJsonArray timelineArray;
    myTimeline *t;
    int timelineCnt = 0;
    foreach(t, timelineList[scenePos]){
        if(sceneList.indexOf(t->scene) == scenePos){
            QJsonObject timelineObject;
            timelineObject.insert("name", t->name->text());
            timelineObject.insert("pos", t->pos().x());
            timelineArray.insert(timelineCnt, timelineObject);
            timelineCnt++;
        }
    }
    return timelineArray;
}

/**
 * @brief MainWindow::jsonMsgs
 * Parse all message lines in sequnece diagram into json array
 * @param scenePos scene to parse
 * @return json array of message lines
 */
QJsonArray MainWindow::jsonMsgs(int scenePos){
    QJsonArray lineArray;
    myMsgLine *l;
    int lineCnt = 0;
    foreach(l, msgLineList[scenePos]){
        if(sceneList.indexOf(l->timeline1->scene) == scenePos){
            QJsonObject lineObject;
            lineObject.insert("timeline 1", timelineList[scenePos].indexOf(l->timeline1));
            lineObject.insert("timeline 2", timelineList[scenePos].indexOf(l->timeline2));
            lineObject.insert("type", l->type);
            lineObject.insert("pos", l->line().p1().y());
            lineObject.insert("text", l->text->text());
            lineArray.insert(lineCnt, lineObject);
            lineCnt++;
        }
    }
    return lineArray;
}

/**
 * @brief MainWindow::on_LoadButton_clicked
 * Open file and load diagrams
 */
void MainWindow::on_LoadButton_clicked(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load diagram"), "diagram.json", tr("Diagram (*.json);;All Files (*)"));
    if(fileName.isEmpty()){
        return;
    }
    else{
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)){
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        deleteEverything();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        QJsonObject mainObject = jsonDoc.object();
        loadRects(mainObject);
        loadLines(mainObject);
        loadSeqScenes(mainObject);
        file.close();
    }
}

/**
 * @brief MainWindow::deleteEverything
 * Delete all items in all diagrams
 */
void MainWindow::deleteEverything(){
    foreach(auto ptr, lineList){
        delete ptr;
    }
    foreach(auto ptr, rectList){
        delete ptr;
    }
    foreach(auto list, msgLineList){
        foreach(auto ptr, list){
            delete ptr;
        }
        list.clear();
    }
    foreach(auto list, timelineList){

        foreach(auto ptr, list){
            delete ptr;
        }
        list.clear();
    }
    foreach(auto ptr, sceneList){
        delete ptr;
    }

    rectList.clear();
    lineList.clear();
    msgLineList.clear();
    timelineList.clear();
    sceneList.clear();
    ui->DiagramList->clear();
}

/**
 * @brief MainWindow::loadRects
 * Load class objects from json
 * @param mainObject main json object
 */
void MainWindow::loadRects(QJsonObject mainObject){
    QJsonArray arrayOfRects = mainObject.value("rects").toArray();
    QJsonValue tmp;
    foreach(tmp, arrayOfRects){
        QJsonObject rectObject = tmp.toObject();
        QString name = rectObject.value("class name").toString();
        int xpos = rectObject.value("x").toInt();
        int ypos = rectObject.value("y").toInt();
        QString variables = rectObject.value("variables").toString();
        QString functions = rectObject.value("functions").toString();

        on_AddClassButton_clicked();
        myRect *rect = rectList.last();
        rect->setPos(xpos, ypos);
        rect->className->setText(name);
        rect->variables->setPlainText(variables);
        rect->functions->setPlainText(functions);
    }
}

/**
 * @brief MainWindow::loadLines
 * Load lines in class diagrams from json
 * @param mainObject main json object
 */
void MainWindow::loadLines(QJsonObject mainObject){
    QJsonArray arrayOfLines = mainObject.value("lines").toArray();
    QJsonValue tmp;
    foreach(tmp, arrayOfLines){
        QJsonObject lineObject = tmp.toObject();
        int rect1 = lineObject.value("rect 1").toInt();
        int rect2 = lineObject.value("rect 2").toInt();
        lineType type = static_cast<lineType>(lineObject.value("type").toInt());
        point1 = rectList[rect1];
        point2 = rectList[rect2];
        if(type == asoc){
            ui->AddAsocButton->setChecked(true);
        }
        else if(type == aggr){
            ui->AddAggrButton->setChecked(true);
        }
        else if(type == comp){
            ui->AddCompButton->setChecked(true);
        }
        else if(type == gener){
            ui->AddGenerButton->setChecked(true);
        }

        myRect *tmpRect = new myRect(ui, scene);
        tmpRect->drawLine();
    }
}

/**
 * @brief MainWindow::loadSeqScenes
 * Load all scenes, timelines and lines in sequence diagrams from json
 * @param mainObject main json object
 */
void MainWindow::loadSeqScenes(QJsonObject mainObject){
    QJsonArray arrayOfScenes = mainObject.value("scenes").toArray();
    foreach(auto tmpSceneValue, arrayOfScenes){

        QJsonObject sceneObject = tmpSceneValue.toObject();
        QString name = sceneObject.value("name").toString();

        QGraphicsScene *newScene = new QGraphicsScene();
        sceneList.append(newScene);

        QList<myTimeline*> listT;
        QList<myMsgLine*> listM;
        timelineList.append(listT);
        msgLineList.append(listM);

        ui->DiagramList->addItem(name);
        ui->DiagramList->setCurrentIndex(ui->DiagramList->count()-1);
        allowButtons();

        QJsonArray arrayOfTimelines = sceneObject.value("timelines").toArray();
        foreach(auto tmpTimelineValue, arrayOfTimelines){

            QJsonObject timelineObject = tmpTimelineValue.toObject();
            QString name = timelineObject.value("name").toString();
            int pos = timelineObject.value("pos").toInt();

            myTimeline *timeline = new myTimeline(ui, seqScene, this);
            newScene->addItem(timeline);
            timeline->name->setText(name);
            timeline->setPos(pos, 0);
            timelineList[sceneList.indexOf(newScene)].append(timeline);

            bool classExists = false;
            foreach(auto rect, rectList){
                if(rect->className->text() == name){
                    classExists = true;
                    timeline->connectedClass = rect;
                }
            }
            if(!classExists){
                timeline->justDoIt = true;
                timeline->name->setStyleSheet("background-color:red;");
            }
        }

        QJsonArray arrayOfMsgs = sceneObject.value("lines").toArray();
        foreach(auto tmpMsgValue, arrayOfMsgs){
            QJsonObject msgObject = tmpMsgValue.toObject();
            QString text = msgObject.value("text").toString();
            int pos = msgObject.value("pos").toInt();
            int t1 = msgObject.value("timeline 1").toInt();
            int t2 = msgObject.value("timeline 2").toInt();
            lineType type = static_cast<lineType>(msgObject.value("type").toInt());
            timeline1 = timelineList[sceneList.indexOf(newScene)][t1];
            timeline2 = timelineList[sceneList.indexOf(newScene)][t2];

            if(type == sync){
                ui->SyncMessageButton->setChecked(true);
            }
            else if(type == async){
                ui->AsyncMessageButton->setChecked(true);
            }
            else if(type == ret){
                ui->ReturnMessageButton->setChecked(true);
            }
            else if(type == creat){
                ui->CreateMessageButton->setChecked(true);
            }
            else if(type == del){
                ui->DeleteMessageButton->setChecked(true);
            }

            timeline2->scene = newScene;
            timeline2->drawMessage(false);

            myMsgLine *msg = msgLineList.last().last();

            msg->text->setText(text);
            msg->setPos(msg->pos().x(), pos);

            if(timeline2->connectedClass == nullptr){
                msg->justDoIt = true;
                msg->text->setStyleSheet("background-color:red;");
            }
            else if(type == sync || type == async || type == creat){
                if(!timeline2->connectedClass->functions->toPlainText().contains(text)){
                    msg->justDoIt = true;
                    msg->text->setStyleSheet("background-color:red;");
                }
            }
        }
    }
}

/**
 * @brief MainWindow::on_SwitchModeButton_clicked
 * Switch between class diagram and sequence diagrams
 */
void MainWindow::on_SwitchModeButton_clicked(){
    if(ui->buttons->currentIndex() == 0){
        ui->buttons->setCurrentIndex(1);
    }
    else{
        ui->buttons->setCurrentIndex(0);
    }

    if(ui->views->currentIndex() == 0){
        ui->views->setCurrentIndex(1);
    }
    else{
        ui->views->setCurrentIndex(0);
    }
    if(ui->SwitchModeButton->text() == "Sequence diagrams"){
        ui->SwitchModeButton->setText("Class diagram");
    }
    else{
        ui->SwitchModeButton->setText("Sequence diagrams");
    }
}




