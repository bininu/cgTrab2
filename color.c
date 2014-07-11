#include "color.h"
Color* getColor(ColorMap* colorMap, int id) {
	int i;
	ColorMapEntry* aux;

	for (aux = (ColorMapEntry*) colorMap->colors; aux != NULL; aux = (ColorMapEntry*) aux->next) {
		if (aux->id == id) {
			return aux->color;
		}
	}

	return NULL;
}

ColorMap* createColorMap() {
	ColorMap* cm;

	cm = (ColorMap*) malloc(sizeof(ColorMap));
	if (cm == NULL) {
		return NULL;
	}

	cm->colors = NULL;
	cm->endColors = NULL;
	return cm;
}

int addColor(ColorMap* cm, int id, unsigned char red, unsigned char green,
		unsigned char blue, unsigned char alpha) {
	ColorMapEntry* newColor;

	newColor = (ColorMapEntry*) malloc(sizeof(ColorMapEntry));
	if (newColor == NULL) {
		return 0;
	}

	newColor->color = (Color*) malloc(sizeof(Color));
	if (newColor->color == NULL) {
		return 0;
	}

	newColor->color->red = red;
	newColor->color->green = green;
	newColor->color->blue = blue;
	newColor->color->alpha = alpha;

	newColor->id = id;
	newColor->next = NULL;

	if (cm->endColors != NULL) {
		cm->endColors->next = (struct ColorMapEntry*) newColor;
	}

	if (cm->colors == NULL) {
		cm->colors = newColor;
	}

	cm->endColors = newColor;
	return 1;
}


/*
	Dados os parâmetros da cor em HSV, retorna
	uma com em RGB equivalente
*/
Color* HSV2RGB(float h, float s, float v){
	Color* RGBcolor;
	float hi,f,p,q,t;

	RGBcolor = (Color*) malloc(sizeof(Color));
	if (RGBcolor == NULL) {
		return NULL;
	}

	hi = ((int)floor(h / 60.0)) % 6;
	f  = h / 60.0 - hi;
	p  = v * (1 - s);
	q  = v * (1 - f*s);
	t  = v * (1 - (1 - f) * s);

	switch((int)hi){
		case 0:
			RGBcolor->red   = round(v * 255);
			RGBcolor->green = round(t * 255);
			RGBcolor->blue  = round(p * 255);
			break;

		case 1:
			RGBcolor->red   = round(q * 255);
			RGBcolor->green = round(v * 255);
			RGBcolor->blue  = round(p * 255);
			break;
		
		case 2:
			RGBcolor->red   = round(p * 255);
			RGBcolor->green = round(v * 255);
			RGBcolor->blue  = round(t * 255);
			break;
		
		case 3:
			RGBcolor->red   = round(p * 255);
			RGBcolor->green = round(q * 255);
			RGBcolor->blue  = round(v * 255);
			break;
		
		case 4:
			RGBcolor->red   = round(t * 255);
			RGBcolor->green = round(p * 255);
			RGBcolor->blue  = round(v * 255);
			break;
		
		case 5:
			RGBcolor->red   = round(v * 255);
			RGBcolor->green = round(p * 255);
			RGBcolor->blue  = round(q * 255);
			break;
			
	}
	RGBcolor->alpha = 0;
	return RGBcolor;
}

/*
	Adiciona um cor no sistema HSV ao mapa de cores
*/
int addColorHSV(ColorMap* cm, int id, float h, float s, float v){
	ColorMapEntry* newColor;

	newColor = (ColorMapEntry*) malloc(sizeof(ColorMapEntry));
	if (newColor == NULL) {
		return 0;
	}

	newColor->color = HSV2RGB(h, s, v);
	if (newColor->color == NULL) {
		return 0;
	}

	newColor->id = id;
	newColor->next = NULL;

	if (cm->endColors != NULL) {
		cm->endColors->next = (struct ColorMapEntry*) newColor;
	}

	if (cm->colors == NULL) {
		cm->colors = newColor;
	}

	cm->endColors = newColor;
	return 1;
}
