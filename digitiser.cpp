#include "digitiser.h"
#include "ui_digitiser.h"

Digitiser::Digitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Digitiser)
{
    ui->setupUi(this);
}

Digitiser::~Digitiser()
{
    delete ui;
}


void Digitiser::on_actionOpen_triggered()
{
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.png *.jpg *.jpeg *.bmp)"));

    if(QString::compare(fileName, QString()) !=0){
        QPixmap pix(fileName);
        ui->label_image->setPixmap(pix);
    }else{
        QMessageBox::critical(this,tr("Error"),tr("No file selected. Please try again."));
    }
}
