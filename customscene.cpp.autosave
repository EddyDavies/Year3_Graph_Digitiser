#include "customscene.h" //includes this class' header file

CustomScene::CustomScene(QObject *parent) : //creates a custom QGraphicsScene class that inherits all the QGraphicScene functionality
    QGraphicsScene(parent) //runs the constructor for the QGraphicsScene class with this class' constructors argument as the input
{

}
void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent){ //runs when a mouse event occurs inside the custome scene
    emit viewClick(mouseEvent->scenePos()); //emits a signal to that calls the respective slot function in digitiser.cpp
}   //the signal has a QPointF variable, that contain the posistion of the mouse event, as an argument

