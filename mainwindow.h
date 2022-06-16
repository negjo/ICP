#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qbrush.h"
#include "qdebug.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qjsonarray.h"
#include "qlineedit.h"
#include "qmainwindow.h"
#include "qnamespace.h"
#include "qobject.h"
#include "qobjectdefs.h"
#include "qpen.h"
#include "qpolygon.h"
#include "qstyle.h"
#include "qtextedit.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QPlainTextEdit>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QPushButton>
#include <QDebug>
#include <QGraphicsLineItem>
#include <QObjectList>
#include <QGraphicsView>
#include <QVariant>
#include <QPolygonF>
#include <QPolygon>
#include <QPointF>
#include <QBrush>
#include <QPen>
#include <QGraphicsPolygonItem>
#include <QStackedWidget>
#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVector2D>
#include <QVector>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum lineType{
    asoc, aggr, comp, gener, sync, async, ret, creat, del
};

class myTextField: public QPlainTextEdit{
public:
    myTextField(int x, int y, QString name){
        setFixedSize(x,y);
        setPlaceholderText(name);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
};

class myLineEdit: public QLineEdit{
public:
    myLineEdit(int x, int y, QString name, bool frame=true){
        resize(x,y);
        if(!frame){
            setFrame(false);
        }
        setAlignment(Qt::AlignCenter);
        setPlaceholderText(name);
        setMaxLength(x/8);
    }

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //common
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QJsonArray jsonRects();
    QJsonArray jsonLines();
    QJsonArray jsonScenes();
    QJsonArray jsonTimelines(int scenePos);
    QJsonArray jsonMsgs(int scenePos);
    void loadRects(QJsonObject mainObject);
    void loadLines(QJsonObject mainObject);
    void loadSeqScenes(QJsonObject mainObject);
    void deleteEverything();

    //class diagram
    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget;
    void drawLine();
    void resetLineAdd();

    //sequence diagram
    QGraphicsScene *seqScene;
    QGraphicsProxyWidget *seqProxy = new QGraphicsProxyWidget;
    void allowButtons();
    void resetMessageAdd();

private slots:
    //common buttons
    void on_SaveButton_clicked();
    void on_LoadButton_clicked();
    void on_SwitchModeButton_clicked();

    //class diagram buttons
    void on_AddClassButton_clicked();
    void on_CdDeleteItemButton_clicked();
    void on_AddAsocButton_clicked();
    void on_AddAggrButton_clicked();
    void on_AddCompButton_clicked();
    void on_AddGenerButton_clicked();

    //sequence diagram buttons
    void on_AddTimelineButton_clicked();
    void on_SyncMessageButton_clicked();
    void on_AsyncMessageButton_clicked();
    void on_ReturnMessageButton_clicked();
    void on_CreateMessageButton_clicked();
    void on_DeleteMessageButton_clicked();
    void on_NewDiagramButton_clicked();
    void on_DeleteDiagramButton_clicked();
    void on_DiagramList_currentIndexChanged(int i);
    void on_SqDeleteItemButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
