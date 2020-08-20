#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/WindowsSTD.hpp>
#include <queue>


/*Creando a la clase de la entidad*/
class snake : public EGE::CORE::Entity<snake>{
    private:
        char direction = 'd';
    public:
        snake(EGE::CORE::EntityId id): Entity(id){};

        void setDirection(char direction){
            this -> direction = direction;
        }

        char getDirection(){
            return this -> direction;
        }
};



/*Creando al manager de la entidad snake*/
class mSnake : public EGE::STD::TERMINAL::WINDOWS::mSprite<snake>{
    public:
        std::vector<EGE::CORE::EntityId> ids;

};


class systemCreateSnake{
    public:
        
        void snakeHead(mSnake *snake){
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> spriteInitializer(id,1,"snakeHead");
            snake -> positionInitializer(id,5,5);
            snake -> ids.push_back(id);
        }

        void snakePiece(mSnake *snake){
            auto snakePieces = snake -> getEntities();
            EGE::CORE::EntityId id = snake -> addEntity();
            auto lastPieceId = id-1;
            snake -> ids.push_back(id);

            auto lastPieceComponentPosition = snake -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(lastPieceId);
            
            auto lastPiecePosition = lastPieceComponentPosition -> getFirstPosition();

            snake -> spriteInitializer(id,1,"snakePiece");
            snake -> positionInitializer(id,std::get<0>(*lastPiecePosition)-1,std::get<1>(*lastPiecePosition));
        }
};

class systemViewSnake{

    private:
        EGE::STD::TERMINAL::WINDOWS::systemVisualizeEntity<mSnake> view;

    public:
        void viewSnake(mSnake *snake,bool view = true){
            
            auto snakePieces = snake -> getEntities();

            for(auto i : snakePieces){
                this -> view.viewColor(i.first,snake,249,view);
            }
        }
};

class systemSnakeInitializer{
    private:
        systemCreateSnake create;
    public:
        void initializer(mSnake *snake){
            this -> create.snakeHead(snake);
            this -> create.snakePiece(snake);
            this -> create.snakePiece(snake);
            this -> create.snakePiece(snake);
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
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnake> displacement;
        EGE::STD::TERMINAL::WINDOWS::moveEntity<mSnake> move;
        EGE::STD::TERMINAL::WINDOWS::systemPositionReset<mSnake> reset;
        EGE::STD::TERMINAL::WINDOWS::systemKeyInverter inverter;
        systemViewSnake view;
        std::vector<point*> points;
        char keys[8] = {'w','W','a','A','s','S','d','D'};
    public:

        void moveSnake(char key,mSnake *manager){
            view.viewSnake(manager,false);
            bool flag = false;

            for(auto i : this -> keys){
                if(i == key){
                    flag = true;
                    break;
                }
            }

            if(flag){
                auto snakeHead = manager -> getEntity<snake>(0);

                /*Verificamos que no sea el contrario*/
                if(this -> inverter.update(snakeHead ->getDirection(),WASD) != key){
                    snakeHead -> setDirection(key);
                    auto componentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
                    auto position = componentPosition -> getFirstPosition();

                    this ->points.push_back(new point(std::get<0>(*position),std::get<1>(*position),key));
                }
            }


            bool isMove;

            for(auto i: manager -> ids){
                auto piece = manager -> getEntity<snake>(i);
                auto componentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(i);
                auto position = componentPosition -> getFirstPosition();

                if(i == 0){
                    isMove = this -> displacement.update(piece -> getDirection(),i,manager,WASD);
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
                            it = this -> points.erase(it);
                        }else{
                            ++it;
                        }
                    }

                    this -> move.update(piece -> getDirection(),i,manager);
                }
            }
            view.viewSnake(manager);
        }
};


int main(){

    /*Inicalizaciones antes de ljuego*/
    mSnake snake;
    bool gameOver = false;
    char tecla = 0;
    EGE::STD::TERMINAL::WINDOWS::mTerminal tablero;

    EGE::CORE::EntityId tableroId = tablero.addEntity(20,20);

    tablero.terminalSetColor(tableroId,240);
    tablero.terminalPersonalized(tableroId);

    /*Sistemas iniciadores*/
    systemSnakeInitializer init;
    systemViewSnake view;
    EGE::STD::TERMINAL::WINDOWS::systemInput entrada;
    init.initializer(&snake);
    view.viewSnake(&snake);

    /*Controles*/
    systemMoveSnake control;

    while(!gameOver){
        tecla = entrada.update();

        if(tecla == 'c'){
            gameOver = true;
        }


        control.moveSnake(tecla,&snake);
        Sleep(100);
    }


    
}
