#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/Others/WindowsSTD.hpp>

#include <stdlib.h>
#include <time.h>
#include <iostream>

const int winPoints =900;


/*Creando a la clase de la entidad*/
class snakePiece : public EGE::CORE::Entity<snakePiece>{
    private:
        char direction;
    public:
        snakePiece(EGE::CORE::EntityId id): Entity(id){};

        void setDirection(char direction){
            this -> direction = direction;
        }

        char getDirection(){
            return this -> direction;
        }
};



/*Creando al manager de la entidad snake*/
class mSnakePiece : public EGE::STD::TERMINAL::WINDOWS::mSprite<snakePiece>{
    public:
        std::vector<EGE::CORE::EntityId> ids;

};

/*Score*/
class score : public EGE::CORE::Entity<score>{
    public:
        score(EGE::CORE::EntityId id) : Entity(id){};
};

class mScore :public EGE::STD::TERMINAL::WINDOWS::mSprite<score>{

}; 

class gameOverBoard: public EGE::CORE::Entity<gameOverBoard>{
    public:
        gameOverBoard(EGE::CORE::EntityId id) : Entity(id){};
};

class mGameOverBoard : public EGE::STD::TERMINAL::WINDOWS::mSprite<gameOverBoard>{

};

/*Comida*/
class food : public EGE::CORE::Entity<food>{
    public:
        food(EGE::CORE::EntityId id): Entity(id){};
};

class mFood : public EGE::STD::TERMINAL::WINDOWS::mSprite<food>{

};

/*Sistemas de la serpiente*/
class systemCreateSnakePiece{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemMoveEntity<mSnakePiece> move;
        EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;
    public:
        
        void createSnakeHead(mSnakePiece *snake){
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> spriteInitializer(id,1,"snakeHead");
            snake -> positionInitializer(id,5,5);
            snake -> ids.push_back(id);

            auto head = snake -> getEntity<snakePiece>(id);

            head -> setDirection('d');
        }

        void createSnakePiece(mSnakePiece *snake){
            auto lastPieceId = snake -> ids.size()-1;
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> ids.push_back(id);

            auto lastPieceComponentPosition = snake -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(lastPieceId);
            
            auto lastPiecePosition = lastPieceComponentPosition -> getFirstPosition();

            snake -> spriteInitializer(id,1,"snakePiece");

            /*Le asignamos la dirección*/
            auto lastPieceEntity = snake -> getEntity<snakePiece>(lastPieceId);

            auto newLastPieceEntity = snake ->getEntity<snakePiece>(id);

            newLastPieceEntity -> setDirection(lastPieceEntity -> getDirection());

            snake -> positionInitializer(id,std::get<0>(*lastPiecePosition),std::get<1>(*lastPiecePosition));

            this -> move.update(this -> inverter.update(newLastPieceEntity -> getDirection(),WASD),id,snake);




        }
};

class systemViewSnake{

    private:
        EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<mSnakePiece> view;

    public:
        void viewSnake(mSnakePiece *snake,bool view = true){
            
            auto snakePieces = snake -> getEntities();

            for(auto i : snakePieces){
                this -> view.viewColor(i.first,snake,249,view);
            }
        }
};

class systemSnakeInitializer{
    private:
        systemCreateSnakePiece create;
    public:
        void initializer(mSnakePiece *snake){
            this -> create.createSnakeHead(snake);
            this -> create.createSnakePiece(snake);
            this -> create.createSnakePiece(snake);
            this -> create.createSnakePiece(snake);
        }
};

class point{
    private:
        int x;
        int y;
        char direction;
        int lifeCounter = 0;
    public:
        point(int x,int y, char direction){
            this -> x =x;
            this -> y = y;
            this -> direction = direction;
        }

        void setX(int x){
            this -> x = x;
        }

        void setY(int y){
            this -> y = y;
        }

        void setDirection(char direction){
            this -> direction = direction;
        }

        void increaseLifeCounter(){
            this ->lifeCounter += 1;
        }

        int getX(){
            return this -> x;

        }

        int getY(){
            return this -> y;
        }

        char getDirection(){
            return this -> direction;
        }

        int getLifeCounter(){
            return this ->lifeCounter;
        }



};


class systemMoveSnake{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnakePiece> displacement;
        EGE::STD::TERMINAL::WINDOWS::systemMoveEntity<mSnakePiece> move;
        EGE::STD::TERMINAL::WINDOWS::systemPositionReset<mSnakePiece> reset;
        EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;
        systemViewSnake view;
        std::vector<point*> points;
        char keys[8] = {'w','W','a','A','s','S','d','D'};
    public:

        void moveSnake(char key,mSnakePiece *manager,bool adminMode = false,bool stopMove = false){
            view.viewSnake(manager,false);
            bool flag = false;

            for(auto i : this -> keys){
                if(i == key){
                    flag = true;
                    break;
                }
            }

            if(flag){
                auto snakeHead = manager -> getEntity<snakePiece>(0);

                /*Verificamos que no sea el contrario*/
                if(this -> inverter.update(snakeHead ->getDirection(),WASD) != key || adminMode){
                    snakeHead -> setDirection(key);
                    auto componentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
                    auto position = componentPosition -> getFirstPosition();

                    this ->points.push_back(new point(std::get<0>(*position),std::get<1>(*position),key));
                }
            }


            bool isMove;

            for(auto i: manager -> ids){
                auto piece = manager -> getEntity<snakePiece>(i);
                auto componentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(i);
                auto position = componentPosition -> getFirstPosition();

                if(i == 0){
                    

                    if(stopMove){
                        isMove = true;
                    }else{
                        isMove = this -> displacement.update(piece -> getDirection(),i,manager,WASD);
                    }

                }else if(!isMove){

                    
                    for(auto j: this ->points){
                        if(j ->getY() == std::get<1>(*position)){
                            if(j ->getX() == std::get<0>(*position)){
                                piece -> setDirection(j ->getDirection());

                                j ->increaseLifeCounter();
                            }
                        }
                    }

                    auto snakePieces = manager -> getNumEntities();
                    for(auto it = this -> points.begin(); it != this -> points.end();){
                        if((*it) -> getLifeCounter() == snakePieces-1){
                            delete *it;
                            it = this -> points.erase(it);
                        }else{
                            ++it;
                        }
                    }

                    this -> move.update(piece -> getDirection(),i,manager);
                }
            }
        }
};

class systemResetPositionSnake{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnakePiece> displacement;
        EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;
    public:
        void reset(mSnakePiece *manager){

            for(auto i: manager -> ids){
                auto piece = manager -> getEntity<snakePiece>(i);

                this -> displacement.update(this -> inverter.update(piece ->getDirection(),WASD),i,manager,WASD);
                this -> displacement.update(this -> inverter.update(piece ->getDirection(),WASD),i,manager,WASD);
            }

        }
};

class systemLimitScore{
    public:
        bool isLimit(mSnakePiece *manager){

            auto componentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
            auto position = componentPosition -> getFirstPosition();
            auto piece = manager -> getEntity<snakePiece>(0);

            if(std::get<1>(*position) == 17){
                if(piece -> getDirection() == 's' || piece -> getDirection() == 'S'){
                    return true;
                }    
            }                   

            return false;
        }
};

/**/

class systemGeneratorFood{
    private:
        EGE::CORE::EntityId idFood;

        int randomNumber(char direction){
            srand(time(NULL));

            if(direction == 'x'){
                return 1 +rand() %(20-1);

            }else{
                return 1 +rand() %(18-1);
            }

        }

    public:
        void foodInitializer(mFood *manager){
            idFood = manager -> addEntity();

            manager -> spriteInitializer(this -> idFood,1,"food");
            manager -> positionInitializer(this -> idFood,this ->randomNumber('x'),this ->randomNumber('y'));
        }

        void resetFood(mFood *manager){
            auto position = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(this -> idFood);
            auto sprite = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Sprite>(this -> idFood);

            position -> positionResetSprite(sprite -> getSprite(),this ->randomNumber('x'),this ->randomNumber('y'));
        }


};

int main(){
    /*Inicalizaciones antes de ljuego*/
    mSnakePiece snake;
    mScore scoreSnake;
    bool gameOver = false;
    char tecla = 0;
    EGE::STD::TERMINAL::WINDOWS::mTerminal tablero;

    EGE::CORE::EntityId tableroId = tablero.addEntity(20,20);
    auto scoreOfSnake = scoreSnake.addEntity();

    scoreSnake.spriteInitializer(scoreOfSnake,19,"score");
    scoreSnake.positionInitializer(scoreOfSnake,1,1);

    tablero.terminalSetColor(tableroId,240);
    tablero.terminalPersonalized(tableroId);

    /*Sistemas iniciadores*/
    systemSnakeInitializer init;
    systemViewSnake view;
    EGE::STD::TERMINAL::WINDOWS::systemInput entrada;
    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<mScore> viewScore;
    EGE::STD::TERMINAL::WINDOWS::systemScore<mScore> scoreNum;
    systemCreateSnakePiece create;

    /*Comida*/
    mFood chef;

    systemGeneratorFood generator;

    generator.foodInitializer(&chef);

    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<mFood> viewFood;
    EGE::STD::TERMINAL::WINDOWS::systemGenericCollition<mSnakePiece,mFood> collitionFood;


    /*Score*/
    scoreNum.scoreInitializer(scoreOfSnake,&scoreSnake);
    init.initializer(&snake);
    view.viewSnake(&snake);
    viewScore.viewColor(scoreOfSnake,&scoreSnake,240);
    viewFood.viewColor(0,&chef,249);

    /*Controles*/
    systemMoveSnake control;
    systemLimitScore limit;



    while(!gameOver){

        viewScore.viewColor(scoreOfSnake,&scoreSnake,240);

        tecla = entrada.update();

        if(tecla == 'c'){
            gameOver = true;
        }

        if(limit.isLimit(&snake)){
            control.moveSnake(tecla,&snake,false,true);
            if(tecla != 's' && tecla != 'S' && tecla != 0){
                control.moveSnake(tecla,&snake,true);
            }
        }else{
            control.moveSnake(tecla,&snake);
        }

        view.viewSnake(&snake);

        if(collitionFood.collition(0,&snake,&chef)){
            viewFood.viewColor(0,&chef,249,false);
            generator.resetFood(&chef);
            scoreNum.update(10,0,&scoreSnake);
            create.createSnakePiece(&snake);
            viewFood.viewColor(0,&chef,249,true);
        }

        if(scoreNum.getPoints() >= winPoints){
            gameOver = true;
        }


        Sleep(80);
    }

    system("cls");
    tablero.terminalPersonalized(tableroId);

    /*Pantalla final*/
    mGameOverBoard boardEnd;
    EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<mGameOverBoard> viewBoard;
    auto board = boardEnd.addEntity();

   if(scoreNum.getPoints() >= winPoints){
        boardEnd.spriteInitializer(board,19,"winGame");
    }else{
      boardEnd.spriteInitializer(board,19,"exitGame");
    }

    boardEnd.positionInitializer(board,1,1);
    viewBoard.viewColor(0,&boardEnd,249);
}
