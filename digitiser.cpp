#include "digitiser.h"
#include "ui_digitiser.h"

Digitiser::Digitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Digitiser)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->view->setScene(scene);
    ui->calibrateGroup->hide();
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
bool Digitiser::noGraphLoaded(){
    int openNow = QMessageBox::information(this,tr("Digitiser Calibration"),tr("A graph must be loaded first, would you like to do that now?"),
                                           QMessageBox::Yes | QMessageBox::No, //two options in message box
                                           QMessageBox::Yes); //highlighted or suggested option
    switch(openNow){
    case QMessageBox::Yes://yes was clicked
        on_actionOpen_triggered();
        return true;
    case QMessageBox::No://no was clicked
        return false;
    default:
        return false;
    };
}

void Digitiser::on_actionCalibrate_triggered(){
    if(!opened) {
        if(noGraphLoaded())
            on_actionCalibrate_triggered();
    }else{
        if(!calibrating){
            ui->calibrateGroup->show();
            calibrating = true;
            on_resetCalibration_clicked();

        }else{
            ui->calibrateGroup->hide();
            calibrating = false;
        }
    }
}

void Digitiser::mousePressEvent(QMouseEvent *mouseevent){
    if(calibrating & (axis.size()<4)){
        axis.push_back(mouseevent->pos());
        qDebug() << "axis Pushed";
        switch(axis.size()){
        case 2:
            qDebug() << "case 2";
            ui->instructLabel->setText("Calibration Y Axis: \n\nClick twice on the Y axis");
            break;
        case 4:
            qDebug() << "case 4";
            ui->instructLabel->setText("Input Graph Values \n\nAdd below the values of each of the respective points clicked. fhsudghsgofoirgsogfgsdfg afgshgsfhglskdfg glhfgks dfnglkdfg lfhgsdlfg f gf");
            break;
        }
    }
}

void Digitiser::on_resetCalibration_clicked(){
    ui->instructLabel->setText("Calibration X Axis: \n\nClick twice on the X axis");
    axis.clear();
    calibrated = false;
}

void Digitiser::on_calibrateButton_clicked(){
    calibrating = false;
    calibrated = true;

    ui->calibrateGroup->hide();
}

