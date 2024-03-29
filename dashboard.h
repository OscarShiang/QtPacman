#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsTextItem>

class Dashboard: public QObject, public QGraphicsItemGroup {
    Q_OBJECT
public:
    Dashboard(QObject *parent = 0);
    void addScore(int ipt);
    void reset();

    int getScore();
    int getHighScore();

private:
    int score, high;
    QGraphicsTextItem *text_score;
    QGraphicsTextItem *text_high;

    static const int font_size = 20;
    const QString font_family = "Joystix";
};

#endif // DASHBOARD_H
