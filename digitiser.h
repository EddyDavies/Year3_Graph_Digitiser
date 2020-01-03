#ifndef DIGITISER_H
#define DIGITISER_H

#include <QMainWindow>
#include <QLabel>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

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
    Ui::Digitiser *ui;
};
#endif // DIGITISER_H
