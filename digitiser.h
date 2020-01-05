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
    void calibrate(QVector<QPointF> axis, double first, double second);
    void cancelCalibration();

    bool opened = false; 
    bool calibrating = false;
    bool calibrated = false;
    double x1=0;
    QVector<QPointF> xAxis;
    QVector<QPointF> yAxis;
    double x2=100;
    double y1=0;
    double y2=100;

    bool ok;
    bool calibratedAtSize = false;
    Ui::Digitiser *ui;
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsPixmapItem *pixItem;
};
#endif // DIGITISER_H
