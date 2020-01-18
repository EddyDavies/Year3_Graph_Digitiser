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
#include <QDoubleSpinBox>



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
    void on_clearPoints_clicked();
    void on_xLinear_toggled(bool checked);
    void on_yLinear_toggled(bool checked);
    void on_xLog_toggled(bool checked);
    void on_yLog_toggled(bool checked);


    void on_actionAutomatic_triggered();

private:
    enum Groups{cali, selectPoints, automated};
    Groups group = cali;
//    void setSizes();
//    void mousePressEvent(QMouseEvent *event);

    void createGroups();
    void selectGroup(Groups group=cali);
    void calibrate(bool log, double logBase, double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset);
    void noGraphLoaded();
    void notCalibrated();
    void convertPoint(QPointF pixelPoint);
    void write(QVector<QPointF> &points);
    double baseLog(double base, double x);
//    void pointColour(QPoint floatPoint);


    Ui::Digitiser *ui;
    QGraphicsScene *scene;

    QVector<QPointF> axis;
    QImage tempI;
    QImage &image = tempI;
    QPixmap tempP;
    QPixmap &pix = tempP;

    QSize *uiSize;
    QSize viewSize;
    QSize imageSize;
    double xPixToGraph;
    double xGraphToPix;
    double yPixToGraph;
    double yGraphToPix;
    double xCalibrationOffset;
    double yCalibrationOffset;
    bool opened = false;
    bool calibrated = false;
    double ratio;
    double xRatioOffset;
    double yRatioOffset;
    double uiX;
    double uiY;

    QVector<QPointF> pixelPoints;
    QVector<QPointF> linLinPoints;
    QVector<QPointF> linLogPoints;
    QVector<QPointF> logLinPoints;
};
#endif // DIGITISER_H
