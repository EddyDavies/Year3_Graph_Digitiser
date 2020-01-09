#ifndef DIGITISER_H
#define DIGITISER_H
#include "customscene.h"

#include <QDebug>
#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QApplication>



QT_BEGIN_NAMESPACE
namespace Ui { class Digitiser; }
QT_END_NAMESPACE

class Digitiser : public QMainWindow
{
    Q_OBJECT

public:
    Digitiser(QWidget *parent = nullptr);
    ~Digitiser();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void on_actionOpen_triggered();
    void on_actionCalibrate_triggered();
    void on_calibrateButton_clicked();
    void on_resetCalibration_clicked();
    void viewClicked(QPointF);

    void on_actionSave_triggered();

    void on_actionSelect_Points_triggered();

private:
    enum Group{cali, selectPoints, save};

    void createGroups();
    void selectGroup(Group group=cali);
    void noGraphLoaded();
    void calibrate(double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset);

    Ui::Digitiser *ui;
    QGraphicsScene *scene;

    QVector<QPointF> axis;
    double sizeY;
    double sizeX;
    double xPixToGraph;
    double xGraphToPix;
    double yPixToGraph;
    double yGraphToPix;
    double xOffset;
    double yOffset;
    bool opened = false;
    bool calibrating = false;
    bool calibrated = false;
    bool selectingPoints = false;
};
#endif // DIGITISER_H
