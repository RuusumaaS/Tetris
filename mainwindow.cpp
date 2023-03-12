#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>
#include <QAbstractGraphicsShapeItem>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);
    this->timer_.setInterval(1000);
    connect(&timer_, &QTimer::timeout, this, &MainWindow::drop_tetromino);

    //Toinen ajastin, jolla seurataan peliin kestänyttä aikaa.
    this->timer2_.setInterval(1000);
    connect(&timer2_, &QTimer::timeout, this, &MainWindow::display_clock);

    // The graphicsView object is placed on the window
    // at the following coordinates, but if you want
    // different placement, you can change their values.
    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.

    //Pelialustaan luodaan ruudukko, joinka viivat ovat harmaita.
    //Ruudukko luodaan addLine-funktiota käyttäen.
    QPen grayPen(Qt::gray);


    //Lisätään alueeseen ruudukko addLine funktion viivoilla.
    for(int y = 1; y < y_max_; ++y){
        scene_->addLine(0, y*STEP_, x_max_*STEP_ , y*STEP_, grayPen);
    }
    for(int x = 1; x < x_max_;++x){
        scene_->addLine(x*STEP_, 0, x*STEP_, y_max_*STEP_, grayPen);
    }

    //Tetris-otsikon asetukset.
    ui->t1Label->setStyleSheet("font: 40pt; color: cyan;");
    ui->eLabel->setStyleSheet("font: 40pt; color: blue;");
    ui->t2Label->setStyleSheet("font: 40pt; color: magenta;");
    ui->rLabel->setStyleSheet("font: 40pt; color: yellow;");
    ui->iLabel->setStyleSheet("font: 40pt; color: green;");
    ui->sLabel->setStyleSheet("font: 40pt; color: darkmagenta;");
    ui->markLabel->setStyleSheet("font: 40pt; color: red;");

    //Alussa kyseiset napit ovat poissa käytöstä, kunnes peli aloitetaan.
    ui->dropButton->setDisabled(true);
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}


//Funktio luo uuden tetronimon. Funktio on pitkä, koska tetrominoja on monta, joten
//toisen funktion käyttäminen tekisi tilanteesta monimutkaisemman tai pitäisi tehdä seitsemän funktiota lisää.
//Alussa määrätään tetronimonien reunojen väriksi musta ja y-koordinaatit,
//mihin neliöt luodaan. Ne luodaan aina vain kahdelle ylimmälle riville ja
//suurin piirtein keskelle x-akselia. Jos ne eivät mahdu niille paikoille,
//pelin häviää. If rakenteessa tarkistetaan, mikä tetronimo on kyseessä. Index
//vastaa enum-rakenteessa olevia tetrominoja.
//Kun tetromino luodaan, sen neliöt lisätään falling_tetromino-vektoriin.
//Ensin lisätään alemman rivin tetrominot ja sen jälkeen ylemmän ja ne lisätään
//järjestyksessä vasemmalta oikealle. Näin tehdessä pohjan tuleminen on helppo tarkastella
//sillä riittää, että falling_tetrominon ensimmäisen neliön osuessa pohjaan, teromino pysähtyy.

void MainWindow::new_tetromino(int index)
{
    //Laudan kaksi ylinä riviä.
    int y1 = 0;
    int y2 = 1;

    //Vasen reuna.
    int x0 = 0;



    //Muuttujaa square käytetään neliöitä luodessa.
    QGraphicsRectItem* square;

    //Jokaisen luodun neliön reunat värjätään mustaksi.
    QPen black(Qt::black);

    //"Horizontal" tetrominon luonti.
    if(index == 0){
        QBrush cyan_brush(Qt::cyan);

        for(int x = 0; x < 4; ++x){

            //Jokainen tetrominon neliö jokaiselle terominolle tehdään niin, että laitetaan
            //ne ensiksi mahdollisimman vasemmalle ja sitten funktiolla setX asetetaan oikeaan kohtaan.
            //Tämä tapahtuu siis moneen kertaan myöhemmin ja sitä ei niissä erikseen kommentoida.
            square = scene_->addRect(x0  ,y1,STEP_, STEP_,black,cyan_brush);
            square->setX( (x+4)*STEP_);

            //Lisätään neliö putoavaan tetrominoon.
            falling_tetromino_.push_back(square);

        }
    }

    //Left_corner -tetromino
    else if(index == 1){

        //Tetrominon väri. Jokaisella tulee olemaan oma värinsä. Väri määritellään aina
        //heti if ehdon perään.
        QBrush blue_brush(Qt::blue);


        //Alarivin neliöt
        for(int x = 0; x < 3; ++x){
            //Kuten x-akselilla aina asetetaan neliö ensiksi vasempaan reunaan ja sitten määritetään
            //erikseen se oaikka haluttuun paikkaan, myös y:n arvo on aina ensiksi nolla.
            //Sitten se lasketaan toisiksi ylimmälle riville setY:llä, jos se sinne kuuluu.
            square = scene_->addRect(x0 *STEP_  ,y1,STEP_, STEP_,black,blue_brush);
            square->setX( (x+4)*STEP_);
            square->setY(y2*STEP_);

            falling_tetromino_.push_back(square);
        }
        //Lisätään ylärivin yksi neliö mukaan tetrominoon.
        square = scene_->addRect(x0,y1,STEP_, STEP_,black,blue_brush);
            square->setX( (x0+4)*STEP_ );

            falling_tetromino_.push_back(square);

    }
    //Right_corner -tetromino
    else if(index == 2){

        QBrush magenta_brush(Qt::magenta);

        //Muuttuja, joka tarvitaan ylärivin neliön paikan säätämiseen.
        int x1 = 2;

        //Alarivi
        for(int x = 0; x < 3; ++x){
            square = scene_->addRect(x0*STEP_ , y1,STEP_, STEP_,black,magenta_brush);
            square->setX( (x+4)*STEP_);

            square->setY(y2*STEP_);

            falling_tetromino_.push_back(square);

        }
        //Ylärivin yksi neliö
        square = scene_->addRect(x0,y1,STEP_, STEP_,black,magenta_brush);
        square->setX( (x1+4)*STEP_ );

        falling_tetromino_.push_back(square);

    }

    //Square-tetromino.
    else if(index == 3){

        QBrush yellow_brush(Qt::yellow);

        //Neliössä tetrominon neliöillä on samat x-akselin paikat, joten kahdella for-loopilla
        //saadaan helposti oikeat paikat neliöille.
        for(int y = 1; y > -1; --y){

            for(int x = 0; x < 2; ++x){
                square = scene_->addRect(x0,y1,STEP_, STEP_,black,yellow_brush);
                square->setX( (x+5)*STEP_);
                square->setY(y*STEP_);

                falling_tetromino_.push_back(square);
            }
        }
    }

    //Step_up_right -tetromino.
    else if(index == 4){
        QBrush green_brush(Qt::green);

        //Ensin alarivin neliöt
        for(int x = 0; x < 2; ++x){

            square = scene_->addRect(x0*STEP_, y1, STEP_,STEP_, black, green_brush);
            square->setX( (x+4)*STEP_);
            square->setY(y2*STEP_);

            falling_tetromino_.push_back(square);
        }

        //Ylärivi
        for(int x = 1; x < 3; ++x){

            square = scene_->addRect(x0*STEP_, y1, STEP_,STEP_, black, green_brush);
            square->setX( (x+4)*STEP_ );

            falling_tetromino_.push_back(square);
        }

    }

    //Pyramid-tetromino
    else if(index == 5){
        QBrush dmagenta_brush(Qt::darkMagenta);

        //Ylärivin neliön säätämiseen tarkoitettu muuttuja.
        int x1 = 1;

        //Alarivi
        for(int x = 0; x<3; ++x){
            square = scene_->addRect(x0*STEP_,y1,STEP_, STEP_,black,dmagenta_brush);
            square->setX( (x+4)*STEP_ );
            square->setY(y2*STEP_);

            falling_tetromino_.push_back(square);
        }
        //Ylärivin neliö
        square = scene_->addRect(x0*STEP_,y1*STEP_,STEP_, STEP_,black,dmagenta_brush);
        square->setX(( x1+4)*STEP_ );
        falling_tetromino_.push_back(square);

    }

    //Step_up_left -tetromino
    else{
        QBrush red_brush(Qt::red);

        //Alarivi
        for(int x = 1; x < 3; ++x){

            square = scene_->addRect(x0*STEP_, y1, STEP_,STEP_, black, red_brush);
            square->setX((x+4)*STEP_);
            square->setY(y2*STEP_);

            falling_tetromino_.push_back(square);
        }
        //Ylärivi
        for(int x = 0; x < 2; ++x){

            square = scene_->addRect(x0*STEP_,y1,STEP_, STEP_,black,red_brush);
            square->setX( (x+4)*STEP_ );

            falling_tetromino_.push_back(square);
        }
    }

    //Tarkistetaan, voiko tetrominoa asettaa sen normaaliin paikkaan, jos ei peli loppuu.
    //Alueesta poistetaan ennen tarkistusta luotu tetromino.
    if(check_if_game_ended()){

        game_ended();
        remove_tetromino_that_didnt_fit();
    }

    //Jos tetromino ei pysty liikkumaan alaspäin, sen jälkeen ei voi enää tulla tetrominoa,
    //joten peli loppuu. Siitä tetrominosta saa kuitenkin pisteen, koska se vielä mahtui.
    else if(not check_if_spawned_tetromino_can_move()){

        ++points_;

        //Lcd näyttää pisteet.
        ui->pointsLcd->display(points_);

        //Siirretään tetromino jo pudonneisiin tetrominoihin, jotta se saadaan helposti poistettua
        //pelin jälkeen reset-napilla.
        save_fallen_into_dropped();
        empty_fallen_tetromino();

        game_ended();


    }
    //Jos ei tullut ongelmia vaan peli voi jatkua, aika käynnistyy, jolloin tetromino putoaa ja
    //siitä lisätään piste.
    else{
        ++points_;
        ui->pointsLcd->display(points_);
        timer_.start();
    }
}


//Funktio saa tetrominon putoamaan. Ajastimen ollessa päällä funktiota kutsutaan joka sekuntti.
//Funktiossa myös tarkastellaan pysähtyykö tetromino pohjaan vai toiseen tetrominoon ja
//voiko tetrominoa siirtää vasemmalle tai oikealle.
void MainWindow::drop_tetromino()
{
    //Kaksi apumuuttujaa. Molempien ollessa nollia tetromino voi liikkua sivuttain molempiin suuntiin.
    int can_move_right = 0;
    int can_move_left = 0;

    //Tutkii voiko tetromino pudota.
    int squares_impossible_to_move = 0;

    //For-loop joka liikuttaa tetrominoa alaspäin, jos pohja ei tule vastaan.
    for(unsigned int index = 0; index < falling_tetromino_.size(); ++index){

        //Neliön x- ja y-koordinaatti.
        qreal current_x = falling_tetromino_.at(index)->x();
        qreal current_y = falling_tetromino_.at(index)->y();

        //Kasvatetaan y:tä yhdellä askeleella eli sen verran kuinka paljon neliöt liikkuvat.
        current_y += STEP_;

        //Muuttuja, jolla tutkitaan pelialuetta
        QRectF rect = scene_->sceneRect();

        //Jos alue ei lopu kesken neliö tippuu yhden ruudun verran.
        if(rect.contains(current_x, current_y)) {
            falling_tetromino_.at(index)->moveBy(0,STEP_);

            //Tutkitaan dropped_squares-vektorilla, onko jonkin putoavan tetrominon neliön alapuolla
            //toisen tetrominon neliö. Jos on, squares_impossible to move arvo kasvaa.
            for(unsigned int index = 0; index < dropped_squares_.size(); ++index){

                if(dropped_squares_.at(index)->y() == current_y+STEP_ and
                        dropped_squares_.at(index)->x() == current_x){
                    ++squares_impossible_to_move;
                }
                //Tarkistetaan, voiko tetromino liikkua joko vasemmalle tai oikealle vai onko
                //en vieressä toinen tetromino. Jos ei voi, toisen suunnan tai mahdollisesti
                //molempien suuntien can_move -muuttujan arvo kasvaa.
                //Nämä funktiot eivät tutki alueen rajaa vaan ainoastaan muita tetrominoja.
                update_left_button_while_falling(index, current_x, current_y, can_move_left);
                update_right_button_while_falling(index, current_x, current_y, can_move_right);
            }
            //Suunnan can_move arvon noustessa, sen suunnan nappi lukitaan. Jos se voi liikkua
            //nappi avataan.
            if(can_move_left != 0){
                ui->leftButton->setDisabled(true);
            }
            else{
                ui->leftButton->setEnabled(true);
            }
            if(can_move_right != 0){
                ui->rightButton->setDisabled(true);
            }
            else{
                ui->rightButton->setEnabled(true);
            }
            //Tarkistetaan onko tetromino reunalla tutkimalla sen kaikkia neliöitä.
            //Jos on toinen liikkuminapeista lukitaan riippuen kummalla reunalla ollaan.
            for(unsigned int index = 0; index < falling_tetromino_.size(); ++index){
                check_if_border_at_left(index);
                check_if_border_at_right(index);

            }
        }

        //Jos alue loppuu kesken, ajastin pysähtyy ja tetromino pysähtyy.
        else {
            timer_.stop();

            //Perutaan kaikkien liikumisnappien ja tetrominon pudottamisnapi lukitus.
            enable_all_buttons();

            //tallennetaan tetromino pudonneisiin ja tyjennetään putoava tetromino-vektori.
            save_fallen_into_dropped();

            empty_fallen_tetromino();
        }
    }
    //Jos tetrominon alapuolella on toinen tetromino tetromino pysähtyy.
    if(squares_impossible_to_move != 0){
        timer_.stop();

        enable_all_buttons();

        save_fallen_into_dropped();

        empty_fallen_tetromino();
    }
}

//Avaa lukitukset nappuloista.
void MainWindow::enable_all_buttons()
{
    ui->dropButton->setEnabled(true);
    ui->rightButton->setEnabled(true);
    ui->leftButton->setEnabled(true);
}

//Lukitsee nappulat.
void MainWindow::disable_buttons()
{
    ui->dropButton->setDisabled(true);
    ui->leftButton->setDisabled(true);
    ui->rightButton->setDisabled(true);
}

//Tallentaa putoavan tetrominon neliöt jo pudonneisiin tetrominon pysähtyessä.
void MainWindow::save_fallen_into_dropped()
{
    for(unsigned int index = 0; index < falling_tetromino_.size(); ++index){
        dropped_squares_.push_back(falling_tetromino_.at(index));
    }
}

//Tyhjentää falling_tetromino_ -vektorin.
void MainWindow::empty_fallen_tetromino()
{
    while(falling_tetromino_.size() != 0){
        falling_tetromino_.pop_back();
    }
}

//Käytetään tutkittaessa, onko toinen tetromino putoavan vieressä, jotta voidaan estää liike vasemmalle.
void MainWindow::update_left_button_while_falling(unsigned int index, qreal x, qreal y, int& movable)
{
    if(dropped_squares_.at(index)->x() == x-STEP_ and
          dropped_squares_.at(index)->y() ==  y ){
        ++movable;
    }
}

//Vastaav kuin ylempi, mutta oikealle.
void MainWindow::update_right_button_while_falling(unsigned int index, qreal x, qreal y,int& movable)
{
    if(dropped_squares_.at(index)->x() == x+STEP_ and
            dropped_squares_.at(index)->y() == y){
        ++movable;
    }

}

//Tarkistaa onko tetromino vasemmalla rajalla.
void MainWindow::check_if_border_at_left(int index)
{
    if(not scene_->sceneRect().contains(falling_tetromino_.at(index)->x()-STEP_,
                                        falling_tetromino_.at(index)->y())){
        ui->leftButton->setDisabled(true);
    }
}

//Oikean rajan tarkistus.
void MainWindow::check_if_border_at_right(int index)
{
    if(not scene_->sceneRect().contains(falling_tetromino_.at(index)->x()+STEP_,
                                        falling_tetromino_.at(index)->y())){
        ui->rightButton->setDisabled(true);
    }
}

//Tarkistaa voiko luotu tetromino tippua yhtään, vai jääkö se heti paikoilleen.
bool MainWindow::check_if_spawned_tetromino_can_move()
{
    for(unsigned int index1 = 0; index1 < falling_tetromino_.size();++index1){

        //Tarkistaa for-loopeilla, onko tetromino alla toista tetrominoa.
        for(unsigned int index = 0; index < dropped_squares_.size(); ++index){
            if(dropped_squares_.at(index)->x() == falling_tetromino_.at(index1)->x() and
                    dropped_squares_.at(index)->y() == falling_tetromino_.at(index1)->y()+STEP_){
                return false;
            }
        }
    }
    return true;
}

//Tarkistaa, voidaanko tetrominoa luoda paikalleen eli loppuuko peli.
bool MainWindow::check_if_game_ended()
{
    for(unsigned int index1 = 0; index1 < falling_tetromino_.size();++index1){

        //tarkistaa onko luotavan tetrominon jonkin neliön paikalla jo jonkin toisen tetrominon neliö.
        for(unsigned int index = 0; index < dropped_squares_.size(); ++index){
            if(dropped_squares_.at(index)->x() == falling_tetromino_.at(index1)->x() and
                    dropped_squares_.at(index)->y() == falling_tetromino_.at(index1)->y()){
                return true;
            }
        }
    }
    return false;
}

//Jos tetromino ei mahdu, se poistetaan. Käytetään myös reset-nappulan kanssa.
void MainWindow::remove_tetromino_that_didnt_fit()
{
    while(falling_tetromino_.size() != 0){
        scene_->removeItem( falling_tetromino_.at(0));
        falling_tetromino_.erase(falling_tetromino_.begin());

    }
}

//Pelin päättyessä, lukitaan liikkumisnapit ja uuden tetrominon pudottava nappi.
//Tulostaa tekstin joka kertoo pelin olevan ohi.
void MainWindow::game_ended()
{
    disable_buttons();
    ui->gameoverLabel->setText("Game over!");
    ui->gameoverLabel->setStyleSheet("font: 26pt;");
    timer2_.stop();
}

//Päivittä lcdNumber-komponentteihin aikaa.)
void MainWindow::display_clock()
{
    ++seconds_;
    if(seconds_ == 60){
        seconds_ = 0;
        ++minutes_;
    }
    ui->lcdNumberSec->display(seconds_);
    ui->lcdNumberMin->display(minutes_);
}

//Tyhjentää pelialueen poistamalla kaikki sen neliöt.
//kaikki neliöt tallennettu dropping_squaresiin paitsi sillä hetkellä liikkuva.
void MainWindow::empty_board()
{
    while(dropped_squares_.size() != 0){
        scene_->removeItem(dropped_squares_.at(0));
        dropped_squares_.erase(dropped_squares_.begin());
    }
}

//Nollaa ajan ja pisteet.
void MainWindow::empty_points_and_time()
{
    seconds_ = 0;
    ui->lcdNumberSec->display(seconds_);

    minutes_ = 0;
    ui->lcdNumberMin->display(minutes_);

    points_ = 0;
    ui->pointsLcd->display(points_);
}

//Liikuttaa tetrominoa vasemmalle yhden ruudn verran.
void MainWindow::move_left()
{
    //Jos liikutetaan oikealle, pakko pystyä liikkumaan takasin vasemmalle vaikka se olisi
    //ollut mahdotonta juuri ennen liikettä oikealle.
    ui->rightButton->setEnabled(true);

    //Muuttuja jolla tutkitaan, voiko tetromino pudota.
    int squares_impossible_to_drop = 0;

    //For-loopilla tutkitaan kaikki tetrominon neliöt ja liikutetaan niitä. Tutkitaan myös valmiiksi
    //seuraavan liikahduksen paikka, jotta lukitaan näppäin tarvittaessa.
    for(unsigned int index = 0; index < falling_tetromino_.size(); ++index){

        if(scene_->sceneRect().contains(falling_tetromino_.at(index)->x()-STEP_,
                                        falling_tetromino_.at(index)->y())){

            falling_tetromino_.at(index)->moveBy(-STEP_, 0);

            qreal current_x = falling_tetromino_.at(index)->x();
            qreal current_y = falling_tetromino_.at(index)->y();

            check_if_border_at_left(index);

            //Jos uuden paikan alapuolella on tetromino. Kasvatetaan square_impossible_to_move
            //muuttujaa.
            for(unsigned int index = 0; index < dropped_squares_.size(); ++index){

                if(dropped_squares_.at(index)->y() == current_y+STEP_ and
                        dropped_squares_.at(index)->x() == current_x){
                    ++squares_impossible_to_drop;
                }
            }
        }
    }
    //Jos alla tetromino, pysäytetään tetromino.
    if(squares_impossible_to_drop != 0){
        timer_.stop();
        enable_all_buttons();
        save_fallen_into_dropped();
        empty_fallen_tetromino();

    }
}

//Liikutta tetrominoa oikealle. Funktio toimii täsmälleen samalla tavalla kuin move_left
//paitsi nyt tutkitaan oikeaa puolta.
void MainWindow::move_right()
{

    int squares_impossible_to_drop = 0;
    ui->leftButton->setEnabled(true);

    for(unsigned int index = 0; index < falling_tetromino_.size(); ++index){
        qreal current_x = falling_tetromino_.at(index)->x();
        qreal current_y = falling_tetromino_.at(index)->y();

        if(scene_->sceneRect().contains(falling_tetromino_.at(index)->x()+STEP_,
                                        falling_tetromino_.at(index)->y())){

            falling_tetromino_.at(index)->moveBy(STEP_,0);
            current_x += STEP_;

            //Jos yksikin tetrominon paloista on reunan vieressä, oikealle liikkumiseen
            //tarkoitettu näppäin lukitaan.
            check_if_border_at_right(index);
            for(unsigned int index = 0; index < dropped_squares_.size(); ++index){

                if(dropped_squares_.at(index)->y() == current_y+STEP_ and
                        dropped_squares_.at(index)->x() == current_x){
                    ++squares_impossible_to_drop;
                }
            }

        }
    }
    if(squares_impossible_to_drop != 0){
        timer_.stop();
        enable_all_buttons();
        save_fallen_into_dropped();
        empty_fallen_tetromino();

    }
}

//Tetrominojen tiputus nappi.
void MainWindow::on_dropButton_clicked()
{
    //Arvotaan tetromino ja annetaan sen luku new_tetromino -funktion parametriksi.
    int index = distr(randomEng);

    //otetaan nappi pois käytöstä kunne tetromino pysähtyy.
    ui->dropButton->setDisabled(true);
    new_tetromino(index);

}

//Move left -nappi.
void MainWindow::on_leftButton_clicked()
{
    move_left();
}
//move right -nappi.
void MainWindow::on_rightButton_clicked()
{
    move_right();
}

//Pelin aloitus -nappi.Käynnistää peliajan. Tätä painettua ei voi enää vaihtaa vaikeutta.
void MainWindow::on_startButton_clicked()
{
    timer2_.start();
    enable_all_buttons();
    ui->difficultyButton->setDisabled(true);
    ui->startButton->setDisabled(true);
}

//Tyhjennetään alue, nollataan pisteet ja ajat, lukitaan liikenappulat, pysäytetään molemmat
//ajastimet ja avataan start_button ja difficulty_button. Myös perutaan mahdollinen Game over -teksti.
void MainWindow::on_resetButton_clicked()
{
    timer_.stop();
    remove_tetromino_that_didnt_fit();
    ui->gameoverLabel->setText("");
    timer2_.stop();
    empty_board();
    empty_points_and_time();
    ui->startButton->setEnabled(true);
    disable_buttons();
    ui->difficultyButton->setEnabled(true);

}

//Muuttaa vaikeustasoa ja vaihtaa nappulan tekstiä. Vaikeustaso riippuu tetrominon putoamisnopeudesta.
//Mikään ei ole erityisen vaikea, mutta halutessa sitä voi muuttaa.
void MainWindow::on_difficultyButton_clicked()
{
    if(ui->difficultyButton->text() == "Easy"){
        timer_.setInterval(750);
        ui->difficultyButton->setText("Medium");
    }
    else if(ui->difficultyButton->text() == "Medium"){
        timer_.setInterval(500);
        ui->difficultyButton->setText("Hard");
    }
    else{
        ui->difficultyButton->setText("Easy");
        timer_.setInterval(1000);
    }
}
