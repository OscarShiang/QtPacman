#include "game.h"
#include "pacman.h"
#include "dot.h"
#include "pellet.h"
#include <QDebug>

Game::Game() {
    // set up the widow
    setFixedSize(width, height);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // create the scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, width, height);
    scene->setBackgroundBrush(QPixmap(":/pic/map/map.png").scaledToHeight(564));
    setScene(scene);

    // create the compass item
    compass = new Compass(this);
    connect(compass, SIGNAL(eat(QPoint)), this, SLOT(itemEat(QPoint)));

    mode = Mode::Menu;

    // create the shining timer for power pellets shine
    shine = new QTimer(this);

    // create the pacman (the player)
    player = new Pacman(compass);
    scene->addItem(player);
    player->hide();

    // create the ghost
    blinky = new Blinky(compass);
    scene->addItem(blinky);
    blinky->setPos(32, 51);

    pinky = new Pinky(compass);
    scene->addItem(pinky);
    pinky->setPos(416, 51);

    inky = new Inky(compass);
    scene->addItem(inky);
    inky->setPos(16, 499);

    clyde = new Clyde(compass);
    scene->addItem(clyde);
    clyde->setPos(416, 499);

    // create the move timer
    pacmanMove = new QTimer(this);
    // player move
    connect(pacmanMove, SIGNAL(timeout()), player, SLOT(move()));

    ghostMove = new QTimer(this);

    // ghost move
    connect(ghostMove, SIGNAL(timeout()), blinky, SLOT(move()));
    connect(ghostMove, SIGNAL(timeout()), pinky, SLOT(move()));
    connect(ghostMove, SIGNAL(timeout()), inky, SLOT(move()));
    connect(ghostMove, SIGNAL(timeout()), clyde, SLOT(move()));

    connect(blinky, SIGNAL(fail()), this, SLOT(gameFail()));
    connect(pinky, SIGNAL(fail()), this, SLOT(gameFail()));
    connect(inky, SIGNAL(fail()), this, SLOT(gameFail()));
    connect(clyde, SIGNAL(fail()), this, SLOT(gameFail()));

    // create the dashboard
    board = new Dashboard(this);
    scene->addItem(board);
    board->hide();

    title = new QGraphicsPixmapItem(QPixmap(":/pic/item/title.png").scaledToWidth(350));
    title->setPos(width / 2 - title->boundingRect().width() / 2, 100);
    scene->addItem(title);

    // initialize the buttons
    play = new Button("play", 35);
    quit = new Button("quit", 35);
    scene->addItem(play);
    scene->addItem(quit);
    play->setPos(width / 2 - play->width() / 2, 280);
    quit->setPos(width / 2 - quit->width() / 2, 340);

    connect(play, SIGNAL(clicked()), this, SLOT(gameStart()));
    connect(quit, SIGNAL(clicked()), this, SLOT(close()));

    show();
}

void Game::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Up)
        player->setDirection(Dir::Up);
    else if (event->key() == Qt::Key_Down)
        player->setDirection(Dir::Down);
    else if (event->key() == Qt::Key_Left)
        player->setDirection(Dir::Left);
    else if (event->key() == Qt::Key_Right)
        player->setDirection(Dir::Right);
    else if (event->key() == Qt::Key_Space) {
        qDebug() << player->pos();
    }
}

void Game::putDots() {
    QList <QPoint> list = compass->dots();
    foreach(QPoint pos, list) {
        item[pos.x()][pos.y()] = new Dot(this);
        item[pos.x()][pos.y()]->setPos(16 * pos.y(), 35 + 16 * pos.x());
        scene->addItem(item[pos.x()][pos.y()]);
        connect(item[pos.x()][pos.y()], SIGNAL(dotEaten()), this, SLOT(dotsAte()));
    }

    list = compass->pellets();
    foreach(QPoint pos, list) {
        item[pos.x()][pos.y()] = new Pellet(this);
        item[pos.x()][pos.y()]->setPos(16 * pos.y(), 35 + 16 * pos.x());
        scene->addItem(item[pos.x()][pos.y()]);
        connect(shine, SIGNAL(timeout()), item[pos.x()][pos.y()], SLOT(shine()));
        connect(item[pos.x()][pos.y()], SIGNAL(pelletEaten()), this, SLOT(pelletAte()));
    }

}

void Game::itemEat(QPoint pos) {
    if (mode == Mode::Play) {
        item[pos.x()][pos.y()]->eaten();
    }
}

void Game::pause() {
    pacmanMove->stop();
    ghostMove->stop();
    shine->stop();
}

void Game::resume() {
    pacmanMove->start();
    ghostMove->stop();
    shine->start();
}

void Game::dotsAte() {
    board->addScore(10);
    remainDots --;
    if (!remainDots)
        gameClear();
}

void Game::pelletAte() {
    board->addScore(50);
}

void Game::gameStart() {
    // change mode
    mode = Mode::Play;

    // put the dots and pellets on the map
    putDots();

    // set timer start
    pacmanMove->start(20);
    ghostMove->start(10);
    shine->start(300);

    player->setPos(width / 2 - player->boundingRect().width() / 2 + 7, 403);

    remainDots = 244;

    board->reset();

    // hide the items not used in playing mode
    title->hide();
    play->hide();
    quit->hide();

    // show the items
    player->show();
    board->show();
}

void Game::gameClear() {
    // game clear
}

void Game::gameFail() {
    mode = Mode::Result;
    pacmanMove->stop();
    ghostMove->stop();
    blinky->hide();
    pinky->hide();
    inky->hide();
    clyde->hide();

    player->die();
    clearDots();
}

void Game::clearDots() {
    QList <QPoint> list = compass->remainDots();
    foreach(QPoint pos, list) {
        qDebug() << pos;
        delete item[pos.x()][pos.y()];
    }
}
