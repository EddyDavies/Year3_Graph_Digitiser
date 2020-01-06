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

    ui->calibrateGroup->hide();
    ui->x1Box->setRange(-100000,100000);
    ui->x2Box->setRange(-100000,100000);
    ui->y1Box->setRange(-100000,100000);
    ui->y2Box->setRange(-100000,100000);

    setDefaults();
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
    sizeX = this->size().width();
    sizeY = this->size().height();
    on_resetCalibration_clicked();
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
            on_resetCalibration_clicked();
            ui->calibrateGroup->show();
            calibrating = true;
        }else{
            ui->calibrateGroup->hide();
            calibrating = false;
        }
    }
}

void Digitiser::viewClicked(QPointF point){
    if(calibrating & (axis.size()<4)){
        axis.append(point);
        switch(axis.size()){
        case 2:
            ui->instructLabel->setText("Calibration Y Axis: \n\nClick twice on the Y axis");
            break;
        case 4:
            ui->instructLabel->setText("Input Graph Values \n\nAdd below the values of each of the respective points clicked.");
            ui->calibrateButton->setEnabled(true);
            break;
        }
    }
}

void Digitiser::on_resetCalibration_clicked(){
    ui->instructLabel->setText("Calibration X Axis: \n\nClick twice on the X axis");
    axis.clear();
    calibrated = false;
    ui->calibrateButton->setEnabled(false);
}

void Digitiser::on_calibrateButton_clicked(){
    calibrating = false;
    calibrated = true;
    x1=ui->x1Box->value();
    x2=ui->x2Box->value();
    y1=ui->y1Box->value();
    y2=ui->y2Box->value();
    qDebug() << x1;
    qDebug() << x2;
    qDebug() << y1;
    qDebug() << y2;

    double xScale = (abs(x1-x2))/(abs(axis[0].x()-axis[1].x()));
    qDebug() << xScale;
    double yScale = (abs(y1-y2))/(abs(axis[2].y()-axis[3].y()));
    qDebug() << yScale;
    //double xOffset = ui->size().x()-sizeX

    ui->calibrateGroup->hide();
}

void Digitiser::setDefaults(){
    ui->x1Box->setValue(x1);
    ui->x2Box->setValue(x2);
    ui->y1Box->setValue(y1);
    ui->y2Box->setValue(y2);
}

