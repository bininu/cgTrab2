#include <stdlib.h>
#include <math.h>

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
} Color;

typedef struct {
	Color* color;
	int id;
	struct ColorMapEntry* next;
} ColorMapEntry;

typedef struct {
	ColorMapEntry* colors;
	ColorMapEntry* endColors;
} ColorMap;

/*
Iniciliza um mapa de cor 
*/
ColorMap* createColorMap();

/*
	Dado um mapa de cor (colorMap) e o identificador, 
	retorna a cor correspondente, se a mesmo existir,
	retorna NULL, caso contrário.
*/
Color* getColor(ColorMap* colorMap, int id);

/*
	Adiciona um cor no sistema RGB ao mapa de cores 
*/
int addColor(ColorMap* cm, int id, unsigned char red, unsigned char green,
		unsigned char blue, unsigned char alpha);

/*
	Dados os parâmetros da cor em HSV, retorna
	uma com em RGB equivalente
*/
Color* HSV2RGB(float h, float s, float v);

/*
	Adiciona um cor no sistema HSV ao mapa de cores
*/
int addColorHSV(ColorMap* cm, int id, float h, float s, float v);