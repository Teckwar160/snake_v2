#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/WindowsSTD.hpp>


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

};


class systemCreateSnake{
    public:
        
        void snakeHead(mSnake *snake){
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> spriteInitializer(id,1,"snakeHead");
            snake -> positionInitializer(id,5,5);
        }

        void snakePiece(mSnake *snake){
            auto snakePieces = snake -> getEntities();
            EGE::CORE::EntityId id = snake -> addEntity();
            auto lastPieceId = id-1;

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
            //this -> create.snakePiece(snake);
            //this -> create.snakePiece(snake);
        }
};

class point{
    public:
        point(int x,int y, char c){
            
        }
        int x;
        int y;
        char c;
};


class systemMoveSnake{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnake> displacement;
        EGE::STD::TERMINAL::WINDOWS::systemPositionReset<mSnake> reset;
        systemViewSnake view;
        std::vector<point> points;
        char keys[8] = {'w','W','a','A','s','S','d','D'};
    public:

        void moveSnake(char key,mSnake *manager){

            bool flag = false;

            for(auto i : this -> keys){
                if(i == key){
                    flag = true;
                    break;
                }
            }

            if(flag){
                auto snakeHead = manager -> getEntity<snake>(0);
                snakeHead -> setDirection(key);
                auto snakeHeadComponentPosition = manager -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
                auto snakeHeadposition = snakeHeadComponentPosition -> getFirstPosition();
                this -> points.push_back(std::make_pair(std::get<0>(*snakeHeadposition),std::get<0>(*snakeHeadposition)),key);

            }
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

        if(tecla != 0){
            control.moveSnake(tecla,&snake);
        }
    }


    
}
