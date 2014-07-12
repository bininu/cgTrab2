/*
Renan Lordello de Aguiar RA 489867

RGB é chamado de sistema aditivo pois ele para gerar uma cor
ele soma o valor de suas componentes
 */
#include "cglib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
	ViewPort *vp;
	// Line line;
	int i, j;
	Point pMin, pMax;
	Universe u;
	ColorMap* cm;
	

	pMin.x = -20;
	pMin.y = -20;
	pMax.x = 10;
	pMax.y = 15;
	// pMin.x = -100;
	// pMin.y = -100;
	// pMax.x = 100;
	// pMax.y = 100;


	u.pMin = pMin;
	u.pMax = pMax;

	vp = createViewPort(newPoint(-2.0, -2.0), newPoint(2, 2), &u);
	// vp = createViewPort(pMin, pMax, &u);
	if (vp == NULL) {
		printf("Erro de alocacao do veiwport\n");
		exit(0);
	}

	if (!allocateViewPortBuffer(vp, MAXIMOX, MAXIMOY)) {
		printf("Erro de alocacao do buffer do viewport\n");
		exit(0);
	}

	cm = createColorMap();
	if (cm == NULL) {
		printf("[ERRO] mapa de cores nao pode ser criado\n");
		exit(0);
	}

	if (!addColor(cm, 0, 255, 255, 255, 0)) {
		printf("[ERRO] cor '0' nao pode ser inserida no mapa de cores\n");
		exit(0);
	}

	if (!addColor(cm, 1, 0, 0, 0, 0)) {
		printf("[ERRO] cor '1' nao pode ser inserida no mapa de cores\n");
		exit(0);
	}

	if (!addColor(cm, 2, 0,0,255,0)) {
		printf("[ERRO] cor '2' nao pode ser inserida no mapa de cores\n");
		exit(0);
	}

	if (!addColor(cm, 3, 255,0,0,0)) {
		printf("[ERRO] cor '3' nao pode ser inserida no mapa de cores\n");
		exit(0);
	}

	if (!addColor(cm, 4, 255, 255, 0, 0)) {
		printf("[ERRO] cor '4' nao pode ser inserida no mapa de cores\n");
		exit(0);
	}


	int polylineCount = 3;
	Polyline polylines[polylineCount];

	polylines[0] = newRectangle(newPoint(-6, -6), newPoint(-1, -2));
	polylines[1] = newPolyline(5, true);
	polylines[1].points[0] = newPoint(-7.0,-3.0);
	polylines[1].points[1] = newPoint(-4.0,-4.0);
	polylines[1].points[2] = newPoint(-3.0,-6.0);
	polylines[1].points[3] = newPoint(-6.0,-9.0);
	polylines[1].points[4] = newPoint(-9.0,-8.0);
	polylines[2] = clone(polylines[1]);

	
	Point bc = negative(barycenter(polylines, polylineCount));

	float om[3][3];
	initOperationMatrix(om);
	addScale(om, 0.25, 0.25);
	addTranslation(om, bc.x, bc.y);
	applyOM(om,&polylines[0]);
	applyOM(om,&polylines[1]);
	applyOM(om,&polylines[2]);
	

	initOperationMatrix(om);
	// bc = negative(centerOf(polylines[2]));
	bc = barycenter(&polylines[2], 1);

	addTranslation(om, bc.x, bc.y);
	addRotationCCW(om, 45.0);
	bc = negative(bc);
	addTranslation(om, bc.x, bc.y);
	applyOM(om,&polylines[2]);

	for (j = 0; j < polylineCount; j++) {
		for (i = 0; i < polylines[j].pointCount; i++) {
			polylines[j].points[i] = sru2srn(polylines[j].points[i], vp->pMin, vp->pMax);
		}
		for (i = 0; i < polylines[j].pointCount; i++) {
			polylines[j].points[i] = srn2srd(polylines[j].points[i], MAXIMOX, MAXIMOY);
		}	
	}
	drawPolyLine(polylines[0], vp, 2);
	drawPolyLine(polylines[1], vp, 3);
	drawPolyLine(polylines[2], vp, 1);


	// bc = srn2srd(sru2srn(bc, vp->pMin, vp->pMax), MAXIMOX, MAXIMOY);
	// drawPoint(&bc, vp, 1);
	// bc = srn2srd(sru2srn(newPoint(1.0, 1.0), vp->pMin, vp->pMax), MAXIMOX, MAXIMOY);
	// drawPoint(&bc, vp, 1);



	show(vp, cm);

	return 0;
}
