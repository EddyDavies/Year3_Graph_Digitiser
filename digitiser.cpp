#include "digitiser.h"
#include "ui_digitiser.h"

Digitiser::Digitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Digitiser)
{
    ui->setupUi(this);
    setCentralWidget(ui->view);

    scene = new QGraphicsScene(this);
    ui->view->setScene(scene);
}

Digitiser::~Digitiser()
{
    delete ui;
}

void Digitiser::on_actionOpen_triggered(){
    //QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    QString fileName = "C:/Users/ASUS/Documents/Qt/Graph.png";
    if(QString::compare(fileName, QString()) !=0){
        scene->clear();
        pix = QPixmap::fromImage(QImage(fileName));
        pixItem = scene->addPixmap(pix);
        ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        opened = true;
    }else{
        int tryAgain = QMessageBox::warning(this,tr("Digitiser Load"),tr("No file selected. Try again?"),
        QMessageBox::Yes | QMessageBox::No, //two options in message box
        QMessageBox::Yes); //highlighted or suggested option
        switch(tryAgain){
        case QMessageBox::Yes://yes was clicked
            on_actionOpen_triggered();
            break;
        case QMessageBox::No://no was clicked
            break;
        };
    }
}
void Digitiser::resizeEvent(QResizeEvent *){
    ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
    }else{
        QMessageBox::warning(this,tr("Error"),tr("No file selected. Please try again."));
    }
}
