/* Codigo de SPACE BREAKERS, hecho por Juan Carlos Morales Velez.
	Proyecto final para Introduccion a la Programacion de segundo semestre, UABC.
	Creado:				  5/09/2020.
	Ultima actualizacion: 5/24/2020. 

		SPACE BREAKERS: juego tipo "Asteroids" con disenos inspirados en "Metroid". Consiste en una nave 
	con un laser controlado por el usuario, inicialmente aparece dentro de la safezone, cierto numero 
	de "metroides" generados aleatoriamente por nivel y un numero de "anillos de poder" que la nave 
	puede absorber.
		
		El objetivo del jugador es destruir a todos los metroides que aparecen cada nivel usando su laser y recolectar 
	los anillos, al destruir un metroide suma 1 punto, al absorber un anillo le aumenta el rango y velocidad del laser, 
	y suma 2 puntos, pierde una vida al chocar con un metroide. Al pasar de nivel se obtiene una vida y aumentan 
	las habilidades de la nave (velocidad y tiempo entre disparos) y aumenta la resistencia de los metroides. 
	Si se pasa el nivel y se han recolectado todos los anillos se obtiene una vida extra y se aumenta la velocidad 
	de la nave. Hay un total de 6 niveles.*/

#include <stdlib.h>
#include <time.h>
#include "miniwin.h"
#include <sstream>
#include <math.h>
#include <list>

using namespace miniwin;
using namespace std;

//Estructura para caracteristicas generales del juego.
struct general{
	int largo =		1200;
	int altura = 	750;
	int nivel = 	1; //Nivel inicial
	int vjuego =	10; //Velocidad de frames.
	int numAst =    0;
};

//Estructura para caracteristicas de la nave.
struct nave{
	int x = 	  50; //Posicion inicial.
	int y = 	  90;
	int r = 	  27; //Radio del area de la nave.
	int v = 	  10; //Velocidad inicial.
	int color =	  4;
	int sentido = 1;
	int vlaseri = 60; //Retardo entre disparos inicial.
	int vlaser =  vlaseri; //Variable para usar de retardo en main.
	int vidas =   3; //Vidas iniciales.
	int score =   0; //Cuenta los asteroides destruidos por el jugador.
	int anillos = 0; //Numero de anillos que ha absorbido.
	int d = 	  0; //Rango extra del laser.
};

//Estructura para caracteristicas de los asteroides.
struct asteroide{
	int x;
	int y;
	int v;
	int sentido;
	int radio;
	int res; //Resistencia a colisiones con laser.
};

//Estructura para caracteristicas del laser.
struct laser{
	int xi, x; //Coordenadas iniciales y coordenadas de movimiento.
	int yi, y; //Coordenadas iniciales y coordenadas de movimiento.
	int v; //Velocidad
	int s; //Sentido
	int dmax; //Distancia maxima que avanza el laser.
};

//Estructura para caracteristicas de anillos.
struct anillo{
	int x;
	int y;
	int r = 10; //Radio general
};

//Prototipos para las funciones.
void imprimeControles();
void imprimePantalla(struct general, struct nave);
void imprimeStats(struct general, struct nave);
void controlTeclas(struct nave *, list<struct laser*> &listLsr);
void genlistLsr(list<struct laser*> &listLsr, int x, int y, int s, int v, int d);
void generalistAst(list<struct asteroide*> &listAst, int numAst);
void generalistAni(list<struct anillo*> &listAni, int numAni);
int imprimeObjetos(list<struct asteroide*> listAst, list<struct laser*> listLsr, list<struct anillo*> listAni, struct nave);
void mueveObjetos(list<struct laser*> &listLsr, list<struct asteroide*> listAst, list<struct anillo*> &listAni);
void colisiones(list<struct asteroide*> &listAst, list<struct laser*> &listLsr, list<struct anillo*> &listAni, struct nave *Jugador);
void detectaEventos(struct general *, struct nave *, list<struct anillo*> &listAni);

//Funcion principal del juego.
int main(){ 
	struct general Juego;
	struct nave Jugador;
	struct asteroide Ast;
	struct laser Lsr;
	struct anillo Ani;
	list<struct asteroide*> listAst;
	list<struct laser*> listLsr;
	list<struct anillo*> listAni;
	vredimensiona(Juego.largo,Juego.altura);
	
	imprimeControles();
	
	do{ //Ciclo para generar caracteristicas de cada nivel.
		borra();
		
		imprimePantalla(Juego,Jugador); //Imprime el nivel y stats de la nave.
		generalistAst(listAst,2*Juego.nivel+1);
		generalistAni(listAni,Juego.nivel);
				
		do{ //Ciclo principal del juego.
			borra();
		
			Juego.numAst=imprimeObjetos(listAst, listLsr, listAni, Jugador); //Actualiza el numero de metroides para mostrarlo en Stats.
			imprimeStats(Juego,Jugador);
			controlTeclas(&Jugador, listLsr);
			mueveObjetos(listLsr, listAst, listAni);
			colisiones(listAst, listLsr, listAni, &Jugador);
			
			//Retardo del disparo.
			Jugador.vlaser-=2;
			refresca();
			espera(Juego.vjuego);
		}while(Jugador.vidas > 0 && Juego.numAst > 0 );
		
		//Detecta si pasa de nivel.
		detectaEventos(&Juego, &Jugador, listAni);
		
		//Detecta si muere, se acaba el juego.
		if(Jugador.vidas <= 0){
			imprimePantalla(Juego, Jugador); 
			break;}
		
	}while(Juego.nivel <= 6);
	
	//Imprime la pantalla final si se pasa el juego.
	imprimePantalla(Juego,Jugador);
	
	return 0;
}

//Funcion que imprime las estadisticas del juego.
void imprimeStats(struct general Juego, struct nave Jugador){
	
	srand(time(NULL));
	stringstream txtStream;
	
	//Imprime stats en la esquina superior izquierda.
	color(AZUL);
	circulo(0,0,160); //Marca la safezone.
	color(MAGENTA);
	txtStream.str("");
	txtStream << "NIVEL: " << Juego.nivel << "   VIDAS: " << Jugador.vidas;
	texto(20, 10, txtStream.str());
	txtStream.str("");
	txtStream << "METROIDES: " << Juego.numAst;
	texto(20, 30, txtStream.str());
	txtStream.str("");
	txtStream << "SCORE: " << Jugador.score;
	texto(20, 50, txtStream.str());
	
}

//Funcion que imprime texto y stats en pantalla negra.
void imprimePantalla(struct general Juego, struct nave Jugador){
	
	stringstream txtStream;
	
	espera(300);
	
	//Imprime las estadisticas del jeugo y la nave si la nave tiene vidas y aun no se ha pasado el ultimo nivel.
	if(Jugador.vidas > 0 && Juego.nivel <= 6){
		borra();
		color(CYAN);
		txtStream.str("");
		txtStream << "NIVEL--->" << Juego.nivel; 
		texto(550,305, txtStream.str());
		color(BLANCO);
		txtStream.str("");
		txtStream << "VIDAS--->" << Jugador.vidas;
		texto(520,325, txtStream.str());
		txtStream.str("");
		txtStream << "VELOCIDAD--->" << Jugador.v;
		texto(520,345, txtStream.str());
		txtStream.str("");
		txtStream << "DISPARO--->" << 60-Jugador.vlaseri; 
		texto(520,365, txtStream.str());
		txtStream.str("");
		txtStream << "SCORE TOTAL--->" << Jugador.score; 
		texto(520,385, txtStream.str());
		refresca();
		espera(3000);
	}
	
	//Imprime que has muerto si la nave ya no tiene vidas.
	if(Jugador.vidas <= 0){
		borra();
		color(ROJO);
		txtStream.str("");
		txtStream << "YOU ARE DEAD!" ; 
		texto(510,305, txtStream.str());
		color(BLANCO);
		txtStream.str("");
		txtStream << "VIDAS--->" << Jugador.vidas;
		texto(520,345, txtStream.str());
		txtStream.str("");
		txtStream << "SCORE TOTAL--->" << Jugador.score; 
		texto(520,365, txtStream.str());
		refresca();
		espera(3000);
	}
	
	//Imprime felicitaciones si se ha pasado el ultimo nivel.
	if(Juego.nivel > 6){
		borra();
		color(MAGENTA);
		txtStream.str("");
		txtStream << "CONGRATULATIONS!" ; 
		texto(520,305, txtStream.str());
		color(BLANCO);
		txtStream.str("");
		txtStream << "VIDAS--->" << Jugador.vidas;
		texto(520,345, txtStream.str());
		txtStream.str("");
		txtStream << "SCORE TOTAL--->" << Jugador.score; 
		texto(520,365, txtStream.str());
		refresca();
		espera(3000);
	}
	
}

//Funcion de control con las teclas y genera listas de laser.
void controlTeclas(struct nave *Jugador, list<struct laser*> &listLsr){
	
	int d;
	int t = tecla();
	
	d=sqrt(pow((*Jugador).x,2) + pow((*Jugador).y,2)); //Distancia de la nave a la safezone.
	
	switch(t){
		case 'W': (*Jugador).sentido = 4; //Arriba.
					if((*Jugador).y-(*Jugador).r>0){
						(*Jugador).y -= (*Jugador).v; 
					} break;
		case 'S': (*Jugador).sentido = 3; //Abajo.
					if((*Jugador).y+(*Jugador).r<750){
						(*Jugador).y += (*Jugador).v; 
					} break;
		case 'A': (*Jugador).sentido = 2; //Izquierda.
					if((*Jugador).x-(*Jugador).r>0){
						(*Jugador).x -= (*Jugador).v; 
					} break;
		case 'D': (*Jugador).sentido = 1; //Derecaha.
					if((*Jugador).x+(*Jugador).r<=1200){
						(*Jugador).x += (*Jugador).v; 
					} break;
		case 'L': if((*Jugador).vlaser<=0 && (d >= 150 + (*Jugador).r)){//Detecta si no esta en la safezone.
						//Genera lista de lasers a partir de las coordenadas del jugador.
						genlistLsr(listLsr, (*Jugador).x, (*Jugador).y, (*Jugador).sentido, (*Jugador).v, (*Jugador).d); 
						(*Jugador).vlaser=(*Jugador).vlaseri;}
						break; 
		case 'P': espera(5000); break; //Pausa por 5 seg.
	}
}

//Funcion que imprime la nave, los laseres y metroides, y regresa el numero de metroides que se han impreso.
int imprimeObjetos(list<struct asteroide*> listAst, list<struct laser*> listLsr, list<struct anillo*> listAni, struct nave Jugador){
	
	int numAst=0;
	int x,y; //Contador del numero de asteroides que se imprimen por frame.
	list<struct laser*>::iterator i1;
	list<struct asteroide*>::iterator i2;
	list<struct anillo*>::iterator i3;
	
	//Imprime estrellas en posiciones aleatorias.
	for(int i=1;i<=60;i++){
		x=rand()%1199+1;
		y=rand()%749+1;
		color(BLANCO);
		punto(x,y);
	}
	
	//Imprime la figura de cada laser en la lista.
	for(i1=listLsr.begin();i1!=listLsr.end();i1++){
		color(CYAN);
		rectangulo_lleno((*i1)->x-4,(*i1)->y-4,(*i1)->x+4,(*i1)->y+4);
		
	}
	
	//Imprime la figura de cada metroide en la lista.
	for(i2=listAst.begin();i2!=listAst.end();i2++){
		color(VERDE);
		circulo_lleno((*i2)->x,(*i2)->y,(*i2)->radio);
		color(BLANCO);
		circulo_lleno((*i2)->x+2*(*i2)->radio/5,(*i2)->y+3*(*i2)->radio/5,2*(*i2)->radio/5);
		circulo_lleno((*i2)->x-2*(*i2)->radio/5,(*i2)->y+3*(*i2)->radio/5,2*(*i2)->radio/5);
		color(ROJO);
		circulo_lleno((*i2)->x,(*i2)->y-(*i2)->radio/4,(*i2)->radio/3);
		circulo_lleno((*i2)->x-(*i2)->radio/3,(*i2)->y-(*i2)->radio/6,(*i2)->radio/5);
		circulo_lleno((*i2)->x+(*i2)->radio/3,(*i2)->y-(*i2)->radio/6,(*i2)->radio/5);
		circulo_lleno((*i2)->x-(*i2)->radio/4,(*i2)->y-(*i2)->radio/2,(*i2)->radio/6);
		circulo_lleno((*i2)->x+(*i2)->radio/4,(*i2)->y-(*i2)->radio/2,(*i2)->radio/6);
		color(NEGRO);
		circulo((*i2)->x,(*i2)->y-(*i2)->radio/4,(*i2)->radio/3);
		circulo((*i2)->x-2*(*i2)->radio/5,(*i2)->y+3*(*i2)->radio/5,2*(*i2)->radio/5);
		circulo((*i2)->x+2*(*i2)->radio/5,(*i2)->y+3*(*i2)->radio/5,2*(*i2)->radio/5);
		circulo_lleno((*i2)->x,(*i2)->y+2*(*i2)->radio/3,(*i2)->radio/2);
		
		numAst++; 
	}
	
	//Imprime la nave segun su sentido.
	switch(Jugador.sentido){
		case 1: color(Jugador.color); //Derecha.
				circulo_lleno(Jugador.x+7,Jugador.y,11);
				circulo_lleno(Jugador.x+18,Jugador.y,6);
				color(Jugador.color);
				circulo_lleno(Jugador.x+6,Jugador.y-11,5);
				circulo_lleno(Jugador.x+6,Jugador.y+11,5);
				color(VERDE);
				circulo_lleno(Jugador.x+3,Jugador.y,10);
				color(NEGRO);
				circulo(Jugador.x+3,Jugador.y,10);
				color(Jugador.color);
				rectangulo_lleno(Jugador.x-8,Jugador.y-16,Jugador.x+5,Jugador.y+16);
				color(ROJO);
				linea(Jugador.x-21,Jugador.y,Jugador.x-8,Jugador.y);
				linea(Jugador.x-20,Jugador.y+2,Jugador.x-8,Jugador.y+2);
				linea(Jugador.x-18,Jugador.y+4,Jugador.x-8,Jugador.y+4);
				linea(Jugador.x-16,Jugador.y+6,Jugador.x-8,Jugador.y+6);
				linea(Jugador.x-14,Jugador.y+8,Jugador.x-8,Jugador.y+8);
				linea(Jugador.x-20,Jugador.y-2,Jugador.x-8,Jugador.y-2);
				linea(Jugador.x-18,Jugador.y-4,Jugador.x-8,Jugador.y-4);
				linea(Jugador.x-16,Jugador.y-6,Jugador.x-8,Jugador.y-6);
				linea(Jugador.x-14,Jugador.y-8,Jugador.x-8,Jugador.y-8);
				 break;
		case 2: color(Jugador.color); //Izquierda.
				circulo_lleno(Jugador.x-7,Jugador.y,11);
				circulo_lleno(Jugador.x-18,Jugador.y,6);
				circulo_lleno(Jugador.x-6,Jugador.y+11,5);
				circulo_lleno(Jugador.x-6,Jugador.y-11,5);
				color(VERDE);
				circulo_lleno(Jugador.x-3,Jugador.y,10);
				color(NEGRO);
				circulo(Jugador.x-3,Jugador.y,10);
				color(Jugador.color);
				rectangulo_lleno(Jugador.x+8,Jugador.y+16,Jugador.x-5,Jugador.y-16);
				color(ROJO);
				linea(Jugador.x+8,Jugador.y,Jugador.x+21,Jugador.y);
				linea(Jugador.x+8,Jugador.y+2,Jugador.x+20,Jugador.y+2);
				linea(Jugador.x+8,Jugador.y+4,Jugador.x+18,Jugador.y+4);
				linea(Jugador.x+8,Jugador.y+6,Jugador.x+16,Jugador.y+6);
				linea(Jugador.x+8,Jugador.y+8,Jugador.x+14,Jugador.y+8);
				linea(Jugador.x+8,Jugador.y-2,Jugador.x+20,Jugador.y-2);
				linea(Jugador.x+8,Jugador.y-4,Jugador.x+18,Jugador.y-4);
				linea(Jugador.x+8,Jugador.y-6,Jugador.x+16,Jugador.y-6);
				linea(Jugador.x+8,Jugador.y-8,Jugador.x+14,Jugador.y-8);
				 break;
		case 3: color(Jugador.color); //Abajo.
				circulo_lleno(Jugador.x,Jugador.y+7,11);
				circulo_lleno(Jugador.x,Jugador.y+18,6);
				circulo_lleno(Jugador.x-11,Jugador.y+6,5);
				circulo_lleno(Jugador.x+11,Jugador.y+6,5);	
				color(VERDE);
				circulo_lleno(Jugador.x,Jugador.y+3,10);
				color(NEGRO);
				circulo(Jugador.x,Jugador.y+3,10);
				color(Jugador.color);
				rectangulo_lleno(Jugador.x-16,Jugador.y-8,Jugador.x+16,Jugador.y+5);
				color(ROJO);
				linea(Jugador.x,Jugador.y-21,Jugador.x,Jugador.y-8);
				linea(Jugador.x+2,Jugador.y-20,Jugador.x+2,Jugador.y-8);
				linea(Jugador.x+4,Jugador.y-18,Jugador.x+4,Jugador.y-8);
				linea(Jugador.x+6,Jugador.y-16,Jugador.x+6,Jugador.y-8);
				linea(Jugador.x+8,Jugador.y-14,Jugador.x+8,Jugador.y-8);
				linea(Jugador.x-2,Jugador.y-20,Jugador.x-2,Jugador.y-8);
				linea(Jugador.x-4,Jugador.y-18,Jugador.x-4,Jugador.y-8);
				linea(Jugador.x-6,Jugador.y-16,Jugador.x-6,Jugador.y-8);
				linea(Jugador.x-8,Jugador.y-14,Jugador.x-8,Jugador.y-8);
				 break;
		case 4: color(Jugador.color); //Arriba.
				circulo_lleno(Jugador.x,Jugador.y-7,11);
				circulo_lleno(Jugador.x,Jugador.y-18,6);
				circulo_lleno(Jugador.x+11,Jugador.y-6,5);
				circulo_lleno(Jugador.x-11,Jugador.y-6,5);
				color(VERDE);
				circulo_lleno(Jugador.x,Jugador.y-3,10);
				color(NEGRO);
				circulo(Jugador.x,Jugador.y-3,10);
				color(Jugador.color);
				rectangulo_lleno(Jugador.x+16,Jugador.y+8,Jugador.x-16,Jugador.y-5);
				color(ROJO);
				linea(Jugador.x,Jugador.y+8,Jugador.x,Jugador.y+21);
				linea(Jugador.x+2,Jugador.y+8,Jugador.x+2,Jugador.y+20);
				linea(Jugador.x+4,Jugador.y+8,Jugador.x+4,Jugador.y+18);
				linea(Jugador.x+6,Jugador.y+8,Jugador.x+6,Jugador.y+16);
				linea(Jugador.x+8,Jugador.y+8,Jugador.x+8,Jugador.y+14);
				linea(Jugador.x-2,Jugador.y+8,Jugador.x-2,Jugador.y+20);
				linea(Jugador.x-4,Jugador.y+8,Jugador.x-4,Jugador.y+18);
				linea(Jugador.x-6,Jugador.y+8,Jugador.x-6,Jugador.y+16);
				linea(Jugador.x-8,Jugador.y+8,Jugador.x-8,Jugador.y+14);
				 break;	 
	}
	
	//Imprime los anillos.
	for(i3=listAni.begin();i3!=listAni.end();i3++){
		color(MAGENTA);
		circulo((*i3)->x,(*i3)->y,(*i3)->r);
		circulo((*i3)->x,(*i3)->y,(*i3)->r+4);
		circulo((*i3)->x,(*i3)->y,(*i3)->r+7);
		color(CYAN);
		circulo((*i3)->x,(*i3)->y,(*i3)->r+2);
		circulo((*i3)->x,(*i3)->y,(*i3)->r+6);
	}
	
	return numAst;
}

//Funcion que va cambiando coordenadas de los objetos de lista segun su sentido de movimiento.
void mueveObjetos(list<struct laser*> &listLsr, list<struct asteroide*> listAst, list<struct anillo*> &listAni){
	
	int d;
	list<struct laser*>::iterator i1;
	list<struct asteroide*>::iterator i2;
	list<struct anillo*>::iterator i3;
	
	//Mueve cada laser de la lista en relacion con la coordenada  y sentido inicial, y los elimina al tocar los bordes de la ventana.
	for(i1=listLsr.begin();i1!=listLsr.end();i1++){
		
		switch((*i1)->s){//Condicionales para eliminar el laser despues de dmax y el lim de la ventana.
			case 1: if((*i1)->x <= 1200 && ((*i1)->x <= (*i1)->xi+(*i1)->dmax)){
						(*i1)->x += (*i1)->v;
						break;}
					else{
						delete (*i1);
						i1=listLsr.erase(i1);
						break;}
			case 2: if((*i1)->x>=0 && ((*i1)->x >= (*i1)->xi-(*i1)->dmax)){
						(*i1)->x -= (*i1)->v;
						break;} 
					else{
						delete (*i1);
						i1=listLsr.erase(i1);
						break;}
			case 3: if((*i1)->y<=750 && ((*i1)->y <= (*i1)->yi+(*i1)->dmax)){
						(*i1)->y += (*i1)->v;
						break;}
					else{
						delete (*i1);
						i1=listLsr.erase(i1);
						break;}
			case 4: if((*i1)->y>=0 && ((*i1)->y >= (*i1)->yi-(*i1)->dmax)){
						(*i1)->y -= (*i1)->v;
						break;}
					else{
						delete (*i1);
						i1=listLsr.erase(i1);
						break;}
		}
	}
	
	//Mueve los metroides segun su sentido y "rebotan" en los bordes.
	for(i2=listAst.begin();i2!=listAst.end();i2++){
		d=sqrt(pow((*i2)->x,2) + pow((*i2)->y,2));
		switch((*i2)->sentido){
			case 1: (*i2)->x+=(*i2)->v; (*i2)->y+=(*i2)->v; //Abajo-derecha.
					if((*i2)->x>=1200-(*i2)->radio){
						(*i2)->sentido=2;
					}
					if((*i2)->y>=750-(*i2)->radio){
						(*i2)->sentido=3;
					} break;
			case 2: (*i2)->x-=(*i2)->v; (*i2)->y+=(*i2)->v; //Abajo-izquierda.
					if((*i2)->x<=0+(*i2)->radio){
						(*i2)->sentido=1;
					}
					if((*i2)->y>=750-(*i2)->radio){
						(*i2)->sentido=4;
					} 
					if(d <= 160 + (*i2)->radio){
						(*i2)->sentido=1;
					} break; 
			case 3: (*i2)->x+=(*i2)->v; (*i2)->y-=(*i2)->v; //Arriba-derecha.
					if((*i2)->x>=1200-(*i2)->radio){
						(*i2)->sentido=4;
					}
					if((*i2)->y<=0+(*i2)->radio){
						(*i2)->sentido=1;
					}
					if(d <= 160 + (*i2)->radio){
						(*i2)->sentido=1;
					} break; 
			case 4: (*i2)->x-=(*i2)->v; (*i2)->y-=(*i2)->v; //Arriba-izquierda
					if((*i2)->x<=0+(*i2)->radio){
						(*i2)->sentido=3;
					}
					if((*i2)->y<=0+(*i2)->radio){
						(*i2)->sentido=2;
					} 
					if(d <= 160 + (*i2)->radio){
						(*i2)->sentido=1;
					} break; 		
		}
	}
	
	//Animacion del anillo.
	for(i3=listAni.begin();i3!=listAni.end();i3++){
		(*i3)->r--;
		if((*i3)->r<=2){
			(*i3)->r=10;
		}
	}
}

//Funcion que genera la lista de laseres segun la posicion hy sentido de la nave.
void genlistLsr(list<struct laser*> &listLsr, int x, int y, int s, int v, int d){
	
	struct laser *Lsr;
	
	//Crea un objeto de la lista por cada vez que se manda a llamar la funcion (detecta la tecla L).
	Lsr = 			new struct laser;
	Lsr->xi = 		x;
	Lsr->yi = 		y;
	Lsr->s =		s;
	Lsr->v = 		v+3;
	Lsr->x =		Lsr->xi;
	Lsr->y =		Lsr->yi;
	Lsr->dmax =		200+d;
	
	listLsr.push_back(Lsr);
}

//Funcion que genera la lista de asteroides cada nivel con caracteristicas aleatorias.
void generalistAst(list<struct asteroide*> &listAst, int numAst){
	
	struct asteroide *Ast;
	
	//Crea un numero de asteroides segun el nivel (se calcula en el main).
	for(int i=1;i<=numAst;i++){
		
		Ast = 		   	new struct asteroide;
		Ast->x = 		rand()%1020+180;
		Ast->y =		rand()%600+150;
		Ast->radio = 	rand()%(numAst+18)+12; //Dependiente del numAst para que aumente la dificultad con el nivel.
		Ast->v = 		rand()%(numAst/2)+1;   //Dependiente del numAst para que aumente la dificultad con el nivel.
		Ast->sentido =	rand()%4+1;
		Ast->res = 		rand()%(numAst/2+1)+1; //Dependiente del numAst para que aumente la dificultad con el nivel.
		
		listAst.push_back(Ast);
	}
}

//Funcion que genera la lista de anillos por nivel.
void generalistAni(list<struct anillo*> &listAni, int numAni){
	
	srand(time(NULL));
	struct anillo *Ani;
	
	//Crea un numero de anillos cada nivel, igual al numero del nivel.
	for(int i=1;i<=numAni;i++){
		Ani = 		new struct anillo;
		Ani->x = 		rand()%1000+160;
		Ani->y =		rand()%580+160;	
		
		listAni.push_back(Ani);
	}
}

//Funcion que detecta colisiones entre los objetos y la nave segun la distancia entre los puntos medios.
void colisiones(list<struct asteroide*> &listAst, list<struct laser*> &listLsr, list<struct anillo*> &listAni, struct nave *Jugador){
	int aux, d, d2, d3, d4;
	list<struct asteroide*>::iterator i1; 
	list<struct laser*>::iterator i3;
	list<struct anillo*>::iterator i4;
	
	//Recorrer la lista de metroides. 
	for(i1=listAst.begin();i1!=listAst.end();i1++){
		
		list<struct asteroide*>::iterator i2=i1;
		advance(i2,1);
		
		//Compara cada metroide *i1 con el resto de de metroides *i2 de la lista.
		for(;i2!=listAst.end();i2++){
			
			d=sqrt(pow((*i2)->x - (*i1)->x,2)+pow((*i2)->y - (*i1)->y,2)); //Distancia metroide-metroide.
			
			//Intercambia los sentidos si han "colisionado".
			if(d<=(*i2)->radio+(*i1)->radio){
				aux=(*i2)->sentido;
				(*i2)->sentido=(*i1)->sentido;
				(*i1)->sentido=aux;
			}
		}
		
		
		d2=sqrt(pow((*i1)->x - (*Jugador).x,2)+pow((*i1)->y - (*Jugador).y,2)); //Distancia metroide-nave.
		
		//Resta vidas y posiciona la nave si d2 indica que ha colisionado con el metroide.
		if(d2<=(*i1)->radio + (*Jugador).r - 10){
			espera(200);
			(*Jugador).vidas--;
			(*Jugador).x=80;
			(*Jugador).y=90;
			(*Jugador).sentido=1;
			espera(200);
		}
		
		
		//Compara cada metroide *i1 con cada laser *i3 de la lista.
		for(i3=listLsr.begin();i3!=listLsr.end();i3++){
			
			d3=sqrt(pow((*i1)->x - (*i3)->x, 2) + pow((*i1)->y - (*i3)->y, 2)); //Distancia metroide-laser.
			
			//Resta resistencia al metroide si d3 indica "colision".
			if(d3<=(*i1)->radio+5){
				delete (*i3);
				i3=listLsr.erase(i3);
				(*i1)->res--; 
				
				//Elimina el metroide si tiene cero resistencia y aumenta el score de la nave.
				if((*i1)->res<=0){
					delete (*i1);
					i1=listAst.erase(i1);
					(*Jugador).score++; 
				}
			}
		}
		
	}
	
	//Detecta si el jugador absorbe un anillo.
	for(i4=listAni.begin();i4!=listAni.end();i4++){
		
		d4=sqrt(pow((*i4)->x - (*Jugador).x,2)+pow((*i4)->y - (*Jugador).y,2));
		//Aumenta score, vidas y rango del laser.
		if(d4 <= 15 + (*Jugador).r){
			delete (*i4);
			i4=listAni.erase(i4);
			(*Jugador).score+=2;
			(*Jugador).anillos++;
			(*Jugador).d+=18;
			(*Jugador).vlaseri--;
		}
	}
}

//Funcion que sube de nivel.
void detectaEventos(struct general *Juego, struct nave *Jugador, list<struct anillo*> &listAni){
		
	list<struct anillo*>::iterator i;	
	
	//Pasa de nivel si se acaban los metroides.
	if((*Juego).numAst <= 0){
		espera(300);
		//Detecta sii ha recogido todos los anillos por nivel.
		if((*Jugador).anillos==(*Juego).nivel){
			(*Jugador).v++;
			(*Jugador).vidas++;
		}
		if((*Jugador).anillos!=(*Juego).nivel){
			//Borra los anillos restantes
			for(i=listAni.begin();i!=listAni.end();i++){
				delete (*i);
				i=listAni.erase(i);
			}
		}
		//Rewards por subir de nivel.
		(*Juego).nivel++;
		(*Jugador).vidas++;
		(*Jugador).vlaseri-=5;
		(*Jugador).v++;
		(*Jugador).x=50;
		(*Jugador).y=90;
		(*Jugador).sentido=1;
		espera(300);
	}
	
	
}

//Funcion que muestra los controles del juego.
void imprimeControles(){
	
	stringstream txtStream;
	
		borra();
		color(MAGENTA);
		txtStream.str("");
		txtStream << "SPACE BREAKERS"; 
		texto(515,305, txtStream.str());
		color(CYAN);
		txtStream.str("");
		txtStream << "CONTROLES:";
		texto(525,325, txtStream.str());
		color(BLANCO);
		txtStream.str("");
		txtStream << "W----->ARRIBA" ;
		texto(522,345, txtStream.str());
		txtStream.str("");
		txtStream << "A----->IZQUIERDA" ; 
		texto(522,365, txtStream.str());
		txtStream.str("");
		txtStream << "S----->ABAJO" ; 
		texto(522,385, txtStream.str());
		txtStream.str("");
		txtStream << "D----->DERECHA" ; 
		texto(522,405, txtStream.str());
		txtStream.str("");
		txtStream << "L----->DISPARA" ; 
		texto(522,425, txtStream.str());
		txtStream.str("");
		txtStream << "P----->PAUSA" ; 
		texto(522,445, txtStream.str());
		refresca();
		espera(3000);
		
}
