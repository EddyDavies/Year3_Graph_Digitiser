#include "digitiser.h"
#include "ui_digitiser.h"

Digitiser::Digitiser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Digitiser)
{
    ui->setupUi(this); //point this file to the digitiser.ui file
    scene = new CustomScene(this); //creates an instance of the custom scene class and point it at the QGraphicsScene pointer
    ui->view->setScene(scene); //sets the QGraphicsView in the ui to show the scene
    connect(scene,SIGNAL(viewClick(QPointF)),this,SLOT(viewClicked(QPointF))); // this connects the signal in customescne.cpp and the slot in this file
    //so that when emit viewClick is called in the customscene class, the viewClick slot function is called.
    createGroups();//runs the createGroups class
}

Digitiser::~Digitiser()
{
    delete ui;//closes ui when program closes
}

void Digitiser::createGroups(){
    //set the range for the boxes that define the 4 calibration points
    ui->x1Box->setRange(-100000,100000);
    ui->x2Box->setRange(-100000,100000);
    ui->y1Box->setRange(-100000,100000);
    ui->y2Box->setRange(-100000,100000);
    //set the default values for 4 calibration points
    ui->x1Box->setValue(0);
    ui->x2Box->setValue(100);
    ui->y1Box->setValue(0);
    ui->y2Box->setValue(100);
    //set the range for the calibration progress bar
    ui->calibrateProgress->setRange(0,4);

    //sets range of values that the log base can be
    ui->xLogBaseBox->setRange(0.00001,10000);
    ui->yLogBaseBox->setRange(0.00001,10000);
    ui->xLogBaseBox->setValue(10);
    ui->yLogBaseBox->setValue(10);
    //sets default mode as linear to linear graph
    ui->xLinear->click();
    ui->yLinear->click();
    selectGroup(); //intialy opens the calibrate group which is default
}
void Digitiser::selectGroup(Groups group){ //takes input of type Group, the enumurator created in the digitiser.h
    this->group=group; //sets the global group variable as the value inputtedas the argument for this class
    //hides all the groups
    ui->calibrateGroup->hide();
    ui->selectPointsGroup->hide();
    ui->automatedGroup->hide();

    switch(group){ //switch select using current group so that controls for respective functions can be accessed
    case cali:
        ui->calibrateGroup->show(); //shows calibtrate group
        break;//breaks out of the switch
    case selectPoints:
        ui->selectPointsGroup->show(); //shows select points group
        break;//breaks out of the switch
    case automated:
        ui->automatedGroup->show(); //shows automated calibtrate group
        break;//breaks out of the switch
    }
}

void Digitiser::on_actionOpen_triggered(){ //function run when either the Open toolbar or menu bar, file pull down, buttons are pressed
    //creates QFileDialog with specifed title, that allows user to select the location of a file the has any of the specified file extensions
    QString filePath=QFileDialog::getOpenFileName(this, tr("Open Line Graph"), "", tr("Images (*.BMP *.GIF *.JPG *.JPEG *.PNG *.PBM *.PGM *.PPM *.XBM *.XPM)"));
    if(QString::compare(filePath, QString()) !=0){ //if a file was selected, filePath will not be empty and thus can be shown in the view
        scene->clear(); //clears the QGraphicsScene
        QImage imageItem(filePath); //opens the image located at the file path held in filePath
        pix = QPixmap::fromImage(imageItem); //creates a QPixmap from imported image held in the QImage
        QImage pixImage = pix.toImage(); //creates an image item from the pixmap that has been added to the QGraphicsScene
        scene->addPixmap(pix); //adds the QPix object to the QGraphicsScene
        image = pixImage; //Passes the memory location of the piximage item to the global image pointer to be used later
        ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio); //fits the QGraphicsView to display the QPixmap that has been place on the QGraphicsScene
        opened = true; //sets the opened to true so other classes can know an image has been loaded
        on_resetCalibration_clicked(); //resets all values involved in calibration
//        setSizes();
    }else{
        //creates a message box asking if the user would like to retry selecting a file when one has not been succesfully selected
        int tryAgain = QMessageBox::warning(this,tr("Digitiser Load"),tr("No file selected. Try again?"), //QMessageBox title and main text
        QMessageBox::Yes | QMessageBox::No, //two options in message box
        QMessageBox::Yes); //highlighted or suggested option
        switch(tryAgain){ //switch to deal with output from the message box
        case QMessageBox::Yes://yes was clicked
            on_actionOpen_triggered(); //runs this function again
            break;//breaks out of the switch
        case QMessageBox::No://no was clicked
            break;//breaks out of the switch
        };
    }
}
void Digitiser::on_resetCalibration_clicked(){ //function run when Reset Calibration button is pressed
    //sets the calibrateInstruct label text to inform the user of first step they need to take
    ui->calibrateInstruct->setText("Calibration X Axis: \n\nClick twice on the X axis");
    calibrated = false; //sets
    axis.clear(); //clears QVector of QPointF holding the points of the axis used for calibration
    ui->calibrateProgress->setValue(0);//sets the calibration progress bar back to zero
    ui->calibrateButton->setEnabled(false);//disable the calibration button as there now no axis points selected
}

void Digitiser::viewClicked(QPointF pixelPoint){

    if(group==cali){ //runs if the program is in calibration mode
        if(!opened){ //checks if a graph has been loaded
            noGraphLoaded();
        }else if((axis.size()<4)){
            axis.append(pixelPoint);
            switch(axis.size()){
            case 1:
                ui->calibrateProgress->setValue(1);
                break;//breaks out of the switch
            case 2:
                ui->calibrateInstruct->setText("Calibration Y Axis: \n\nClick twice on the Y axis");
                ui->calibrateProgress->setValue(2);
                break;//breaks out of the switch
            case 3:
                ui->calibrateProgress->setValue(3);
                break;//breaks out of the switch
            case 4:
                ui->calibrateProgress->setValue(4);
                ui->calibrateInstruct->setText("Input Graph Values \n\nAdd below the values of each of the respective points clicked.");
                ui->calibrateButton->setEnabled(true);
                break;//breaks out of the switch
            }
        }
    }
    if(group==selectPoints){
       if(!opened){ //checks if a graph has been loaded
           noGraphLoaded();
       }else if (!(axis.size()==4)){ //checks enough calibration points have been chosen
           notCalibrated();
       }else{//
           pixelPoints.append(pixelPoint);
           convertPoint(pixelPoint);
       }
    }
}

void Digitiser::convertPoint(QPointF pixelPoint){
    if(!calibrated){//runs the calibration function once for each axis with x and y version of same variables
        calibrate(ui->xLog->isChecked(), ui->xLogBaseBox->value(), ui->x1Box->value(),
                  ui->x2Box->value(), axis[0].x(), axis[1].x(), xPixToGraph, xGraphToPix, xCalibrationOffset);
        calibrate(ui->yLog->isChecked(), ui->xLogBaseBox->value(), ui->y1Box->value(),
                  ui->y2Box->value(), axis[2].y(), axis[3].y(), yPixToGraph, yGraphToPix, yCalibrationOffset);
        calibrated=true;
    }
    QPointF point;//creates a point and fills it with the pixel coordinate converted into the graph space coordinate
    double x = (pixelPoint.x()-xCalibrationOffset) *xPixToGraph;
    double y = (pixelPoint.y()-yCalibrationOffset) *yPixToGraph;
    if(!ui->xLog->isChecked() & !ui->yLog->isChecked()){ //runs in linear linear mode
        point.setX(x);
        point.setY(y);
        linLinPoints.append(point); // adds the point to the QVector of QPointFs for linear linear mode
    }else if(!ui->xLog->isChecked() & ui->yLog->isChecked()){
        point.setX(x);
        point.setY(exp(y));//converts the value back in real value rather than log value
        linLogPoints.append(point);// adds the point to the QVector of QPointFs for linear log mode
    }else if(ui->xLog->isChecked() & !ui->yLog->isChecked()){
        point.setX(exp(x));//converts the value back in real value rather than log value
        point.setY(y);
        logLinPoints.append(point);// adds the point to the QVector of QPointFs for log linear mode
    }
    ui->xLabel->setNum(round(point.x()*100)/100 ); //shows the value in ui label to 2 decimal places
    ui->yLabel->setNum(round(point.y()*100)/100 );
}

void Digitiser::calibrate(bool log, double logBase, double graph1, double graph2, double pix1, double pix2, double &pixToGraph, double &graphToPix, double &offset){
    if(log){//if the funcition is run in log mode, calculates the log of the two calibration point values
        graph1=baseLog(logBase, graph1);
        graph2=baseLog(logBase, graph2);
    }
    double graphChange = graph1-graph2;  //calculates difference between two calibration points chosen
    double pixChange = pix1-pix2; //calculates difference between coordinate value of two calibration points chosen
    pixToGraph = (graphChange/pixChange); //calculates conversion ratio from pixel coordiant to Graph coordiante
    graphToPix = (pixChange/graphChange);//calculates conversion ratio from pixel coordiant to Graph coordiante
    double offset1 =(pix1-(graph1*graphToPix)); //calculates difference between first point on axis and firs point in QGraphicsView
    double offset2 =(pix2-(graph2*graphToPix)); //calculates difference between second point on axis and second point in QGraphicsView
    offset = (offset1+offset2)/2; //averages these two for the most accurate offset figure
}
double Digitiser::baseLog(double base, double x) {
    return (log(x) / log(base));//calculates log of provided value at provided base
}

void Digitiser::on_calibrateButton_clicked(){
    selectGroup(selectPoints);//switch to the select points group showing controls for selecting points
}

void Digitiser::on_actionCalibrate_triggered(){ //function run when either the Calibrate toolbar or menu bar, edit pull down, buttons are pressed
    if(!(group==cali) & !(axis.size()==4)){ //checks if calibration is not currently happening and has not been completed
        on_resetCalibration_clicked(); //run to reset value involved calibration if calibration was not completed when group switched away from and back to
    }
    selectGroup(cali); //shows the Calibrate group and hides the rest
}

void Digitiser::on_actionSelect_Points_triggered(){ //function run when either the Select Points toolbar or menu bar, edit pull down, buttons are pressed
    selectGroup(selectPoints); //shows select point controls
}
void Digitiser::on_actionSave_triggered(){ //function run when either the Save toolbar or menu bar, file pull down, buttons are pressed
    if(!pixelPoints.empty()){//checks points have been selected and stored in the pixel points QVector of QPointFs
        if(!ui->xLog->isChecked() & !ui->yLog->isChecked()){//runs when the program is in x linear and y linar mode
            write(linLinPoints);//runs write function with input of the linLinPoints QVector of QPointFs
        }else if(!ui->xLog->isChecked() & ui->yLog->isChecked()){//runs when the program is in x linear and y log mode
            write(linLogPoints);//runs write function with input of the linLogPoints QVector of QPointFs
        }else if(ui->xLog->isChecked() & !ui->yLog->isChecked()){//runs when the program is in x log and y linar mode
            write(logLinPoints);//runs write function with input of the logLinPoints QVector of QPointFs
        }
    }
}
void Digitiser::write(QVector<QPointF> &points){ //takes input of the memory address for specific QVector of QPointFs for mode the program is on
    //creates a file dialog box that asks for a file path to save a .csv file and stores this in filePath string
    QString filePath=QFileDialog::getSaveFileName(this, tr("Save Data"), "", tr("CSV File (*.csv)"));
    QFile file(filePath); //creates a file stream with the filePath as the location
    if(file.open(QFile::WriteOnly)){ //checks the file path is not in use already by another file
        for(auto p : pixelPoints){ //runs a for loop once for each of the raw pixel coordinates selected points stored in QPointFs which are stored in the QVector of QPointFs pixelPoints
            convertPoint(p); //Runs the point conversion function for each point
        }
        QTextStream stream(&file);//creates a text stream with memory address of the selected file path
        for(auto p : points){ //runs a for loop once for each QPointF in points
            stream << p.x() << "," << p.y() << "\n"; //writes the x value a ',' then y value for each point and then starts a new line
        }
        file.close(); //closes the file stream
    }else{//ran if the file name is in use
        int saveNow = QMessageBox::information(this,tr("Digitiser Load"),tr("There was a file with that name in that folder. \n Would you like to select another name or folder now?"),
                                               QMessageBox::Yes | QMessageBox::No, //two options in message box
                                               QMessageBox::Yes); //highlighted or suggested option
        switch(saveNow){//ask for file path to save again if the user clicks Yes.
        case QMessageBox::Yes://yes was clicked
            on_actionSave_triggered(); //runs this function again
            break;//breaks out of the switch
        case QMessageBox::No://no was clicked
            break;//breaks out of the switch
        default:
            break;//breaks out of the switch
        };
    }

}

void Digitiser::resizeEvent(QResizeEvent *){ //run everytime the window is resized and when the window first opens
    ui->view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);//refits the QGraphicsView to have the QPixMap for the graph fit the screen

    //if an image is opend and the graph calibrated, calibration is all cleared and a message box to remind user that these need readding created
    if(opened & (!axis.empty() | calibrated)){
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


void Digitiser::noGraphLoaded(){//asks user if they wish to load a graph when they perform an action that expects the program to already have a graph loaded
    int openNow = QMessageBox::information(this,tr("Digitiser Load"),tr("A graph must be loaded first. \nWould you like to do that now?"),
                                           QMessageBox::Yes | QMessageBox::No, //two options in message box
                                           QMessageBox::Yes); //highlighted or suggested option
    switch(openNow){
    case QMessageBox::Yes://yes was clicked
        on_actionOpen_triggered();
        break;//breaks out of the switch
    case QMessageBox::No://no was clicked
        break;//breaks out of the switch
    default:
        break;//breaks out of the switch
    };
}
void Digitiser::notCalibrated(){//asks user if they wish to calibrate when they perform an action that expects the program to already be calibrated
    int calibrateNow = QMessageBox::warning(this,tr("Digitiser Calibration"),tr("The Graph must be calibrated first. \nWould you like to to do that now?"),
                                           QMessageBox::Yes | QMessageBox::No, //two options in message box
                                           QMessageBox::Yes); //highlighted or suggested option
    switch(calibrateNow){
    case QMessageBox::Yes://yes was clicked
        on_actionCalibrate_triggered();
        break;//breaks out of the switch
    case QMessageBox::No://no was clicked
        break;//breaks out of the switch
    default:
        break;//breaks out of the switch
    };
}
void Digitiser::on_clearPoints_clicked()//ran when Clear Points button is clicked
{//clear all the QVector of QPointF that hold the points slected on the graph
    pixelPoints.clear();
    linLinPoints.clear();
    linLogPoints.clear();
    logLinPoints.clear();
}

void Digitiser::on_xLinear_toggled(bool checked)//ran when the x axis linear button is toggled
{
    if(checked){//selects for when the radio button is being toggled on
        calibrated = false;//resets calibrated variable to false so the convertPoint() function knows to recalculate pixel to graph space coordinates conversion
        ui->xLogBaseLabel->hide();//hides both the logBase Label and Box that are not required in this mode
        ui->xLogBaseBox->hide();
    }
}
void Digitiser::on_xLog_toggled(bool checked)//ran when the x axis log button toggled
{
    if(checked){//selects for when the radio button is being toggled on
        calibrated = false;//resets calibrated variable to false so the convertPoint() function knows to recalculate pixel to graph space coordinates conversion
        if(ui->yLog->isChecked()){//switches the y axis mode as log log is not a valid option
            ui->yLinear->toggle();//presses the y linear radio button if y is in log mode
        }
        ui->xLogBaseLabel->show();//shows both the logBase Label and Box that are required in this mode
        ui->xLogBaseBox->show();
    }
}
void Digitiser::on_yLinear_toggled(bool checked)//ran when the y axis linear button toggled
{
    if(checked){//selects for when the radio button is being toggled on
        calibrated = false;//resets calibrated variable to false so the convertPoint() function knows to recalculate pixel to graph space coordinates conversion
        ui->yLogBaseLabel->hide();//hides both the logBase Label and Box that are not required in this mode
        ui->yLogBaseBox->hide();
    }
}
void Digitiser::on_yLog_toggled(bool checked)//ran when the y axis log button toggled
{
    if(checked){//selects for when the radio button is being toggled on
        calibrated = false;//resets calibrated variable to false so the convertPoint() function knows to recalculate pixel to graph space coordinates conversion
        if(ui->xLog->isChecked()){//checks the x axis mode as log log is not a valid option
            ui->xLinear->toggle();//presses the x linear radio button if x is in log mode
        }
        ui->yLogBaseLabel->show();//shows both the logBase Label and Box that are required in this mode
        ui->yLogBaseBox->show();
    }
}





void Digitiser::on_actionAutomatic_triggered()
{
    selectGroup(automated);
//    double x = image.width()+226;
//    double y = image.height()+95;
//    QSize size(x,y);
//    uiSize = &size;
//    this->showMaximized();

}



//void Digitiser::mousePressEvent(QMouseEvent *event){
//    QPoint viewPoint(event->x()-10,event->y()-65);
//    pointColour(viewPoint);
//}
//void Digitiser::pointColour(QPoint point){
//    qDebug() << "point " << point;
//    qDebug() << "view size " << ui->view->size();
//    qDebug() << "image Size "<< image.size();
//    double x = (point.x()*ratio)-xRatioOffset;
//    qDebug() << "xRatioOffset" << xRatioOffset;
//    double y = (point.y()*ratio)-yRatioOffset;
//    qDebug() << "yRatioOffset" << yRatioOffset;
//    QPointF imagePoint(x,y);
//    qDebug() << "imagePoint" << imagePoint;

//    QPoint point( round(floatPoint.x()), round(floatPoint.y()) );
//    QColor colourAt = image->pixelColor(point);
//    qDebug() << "Colour " << colourAt;

//}
//void Digitiser::setSizes(){
//    double viewX = ui->view->width();
//    double imageX = image.width();
//    double viewY = ui->view->height();
//    double imageY = image.height();
//    double diff = imageY * ( viewX /imageX );

//    if(diff > ui->view->size().height()){
//        qDebug() << "bound by y";
//        ratio = (imageY/viewY);
//        xRatioOffset = abs(( ((viewX*(1/ratio))-imageX) /2));
//        yRatioOffset = 0;
//    }else{
//        qDebug() << "bound by x";
//        ratio = (imageX/viewX);
//        xRatioOffset = 0;
//        yRatioOffset = abs(( ((viewY*(1/ratio))-imageY) /2));
//    }
//}


