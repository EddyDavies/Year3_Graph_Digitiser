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

private slots:
    void on_actionOpen_triggered();

private:
    bool opened = false; 
    Ui::Digitiser *ui;
    QGraphicsScene *scene;
    QPixmap pix;
    QGraphicsPixmapItem *pixItem;
};
#endif // DIGITISER_H
