#ifndef DIGITISER_H
#define DIGITISER_H

#include <QDebug>
#include <QtCore>
#include <QtGui>
#include <QMainWindow>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>


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
    void mousePressEvent(QMouseEvent *mouseevent);
    void resizeEvent(QResizeEvent *);

private slots:
    void on_actionOpen_triggered();
    void on_actionCalibrate_triggered();

private:
    void noGraphLoaded();
    void storeValues(QVector<QPointF> &axis, double &first, double &second);
    void convertValues();
    void calibrate();
    void cancelCalibration();
    void clearCalibration();

    bool calibratedAtSize = false;
    Ui::Digitiser *ui;
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsPixmapItem *pixItem;

    double x1=0;
    double x2=100;
    double y1=0;
    double y2=100;
    bool calibrating = false;
    bool calibrated = false;
    QVector<QPointF> xAxis;
    QVector<QPointF> yAxis;
    bool opened = false; 
};
#endif // DIGITISER_H
