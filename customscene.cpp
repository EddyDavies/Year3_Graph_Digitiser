#include "customscene.h"
#include <QDebug>

CustomScene::CustomScene(QObject *parent) :
    QGraphicsScene(parent)
{

}
void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent){
    emit viewClick(mouseEvent->scenePos());
}

