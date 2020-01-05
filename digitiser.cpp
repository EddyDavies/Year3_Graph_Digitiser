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
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    //QString fileName = "C:/Users/ASUS/Documents/Qt/Graph.png";
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
void Digitiser::noGraphLoaded(){
    int openNow = QMessageBox::information(this,tr("Digitiser Calibration"),tr("A graph must be loaded first, would you like to do that now?"),
                                           QMessageBox::Yes | QMessageBox::No, //two options in message box
                                           QMessageBox::Yes); //highlighted or suggested option
    switch(openNow){
    case QMessageBox::Yes://yes was clicked
        on_actionOpen_triggered();
        break;
    case QMessageBox::No://no was clicked
        break;
    };
}

void Digitiser::mousePressEvent(QMouseEvent *mouseevent){
    if(calibrating){
        if(xAxis.size()<2){
            qDebug()<< "mouseEvent X";
            xAxis.push_back(mouseevent->pos());
            storeValues(xAxis, x1, x2);
        }else if(yAxis.size()<2){
            qDebug()<< "mouseEvent y";
            yAxis.push_back(mouseevent->pos());
            storeValues(yAxis, y1, y2);
        }
    }
}
void Digitiser::storeValues(QVector<QPointF> &axis, double &first, double &second){
    bool ok;
    switch(axis.size()){
    case 1:
        first = QInputDialog::getDouble(this, tr("Digitiser Calibration 1st Value"),
                                        tr("What value was clicked:"), first, -10000, 10000, 2, &ok);
        if (!ok) cancelCalibration();
        break;
    case 2:
        second = QInputDialog::getDouble(this, tr("Digitiser Calibration 2nd Value"),
                                         tr("What value was clicked:"), second, -10000, 10000, 2, &ok);
        if (!ok) cancelCalibration();
        calibrate();
        break;
    }
}

void Digitiser::cancelCalibration(){
    clearCalibration();
    calibrating = false;
}
void Digitiser::clearCalibration(){
    xAxis.clear();
    yAxis.clear();
    calibrated = false;
}

void Digitiser::on_actionCalibrate_triggered(){
    if(!opened) {
        noGraphLoaded();
        on_actionCalibrate_triggered();
    }else{
        if(calibrating || calibrated){
            clearCalibration();
        }
        calibrating = true;
        calibrate();
    }
}
void Digitiser::calibrate(){
    if (xAxis.size()<2){
        QMessageBox::information(this,tr("Digitiser Calibration X Axis"),tr("Click twice on the X axis"));
    }else if (yAxis.size()<2){
        QMessageBox::information(this,tr("Digitiser Calibration Y Axis"),tr("Click twice on the Y axis"));
    }else if ((yAxis.size()==2) & (xAxis.size()==2)){
        convertValues();
    }
}


void Digitiser::convertValues(){
    calibrating = false;
    calibrated = true;
}



