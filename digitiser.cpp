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
    ui->calibrateProgress->setRange(0,4);
    on_resetCalibration_clicked();


    ui->xLogBaseBox->setRange(0,100000);
    ui->yLogBaseBox->setRange(0,100);
    ui->xLogBaseBox->setValue(10);
    ui->yLogBaseBox->setValue(10);
    ui->xLinear->click();
    ui->yLinear->click();
    selectGroup();
}
void Digitiser::selectGroup(Groups group){
    this->group=group;

    ui->calibrateGroup->hide();
    ui->selectPointsGroup->hide();
    ui->saveGroup->hide();

    switch(group){
    case cali:
        ui->calibrateGroup->show();
        break;
    case selectPoints:
        ui->selectPointsGroup->show();
        break;
    case save:
        ui->saveGroup->show();
        break;
    }
}

void Digitiser::on_actionOpen_triggered(){
    QString fileName=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    if(QString::compare(fileName, QString()) !=0){
        scene->clear();
        QPixmap pix = QPixmap::fromImage(QImage(fileName));
        scene->addPixmap(pix);
        ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
        opened = true;
        on_resetCalibration_clicked();
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
void Digitiser::on_resetCalibration_clicked(){
    ui->calibrateInstruct->setText("Calibration X Axis: \n\nClick twice on the X axis");
    calibrated = false;
    axis.clear();
    ui->calibrateProgress->setValue(0);
    ui->calibrateButton->setEnabled(false);
}
void Digitiser::on_calibrateButton_clicked(){
    selectGroup(selectPoints);
}
void Digitiser::on_actionCalibrate_triggered(){
    if(!(group==cali) & !(axis.size()==4)){
        on_resetCalibration_clicked();
    }
    selectGroup(cali);
}
void Digitiser::resizeEvent(QResizeEvent *){
    ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    sizeX = this->size().width();
    sizeY = this->size().height();

    if(opened & (!axis.empty() | calibrated)){
        qDebug() << "Resized and Reset";
        on_resetCalibration_clicked();
        on_clearPoints_clicked();
        selectGroup(cali);// move maybe
        if(pixelPoints.empty()){
            QMessageBox::information(this,tr("Digitiser Resized"),tr("Calibration reset, please recalibrate after window is resized."));
        }else{
            QMessageBox::information(this,tr("Digitiser Resized"),tr("Calibration reset and points cleared, please recalibrate and reselect points after window is resized."));
        }
    }
}

void Digitiser::calibrate(bool log, double logBase, double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset){
    if(log){
        qDebug() << "Graph 1 " << graph1;
        graph1=baseLog(logBase, graph1);
        qDebug() << "Graph 1 " << graph1;
        qDebug() << "Graph 2 " << graph2;
        graph2=baseLog(logBase, graph2);
        qDebug() << "Graph 2 " << graph2;
    }
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
}
void Digitiser::on_actionSave_triggered(){
    if(!pixelPoints.empty()){
        if(!xAxisTypeLog & !yAxisTypeLog){
            write(linLinPoints);
        }else if(!xAxisTypeLog & yAxisTypeLog){
            write(linLogPoints);
        }else if(xAxisTypeLog & !yAxisTypeLog){
            write(logLinPoints);
        }
    }
}


void Digitiser::write(QVector<QPointF> &points){
    QString fileName=QFileDialog::getSaveFileName(this, tr("Save Data"), "", tr("CSV File (*.csv)"));
    QFile file(fileName);
    if(file.open(QFile::WriteOnly)){
        if( !(pixelPoints.size()==points.size())){
            for(auto p : pixelPoints){
                convertPoint(p);
            }
        }
        QTextStream stream(&file);
        for(auto p : points){
            stream << p.x() << "," << p.y() << "\n";
        }
        file.close();
    }

}
void Digitiser::viewClicked(QPointF pixelPoint){
    if(group==cali){
        if(!opened){
            noGraphLoaded();
        }else if((axis.size()<4)){
            axis.append(pixelPoint);
            switch(axis.size()){
            case 1:
                ui->calibrateProgress->setValue(1);
                break;
            case 2:
                ui->calibrateInstruct->setText("Calibration Y Axis: \n\nClick twice on the Y axis");
                ui->calibrateProgress->setValue(2);
                break;
            case 3:
                ui->calibrateProgress->setValue(3);
                break;
            case 4:
                ui->calibrateProgress->setValue(4);
                ui->calibrateInstruct->setText("Input Graph Values \n\nAdd below the values of each of the respective points clicked.");
                ui->calibrateButton->setEnabled(true);
                break;
            }
        }
    }
    if(group==selectPoints){
       if(!opened){
           noGraphLoaded();
       }else if (!(axis.size()==4)){
           notCalibrated();
       }else{
           pixelPoints.append(pixelPoint);
           convertPoint(pixelPoint);
       }
    }
}
double Digitiser::baseLog(double base, double x) {
    return (log(x) / log(base));
}
void Digitiser::convertPoint(QPointF pixelPoint){
    if(!calibrated){
        calibrate(xAxisTypeLog, ui->xLogBaseBox->value(), ui->x1Box->value(), ui->x2Box->value(), axis[0].x(), axis[1].x(), xPixToGraph, xGraphToPix, xOffset);
        calibrate(yAxisTypeLog, ui->xLogBaseBox->value(), ui->y1Box->value(), ui->y2Box->value(), axis[2].y(), axis[3].y(), yPixToGraph, yGraphToPix, yOffset);
        calibrated=true;
    }
    QPointF point;
    double x = (pixelPoint.x()-xOffset) *xPixToGraph;
    double y = (pixelPoint.y()-yOffset) *yPixToGraph;
    if(!xAxisTypeLog & !yAxisTypeLog){
        point.setX(x);
        point.setY(y);
//        point.setX(round(x*100)/100);
//        point.setY(round(y*100)/100);
        linLinPoints.append(point);
    }else if(!xAxisTypeLog & yAxisTypeLog){
        point.setX(x);
        point.setY(exp(y));
//        point.setX(round(x*100)/100);
//        point.setY(exp( y*100)/100 ));
        ui->xLabel->setNum(point.x());
        ui->yLabel->setNum(point.y());
        linLogPoints.append(point);
    }else if(xAxisTypeLog & !yAxisTypeLog){
        point.setX(exp(x));
        point.setY(y);
//        point.setX(exp( round(x*100)/100 ));
//        point.setY(round(y*100)/100 );
        ui->xLabel->setNum(point.x());
        ui->yLabel->setNum(point.y());
        logLinPoints.append(point);
    }
}
void Digitiser::noGraphLoaded(){
    int openNow = QMessageBox::information(this,tr("Digitiser Load"),tr("A graph must be loaded first. \nWould you like to do that now?"),
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
void Digitiser::notCalibrated(){
    int calibrateNow = QMessageBox::warning(this,tr("Digitiser Calibration"),tr("The Graph must be calibrated first. \nWould you like to to do that now?"),
                                           QMessageBox::Yes | QMessageBox::No, //two options in message box
                                           QMessageBox::Yes); //highlighted or suggested option
    switch(calibrateNow){
    case QMessageBox::Yes://yes was clicked
        on_actionCalibrate_triggered();
        break;
    case QMessageBox::No://no was clicked
        break;
    default:
        break;
    };
}

void Digitiser::on_clearPoints_clicked()
{
    pixelPoints.clear();
    linLinPoints.clear();
    linLogPoints.clear();
    logLinPoints.clear();
}
void Digitiser::on_xLinear_toggled(bool checked)
{
    if(checked){
        calibrated = false;
        ui->xLogBaseLabel->hide();
        ui->xLogBaseBox->hide();
        xAxisTypeLog=false;
    }
}
void Digitiser::on_xLog_toggled(bool checked)
{
    if(checked){
        calibrated = false;
        if(yAxisTypeLog){
            ui->yLinear->toggle();
            yAxisTypeLog = false;
        }
        ui->xLogBaseLabel->show();
        ui->xLogBaseBox->show();
        xAxisTypeLog=true;
    }
}
void Digitiser::on_yLinear_toggled(bool checked)
{
    if(checked){
        calibrated = false;
        ui->yLogBaseLabel->hide();
        ui->yLogBaseBox->hide();
        yAxisTypeLog=false;
    }
}
void Digitiser::on_yLog_toggled(bool checked)
{
    if(checked){
        calibrated = false;
        if(xAxisTypeLog){
            ui->xLinear->toggle();
            xAxisTypeLog = false;
        }
        ui->yLogBaseLabel->show();
        ui->yLogBaseBox->show();
        yAxisTypeLog=true;
    }
}


