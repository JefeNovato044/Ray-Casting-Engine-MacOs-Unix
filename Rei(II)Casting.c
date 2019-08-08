//Motor de renderizado Raycasting por Jefe Novato 044, basado en la idea del vídeo de javidx.

#include<ncurses.h>
#include<stdlib.h>
#include<math.h>



//Prototipos
void InitNcurses();
void Controles(char tecla,float *PosX, float *PosY,float *AnguloJugador,float Velocidad, float VelocidadRotacion);


int AlturaPantalla, AnchoPantalla;
int AnchoMapa = 16;
int AlturaMapa = 16;


FILE *archivo;

#define PI 3.1416f

#define CIELO 1
#define PARED 2
#define PISO  3


char mapa[] =  {
"################\
#..............#\
#..............#\
#...#......#...#\
#...#......#...#\
#####......#...#\
#......##......#\
#..............#\
#..............#\
#..............#\
#..............#\
#..............#\
#..............#\
#..............#\
#..............#\
################"};

int main () {
    InitNcurses();
    float PosX = 7.0f;
    float PosY = 8.0f;
    float AnguloJugador = 2.0f;
    float FOV = 3.1416f/2.0f; //90 grados en radianes
    float Profundidad = 15.0f;//Distancia de Dibujado
    float Velocidad = 1.0f;
    float VelocidadRotacion = 2.0f;
    int salir = 1;
     int tecla = 0;
    
    while (tecla != 27) {
        for (int i =0; i < AnchoPantalla; i++){//Raycast
            //El rayo inicia en 0 y Va sumando el porcentaje del fov hasta que se haya escaneado todo el fov
            float anguloRayo = (AnguloJugador-FOV/2.0f) + ((float)i/(float)AnchoPantalla)*FOV;
            float DistanciaPared = 0.0f;
            float VecX = cosf(anguloRayo);//Descomponer vector del rayo en vectores "x" y "y".
            float VecY = sinf(anguloRayo);
            int ColisonPared = 0;
            
            while (ColisonPared == 0 && DistanciaPared < Profundidad){
                //Crecen hasta que choca con una pared
                int RayoX = (int) (PosX + VecX*DistanciaPared);
                int RayoY = (int) (PosY + VecY*DistanciaPared);
                
                //Verificar su el rayo esta fuera de los bordes
                if ( (RayoX < 0||RayoX >= AnchoPantalla)||(RayoY < 0||RayoY >= AlturaPantalla) ){
                    DistanciaPared = Profundidad;
                    ColisonPared = 1;
                } else if (mapa[(int)RayoY*AnchoMapa + RayoX] == '#'){
                    ColisonPared = 1;
                }
                DistanciaPared += 0.1f;
            }
                DistanciaPared *= cosf(FOV/2.0f); //Correcion de la distorcion
            
            //Calcular distancia a paredes y suelo
            int Pared = (float)(AlturaPantalla/2.0f) - AlturaPantalla/((float)DistanciaPared);
            int Suelo = AlturaPantalla - Pared;
          
           
            for (int j=0; j<AlturaPantalla; j++){
                if (j<=Pared){//Es techo
                    attron(COLOR_PAIR(CIELO));
                    move(j,i);// j->y i->x
                    addch(' ');
                    attroff(COLOR_PAIR(CIELO));
                } else if (j>Pared && j<= Suelo){//Es Pared
                    move(j,i);// j->y i->x
                    
                        attron(COLOR_PAIR(PARED));
                    //1/5 de la distancia del rayo
                    if (DistanciaPared <= Profundidad*0.2f){
                        addch('=');
                    }
                    else if (DistanciaPared <= Profundidad*0.4f){
                        addch('_');
                    } else if (DistanciaPared <= Profundidad*0.8f){
                        addch('.');
                    }else {
                        addch(' ');
                    }
                     attroff(COLOR_PAIR(PARED));
                } else{//Es suelo
                    attron(COLOR_PAIR(PISO));
                    move(j,i);// j->y i->x
                    addch(' ');
                    attroff(COLOR_PAIR(PISO));
                }
            }
           
        }
   
        for (int x=0; x<16; x++) {
            for (int y=0; y<16; y++) {
                move(y,x);
                printw("%c",mapa[y*AnchoMapa + x]);
                move((int)PosY,(int)PosX);
                addch('F');
            }
            
        }
         refresh();
     
        move (0,40);
        printw("%.3fº",AnguloJugador*180/PI);
        
       
        tecla = getch();
        Controles(tecla,&PosX,&PosY,&AnguloJugador,Velocidad,VelocidadRotacion);
        
       
        
    }
    endwin();
    return 0;
        
    }
    

void InitNcurses(){
    initscr();
    getmaxyx(stdscr, AlturaPantalla, AnchoPantalla);
    noecho();
    cbreak();
    curs_set(0);//Visibilidad del cursor
    keypad(stdscr, TRUE);
  
    start_color();
    init_color(COLOR_RED, 500, 0, 0);

    init_pair(CIELO, COLOR_CYAN, COLOR_CYAN);
    init_pair(PARED,COLOR_RED,COLOR_WHITE);
    init_pair(PISO,COLOR_BLACK,COLOR_GREEN);
}




void Controles(char tecla,float *PosX, float *PosY,float *AnguloJugador,float Velocidad, float VelocidadRotacion){
    
    switch(tecla)//Movimiento y coliciones
    {
        case 'W':
        case 'w':
            *PosX =  *PosX + cosf(*AnguloJugador) * Velocidad;
            *PosY = *PosY + sinf(*AnguloJugador) * Velocidad;
            
            if(mapa[(int)(*PosY)*AnchoMapa + (int)(*PosX)] == '#')
            {
                *PosX = *PosX - cosf(*AnguloJugador) * Velocidad;
                *PosY = *PosY - sinf(*AnguloJugador) * Velocidad;
            }
            
            break;
            
        case 'S':
        case 's':
            *PosX -= cosf(*AnguloJugador) * Velocidad;
            *PosY -= sinf(*AnguloJugador) * Velocidad;
            
            if(mapa[(int)(*PosY)*AnchoMapa + (int)(*PosX)] == '#')
            {
                *PosX = *PosX + cosf(*AnguloJugador) * Velocidad;
                *PosY = *PosY + sinf(*AnguloJugador) * Velocidad;
            }
            
            break;
            
        case 'J':
        case 'j':
            *AnguloJugador = *AnguloJugador - 0.1f*VelocidadRotacion;
            break;
            
        case 'L':
        case 'l':
            *AnguloJugador = *AnguloJugador + 0.1f*VelocidadRotacion;
            break;
            
        case 'D':
        case 'd':
            *PosX =  *PosX + cosf(*AnguloJugador + 1.57079f) * Velocidad;//90º+Angulo
            *PosY =  *PosY + sinf(*AnguloJugador + 1.57079f) * Velocidad;//Vector Unitario del vector normal
            
            if(mapa[(int)(*PosY)*AnchoMapa + (int)(*PosX)] == '#')
            {
                //90º+Angulo
                *PosX = *PosX - cosf(*AnguloJugador + 1.57079f) * Velocidad;
                *PosY = *PosY - sinf(*AnguloJugador + 1.57079f) * Velocidad;
            }
            
            break;
            
        case 'A':
        case 'a':
            *PosX = *PosX - cosf(*AnguloJugador + 1.57079f) * Velocidad;//90º-Angulo
            *PosY = *PosY - sinf(*AnguloJugador + 1.57079f) * Velocidad;
            
            if(mapa[(int)(*PosY)*AnchoMapa + (int)(*PosX)] == '#')
            {
                *PosX = *PosX + cosf(*AnguloJugador + 1.57079f) * Velocidad;//90º-Angulo
                *PosY = *PosY + sinf(*AnguloJugador + 1.57079f) * Velocidad;
            }
            
            break;
    }
}
