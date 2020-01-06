#ifndef CUSTOMSCENE_H
#define CUSTOMSCENE_H
#include "digitiser.h"

#include <QGraphicsScene>
#include <QGraphicsSceneEvent>

class CustomScene : public QGraphicsScene
{
    Q_OBJECT

public:
    QVector<QPointF> *axis;
    CustomScene(QObject *parent = 0);

signals:
    void viewClick(QPointF point);


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // CUSTOMSCENE_H
