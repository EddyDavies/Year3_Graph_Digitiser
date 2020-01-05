#ifndef DIGITISER_H
#define DIGITISER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
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
    void resizeEvent(QResizeEvent *);

private slots:
    void on_actionOpen_triggered();
    void on_actionCalibrate_triggered();

private:
    void cancelCalibration();
    bool opened = false; 
    bool calibrating = false;
    bool calibrated = false;
    QVector<QPointF> xAxis;
    QVector<QPointF> yAxis;
    Ui::Digitiser *ui;
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsPixmapItem *pixItem;
};
#endif // DIGITISER_H
