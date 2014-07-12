#include "color.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   
#include <X11/Xlib.h>
#define true 1
#define false 0

#define MAXIMOX 640
#define MAXIMOY 480

typedef struct {
	float x;
	float y;
} Point;

typedef struct {
	Point p1;
	Point p2;
} Line;

typedef struct {
	Point* points;
	int pointCount;
	int closed;
} Polyline;

Point newPoint(float x, float y);
Point negative(Point point);
Polyline newPolyline(int pointCount, int closed);
Polyline newRegularPolygon(int sides, Point center, int radius);
Polyline newRectangle(Point p1, Point p2);

void rotateCCW(Polyline* polyline, int degrees, Point origin);
void rotateCW(Polyline* polyline, int degrees, Point origin);
void move(Polyline* polyline, Point delta);
void scale(Polyline* polyline, float scaleFactor, Point origin);

Polyline clone(Polyline original);
Point centerOf(Polyline polyline);
void freePolyline(Polyline polyline);

typedef struct {
	Point pMin;
	Point pMax;
} Universe;

typedef struct {
	int *buffer;
	int height;
	int width;
	Point pMin;
	Point pMax;
	Point pMinSRD;
	Point pMaxSRD;
	Universe* universe;
} ViewPort;

/*
 Entradas:
 pMin: menor ponto do viewport
 pMax: maior ponto do viewport
 u: Universo do viewport

 Saída:
 um ponteiro para ViewPort, mas esse ViewPort não tem um buffer allocado e
 os campos height e width não preenchidos.
 Em caso de falha é retornado um ponteiro nulo
 */
ViewPort* createViewPort(Point pMin, Point pMax, Universe* u);

/*
 Entrads:
 vp: O ViewPort no qual sera alocado o buffer e preenchido os campos
 height and width
 screenMaxWidth: largura do dispositivo
 screenMaxHeight: altura do dispositivo

 Saída:
 retorna 0 em caso de falha de alocação; 1 caso contrário
 */
char allocateViewPortBuffer(ViewPort* vp, int screenMaxWidth,
		int screenMaxHeight);

/*
 Entradas:
 pSru: ponto a ser convertido
 pMin: ponto inferior esquerdo do limite do SRU
 pMax: ponto superior direito do limite do SRU

 Saída:
 o ponto convertido para o SRN

 */
Point sru2srn(Point pSRU, Point pMin, Point pMax);

/*
 Entradas:
 pSRN: ponto a ser convertido
 width: largura do ViewPort
 height: altura do ViewPort


 Saída:
 o ponto convertido para o SRD
 */
Point srn2srd(Point pSRN, int width, int height);

void drawPolyLine(Polyline polyline, ViewPort* viewPort, int color);
void fill(Point* seed, ViewPort* viewPort, int color);

/*
 Entradas:
 line: linha a ser desenhada, a mesma deve estar no SRD
 viewPort: matriz aonde sera desenhada a linha
 width: largura do viewPort
 height: altura do viewPort

 */
void drawLine(Line line, ViewPort* viewPort, int color);
// void drawLine(Line line, ViewPort* viewPort, int width, int height);

/*
 Desenha uma linha usando o algoritimo do ponto médio

 Entradas:
 line: linha a ser desenhada, a mesma deve estar no SRD
 viewPort: matriz aonde sera desenhada a linha
 width: largura do viewPort
 height: altura do viewPort
 */
void drawLineMidPoint(Line line, ViewPort* viewPort, int color);

/*
 Entradas:
 p: ponto a ser desenhado
 viewPort: matriz aonde sera desenhado o ponto
 color: cor do ponto
 */
void drawPoint(Point* p, ViewPort* viewPort, int color);

/*
 Entradas:
 viewPort: imagem a ser mostrada
 width: largura do viewPort
 height: altura do viewPort
 */
void show(ViewPort* viewPort, ColorMap* colorMap);

/*
 Entradas:
 	m1: primeira matriz
 	m2: segunda matriz

 Saída:
 	m1 = m1*m2
*/
void mult(float m1[3][3], float m2[3][3]);

/*
	Entrada:
		om: uma matriz 3x3
	Saída:
		om fica inicializada com a diagonal principal com 1
*/
void initOperationMatrix(float om[3][3]);

/*
	Adiciona a om uma rotação

	Entrada:
		om: operation matrix
		degrees: angulo de rotação
*/
void addRotationCCW(float om[3][3], float degrees);


/*
	Aplica a matriz de operações em um poligono

	Entrada:
		om: operation matrix
		polygon: poligono onde sera aplicada a transformação
*/
void applyOM(float om[3][3], Polyline *polygon);

/*
	Adiciona a om uma translação 

	Entrada:
		om: operation matrix
		dx: variação no x
		dy: variação no y
*/
void addTranslation(float om[3][3], float dx, float dy);


/*
	Acha o baricentro de um comjunto de poligonos

	Saída:
		o baricentro
*/
Point barycenter(Polyline* vetor, int number);


/*
	Adiciona a om uma operação de escala

	Entrada:
		om: operation matrix
		dx: variação no x
		dy: variação no y
*/
void addScale(float om[3][3], float Sx, float Sy);