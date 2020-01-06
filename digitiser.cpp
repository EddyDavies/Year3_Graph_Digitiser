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

void Digitiser::selectGroup(int){
    Color r = red;
    switch(r)
    {
        case red  : std::cout << "red\n";   break;
        case green: std::cout << "green\n"; break;
        case blue : std::cout << "blue\n";  break;
    }
}

void Digitiser::on_actionOpen_triggered(){
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    //QString fileName = "C:/Users/ASUS/Documents/Qt/Graph.jfif";
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
//add reminder to recalibrate
void Digitiser::resizeEvent(QResizeEvent *){
    ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    sizeX = this->size().width();
    sizeY = this->size().height();
    if(calibrated)
        on_actionCalibrate_triggered();
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
            //ui->calibrateGroup->show();
            calibrating = true;
        }else{
            // ui->calibrateGroup->hide();
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
    qDebug() << "Pixel Point";
    qDebug() << point;
    if(calibrated){
        QPointF realPoint( (point.x()-xOffset) *xPixToGraph, (point.y()-yOffset) *yPixToGraph);
        qDebug() << "Graph Point";
        qDebug() << realPoint;
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

    calibrate(ui->x1Box->value(), ui->x2Box->value(), axis[0].x(), axis[1].x(), xPixToGraph, xGraphToPix, xOffset);
    calibrate(ui->y1Box->value(), ui->y2Box->value(), axis[2].y(), axis[3].y(), yPixToGraph, yGraphToPix, yOffset);

    ui->calibrateGroup->hide();
}

void Digitiser::calibrate(double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset){
    double pixChange = graph1-graph2;
    double graphChange = pix1-pix2;
    pixToGraph = (pixChange/graphChange);
    graphToPix = (graphChange/pixChange);
    qDebug() << "x1Graph " << graph1 << "  x2Graph " << graph2;
    qDebug() << "x1Pixel " << pix1 << "  x2Pixel " << pix2;
    qDebug() << "xPixChange " << pixChange << "  xGraphChange " << graphChange;
    qDebug() << "xPixToGraph " << pixToGraph << "  xGraphToPix " << graphToPix;
    double offset1 =(pix1-(graph1*graphToPix));
    double offset2 =(pix2-(graph2*graphToPix));
    offset = (offset1+offset2)/2;
    qDebug() << "offset1 " << offset1 << "  offset1 " << offset2;
    qDebug() << "offset " << offset;

}

void Digitiser::setDefaults(){
    ui->x1Box->setValue(x1);
    ui->x2Box->setValue(x2);
    ui->y1Box->setValue(y1);
    ui->y2Box->setValue(y2);
}


void Digitiser::on_sideBar_currentChanged(int index)
{

}
