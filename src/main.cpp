#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/Others/WindowsSTD.hpp>
#include <queue>


/*Creando a la clase de la entidad*/
class snakePiece : public EGE::CORE::Entity<snakePiece>{
    private:
        char direction = 'd';
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


class systemCreateSnakePiece{
    public:
        
        void snakeHead(mSnakePiece *snake){
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> spriteInitializer(id,1,"snakeHead");
            snake -> positionInitializer(id,5,5);
            snake -> ids.push_back(id);
        }

        void snakePiece(mSnakePiece *snake){
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

    scoreNum.scoreInitializer(scoreOfSnake,&scoreSnake);
    init.initializer(&snake);
    view.viewSnake(&snake);
    viewScore.viewColor(scoreOfSnake,&scoreSnake,240);


    /*Controles*/
    systemMoveSnake control;
    systemLimitScore limit;

    while(!gameOver){
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

        Sleep(100);
    }


    
}
