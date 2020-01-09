#include "digitiser.h"
#include "ui_digitiser.h"

Digitiser::Digitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Digitiser)
{
    ui->setupUi(this);
    scene = new CustomScene(this);
    ui->view->setScene(scene);
    connect(scene,SIGNAL(viewClick(QPointF)),this,SLOT(viewClicked(QPointF)));

    createGroups();
}

Digitiser::~Digitiser()
{
    delete ui;
}

void Digitiser::createGroups(){

    ui->x1Box->setRange(-100000,100000);
    ui->x2Box->setRange(-100000,100000);
    ui->y1Box->setRange(-100000,100000);
    ui->y2Box->setRange(-100000,100000);
    ui->x1Box->setValue(0);
    ui->x2Box->setValue(100);
    ui->y1Box->setValue(0);
    ui->y2Box->setValue(100);
    ui->calibrateProgress->setRange(0,100);
    on_resetCalibration_clicked();
    selectGroup();
}
void Digitiser::selectGroup(Group group){

    ui->calibrateGroup->hide();
    ui->selectPointsGroup->hide();
    ui->saveGroup->hide();

    switch(group){
    case cali:
        ui->calibrateGroup->show();
        calibrating = true;
        break;
    case selectPoints:
        calibrating = false;
        ui->selectPointsGroup->show();
        break;
    case save:
        calibrating = false;
        ui->saveGroup->show();
        break;
    }
}

void Digitiser::on_actionOpen_triggered(){
    //QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    QString fileName = "C:/Users/ASUS/Documents/Qt/Graph1.gif";
    if(QString::compare(fileName, QString()) !=0){
        scene->clear();
        QPixmap pix = QPixmap::fromImage(QImage(fileName));
        scene->addPixmap(pix);
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
    sizeX = this->size().width();
    sizeY = this->size().height();

    if(opened){
        qDebug() << "Resized and Reset";
        on_resetCalibration_clicked();
        QMessageBox::information(this,tr("Digitiser Resized"),tr("Calibration must be completed again after window resized."));
    }
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
    default:
        break;
    };
}
void Digitiser::on_resetCalibration_clicked(){
    ui->calibrateInstruct->setText("Calibration X Axis: \n\nClick twice on the X axis");
    calibrated = false;
    axis.clear();
    ui->calibrateProgress->setValue(0);
    ui->calibrateButton->setEnabled(false);
}
void Digitiser::on_actionCalibrate_triggered(){
    selectGroup(cali);
    if(!calibrating & !calibrated){
        on_resetCalibration_clicked();
    }
}
void Digitiser::viewClicked(QPointF point){
    if(calibrating){
        if(!opened){
            noGraphLoaded();
        }else if((axis.size()<4)){
            axis.append(point);
            switch(axis.size()){
            case 1:
                ui->calibrateProgress->setValue(25);
                break;
            case 2:
                ui->calibrateInstruct->setText("Calibration Y Axis: \n\nClick twice on the Y axis");
                ui->calibrateProgress->setValue(50);
                break;
            case 3:
                ui->calibrateProgress->setValue(75);
                break;
            case 4:
                ui->calibrateProgress->setValue(100);
                ui->calibrateInstruct->setText("Input Graph Values \n\nAdd below the values of each of the respective points clicked.");
                ui->calibrateButton->setEnabled(true);
                break;
            }
        }
    }
    if(calibrated){
        qDebug() << "Graph Point";
        QPointF realPoint( round((point.x()-xOffset) *xPixToGraph*10)/10, round((point.y()-yOffset) *yPixToGraph*10)/10);
        qDebug() << realPoint;
    }else{
        qDebug() << "Pixel Point";
        qDebug() << point;
    }
}
void Digitiser::on_calibrateButton_clicked(){
    calibrated = true;
    calibrate(ui->x1Box->value(), ui->x2Box->value(), axis[0].x(), axis[1].x(), xPixToGraph, xGraphToPix, xOffset);
    calibrate(ui->y1Box->value(), ui->y2Box->value(), axis[2].y(), axis[3].y(), yPixToGraph, yGraphToPix, yOffset);
    selectGroup(selectPoints);
}
void Digitiser::calibrate(double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset){
    double pixChange = graph1-graph2;
    double graphChange = pix1-pix2;
    pixToGraph = (pixChange/graphChange);
    graphToPix = (graphChange/pixChange);
    double offset1 =(pix1-(graph1*graphToPix));
    double offset2 =(pix2-(graph2*graphToPix));
    offset = (offset1+offset2)/2;
}

void Digitiser::on_actionSelect_Points_triggered(){
    selectGroup(selectPoints);
    selectingPoints = true;
}

void Digitiser::on_actionSave_triggered(){
    selectGroup(save);

}
