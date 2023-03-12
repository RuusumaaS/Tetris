#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsScene>
#include <random>
#include <QGraphicsRectItem>
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include<iostream>
#include<vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:

    void on_dropButton_clicked();

    void on_leftButton_clicked();

    void on_rightButton_clicked();

    void on_startButton_clicked();

    void on_resetButton_clicked();

    void on_difficultyButton_clicked();

private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    std::vector<QGraphicsRectItem> squares_;

    QTimer timer_;

    QTimer timer2_;

    //Funktio, joka luo uuden tetrominon, jos se on maahdollista.
    void new_tetromino(int index);

    //Funktio, jota käytetään tetrominon pudottamiseen.
    void drop_tetromino();

    //Funktio peruu näppäinten move_left, move_right ja drop_tetrominon lukituksen.
    //Alunperin oli vain kolme nappia, sitä nimi all_buttons.
    void enable_all_buttons();

    //Lukitsee näppäimet, jotka enable_all_buttons aukaisee.
    void disable_buttons();

    //Lisää falling_tetromino-vektorin neliöt dropped_tetromino-vektoriin.
    void save_fallen_into_dropped();

    //Tyjentää fallen:tetromino-ektorin.
    void empty_fallen_tetromino();

    //Funktio kasvattaa movablen arvoa, jos vasemmalle ei voi siirtyä eli jos vasemmalla
    //on toinen tetromino. Movablen ollessa jotain muuta kuin nolla, move_left -näppäin lukitaan.
    void update_left_button_while_falling(unsigned int index, qreal x, qreal y, int& movable);

    //Sama kuin ylemmällä mutta tarkistaa oikeaa puolta.
    void update_right_button_while_falling(unsigned int index, qreal x, qreal y, int& movable);

    //Tarkistaa, onko alueen raja putoavan tetrominon vieressä vasemmalla.
    void check_if_border_at_left(int index);

    //Tarkistaa oikean puolen
    void check_if_border_at_right(int index);

    //Tetrominon luodessa tarkistetaan, voiko se tippua ollenkaan vaan jääkö se paikoilleen.
    bool check_if_spawned_tetromino_can_move();

    //Testaa, voiko uutta tetrominoa sijoittaa laudalle, jos ei peli loppuu.
    bool check_if_game_ended();

    //Kun tetromino on luotu, jos sen paikalla on jo toinen tetromino, uusi poistetaan.
    void remove_tetromino_that_didnt_fit();
    
    //Pelin lopussa muun muassa ajastimen pysäyttävä  ja pelin loppumis-ilmoituksen
    //tulostava funktio.
    void game_ended();

    //Päivittää kelloa, jokaisella sekunnilla.
    void display_clock();

    //Poistaa kaikki neliöt pelialueelta.
    void empty_board();

    //Nolla pisteet ja ajan.
    void empty_points_and_time();

    //Liikuttaa tetrominoa vasemmalle yhden ruudun verran.
    void move_left();

    //Liikuttaa oikealle ruudun verran.
    void move_right();


    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 360; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)

    //Vakio, joka kertoo kuinka paljon neliö liikkuu kerrallaan.
    const int STEP_ = 20;

    //Y:n ja x:n suurimmat indeksit pelikentävektorissa. Saatu jakamalla
    //kentän leveys ja korkeus askeleen koolla.
    const int y_max_ = 18;
    const int x_max_ = 12;


    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};

    //Vekorit, joka sisältää kaikki jo pudotettujen terominojen neliöt. Näin saadaan vältettyä
    //niitä menemästä päällekäin.
    std::vector<QGraphicsRectItem*> dropped_squares_;

    //Vektori, joka sisältää sen hetkisen tippuvan tetrominon neliöt.
    std::vector<QGraphicsRectItem*> falling_tetromino_;
    
    //Pelaajan pisteet.
    int points_ = 0;

    //Peliajan sekunnit ja minuutit.
    int seconds_ = 0;
    int minutes_ = 0;

    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.


    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // More constants, attibutes, and methods
};

#endif // MAINWINDOW_HH
