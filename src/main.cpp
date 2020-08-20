#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/WindowsSTD.hpp>
#include <queue>


/*Creando a la clase de la entidad*/
class snake : public EGE::CORE::Entity<snake>{
    private:
        char direction = 's';
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
            //this -> create.snakePiece(snake);
            //this -> create.snakePiece(snake);
            //this -> create.snakePiece(snake);
        }
};

class point{
    private:
        int x;
        int y;
        char direction;
        size_t lifeCounter = 0;
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

        size_t getLifeCounter(){
            return this ->lifeCounter;
        }



};


class systemMoveSnake{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnake> displacement;
        EGE::STD::TERMINAL::WINDOWS::moveEntity<mSnake> move;
        EGE::STD::TERMINAL::WINDOWS::systemPositionReset<mSnake> reset;
        systemViewSnake view;
        std::vector<point> points;
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
                /*Obtenemos a la entidad de la cabeza*/
                auto snakeHead = manager -> getEntity<snake>(0);

                /*Accinamos su dirección*/
                snakeHead -> setDirection(key);

                /*Obtenemos su posicion*/
                auto snakeHeadComponentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
                auto snakeHeadposition = snakeHeadComponentPosition -> getFirstPosition();

                /*Marcamos el punto critico*/
                point criticalPoint(std::get<0>(*snakeHeadposition),std::get<0>(*snakeHeadposition),key);
                this -> points.push_back(criticalPoint);
            }

            /*Actualizamos a las entidades de la serpiente*/
            auto snakePieces = manager -> getEntities();

            for(auto i: manager -> ids){
                if(i != 0){

                    /*Obtenemos la posición de la pieza*/
                    auto pieceComponentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(i);
                    auto piecePosition = pieceComponentPosition -> getFirstPosition();
                    auto piece = manager -> getEntity<snake>(i);

                    /*vector temporal*/
                    std::vector<point> tmp;

                    for(auto j: this ->points){
                        /*Vemos si la entidad esta en un punto critico*/
                        std::cout << "j.x: " << j.getX() << " p: " << std::get<0>(*piecePosition);
                        if(std::get<1>(*piecePosition) == j.getY()){
                            if(std::get<0>(*piecePosition) == j.getX()){
                                
                                /*Actualizamos su dirección*/
                                piece -> setDirection(j.getDirection());
                                
                                /*Incrementamos el contador del punto critico*/
                                j.increaseLifeCounter();
                            }
                        }
                        tmp.push_back(j);
                    }
                    auto p = manager -> getEntity<snake>(i);
                    std::cout <<"Direccion: "<< p -> getDirection() << std::endl;
                    
                    this ->points.swap(tmp);

                     /*Vemos si continua vivo*/
                    for(auto k = this ->points.begin(); k!= this -> points.end(); k++){
                        if(k ->getLifeCounter() == snakePieces.size()-1){
                            this -> points.erase(k);
                        }
                     }


                    this -> displacement.update(piece ->getDirection(),i,manager,WASD);
                }else{
                    auto piece = manager -> getEntity<snake>(i);
                    this ->displacement.update(piece ->getDirection(),i,manager,WASD);
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
        Sleep(300);
    }


    
}
