#include <QObject>
#include "dot.h"

Dot::Dot(Game *parent_ipt) {
    parent = parent_ipt;
    setPixmap(QPixmap(":/pic/item/dot.png").scaledToWidth(16));
}

void Dot::eaten() {
    emit dotEaten();
    delete this;
}

void Dot::shine() {

}
