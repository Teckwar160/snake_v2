#include <iostream>
#include <core/Core.hpp>
#include <std/Windows/WindowsSTD.hpp>


/*Creando a la clase de la entidad*/
class snake : public EGE::CORE::Entity<snake>{
    public:
        snake(EGE::CORE::EntityId id): Entity(id){};
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
            snake -> ids.push_back(id);
            snake -> spriteInitializer(id,1,"snakeHead");
            snake -> positionInitializer(id,5,5);
        }

        void snakePiece(mSnake *snake){
            auto snakePieces = snake -> getEntities();
            EGE::CORE::EntityId id = snake -> addEntity();
            snake -> ids.push_back(id);
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

class pixel{
    public:
    pixel(int x,int y){this ->x = x;this -> y = y;};
        int x;
        int y;
};

class systemMoveSnake{
    private:
        EGE::STD::TERMINAL::WINDOWS::systemDisplacementEntity<mSnake> displacement;
        EGE::STD::TERMINAL::WINDOWS::systemPositionReset<mSnake> reset;
        systemViewSnake view;
        char keys[8] = {'w','W','a','A','s','S','d','D'};
    public:

        void moveSnake(char key,mSnake *snake){

            bool flag = false;

            for(auto i : this -> keys){
                if(i == key){
                    flag = true;
                    break;
                }
            }

            if(flag){

                /*Variables*/
                EGE::STD::TERMINAL::WINDOWS::Position *snakePieceComponentPosition;
                std::tuple<int,int> *snakePiecePosition;
                pixel snakePiecePositionTuple(0,0);
                pixel snakePiecePositionTupleTmp(0,0);

                /*Borramos a la vieja snake*/
                this -> view.viewSnake(snake,false);

                /*Guardamos la vieja posición de la cabeza*/
                snakePieceComponentPosition = snake -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(0);
                snakePiecePosition = snakePieceComponentPosition -> getFirstPosition();
                snakePiecePositionTuple.x = std::get<0>(*snakePiecePosition);
                snakePiecePositionTuple.y = std::get<1>(*snakePiecePosition);

                /*Actualizamos a la cabeza*/
                this -> displacement.update(key,0,snake,WASD);


                for(size_t i = 1; i< snake -> ids.size(); i++){

                    snakePieceComponentPosition = snake -> getComponent<EGE::STD::TERMINAL::WINDOWS::Position>(i);
                    snakePiecePosition = snakePieceComponentPosition -> getFirstPosition();
                    snakePiecePositionTupleTmp.x = std::get<0>(*snakePiecePosition);
                    snakePiecePositionTupleTmp.y = std::get<1>(*snakePiecePosition);
                    this -> reset.positionResetSprite(i,snake,snakePiecePositionTuple.x,snakePiecePositionTuple.y);
                    snakePiecePositionTuple.x = snakePiecePositionTupleTmp.x;
                    snakePiecePositionTuple.y = snakePiecePositionTupleTmp.y;


                }
                
                this -> view.viewSnake(snake);
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
