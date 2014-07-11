#include "cglib.h"


Point newPoint(float x, float y) {
	Point point;
	point.x = x;
	point.y = y;
	return point;
}

Point negative(Point point) {
	Point negativePoint;
	negativePoint.x = -(point.x);
	negativePoint.y = -(point.y);
	return negativePoint;
}

Polyline newPolyline(int pointCount, int closed) {
	Polyline polyline;
	polyline.points = (Point*) malloc(pointCount * sizeof(Point));
	polyline.pointCount = pointCount;
	polyline.closed = closed;
	return polyline;
}

Polyline newRegularPolygon(int sides, Point center, int radius) {
	Polyline polygon = newPolyline(sides, true);
	float theta = (2 * M_PI) / sides;
	int i;
	for (i = 0; i < sides; i++) {
		polygon.points[i].x = center.x + radius * cos(i * theta);
		polygon.points[i].y = center.y + radius * sin(i * theta);
	}
	return polygon;
}

Polyline newRectangle(Point p1, Point p2) {
	Polyline rectangle = newPolyline(4, true);
	rectangle.points[0] = p1;
	rectangle.points[1].x = p2.x;
	rectangle.points[1].y = p1.y;
	rectangle.points[2] = p2;
	rectangle.points[3].x = p1.x;
	rectangle.points[3].y = p2.y;
	return rectangle;
}

void rotateCCW(Polyline* polyline, int degrees, Point origin) {
	double theta = (degrees * M_PI) / 180; // converting degrees into radians
	move(polyline, negative(origin));
	float x, y;
	int i;
	for (i = 0; i < polyline->pointCount; i++) {
		x = polyline->points[i].x;
		y = polyline->points[i].y;
		polyline->points[i].x = (x * cos(theta) - y * sin(theta));
		polyline->points[i].y = (x * sin(theta) + y * cos(theta));
	}
	move(polyline, origin);
}

void rotateCW(Polyline* polyline, int degrees, Point origin) {
	rotateCCW(polyline, -degrees, origin);
}

void move(Polyline* polyline, Point delta) {
	int i;
	for (i = 0; i < polyline->pointCount; i++) {
		polyline->points[i].x += delta.x;
		polyline->points[i].y += delta.y;
	}
}

void scale(Polyline* polyline, float scaleFactor, Point origin) {
	move(polyline, negative(origin));
	int i;
	for (i = 0; i < polyline->pointCount; i++) {
		polyline->points[i].x *= scaleFactor;
		polyline->points[i].y *= scaleFactor;
	}
	move(polyline, origin);
}

Polyline clone(Polyline original) {
	Polyline copy;
	copy.closed = original.closed;
	copy.pointCount = original.pointCount;
	copy.points = (Point*)malloc(copy.pointCount * sizeof(Point));
	int i;
	for (i = 0; i < copy.pointCount; i++) {
		copy.points[i] = original.points[i];
	}
	return copy;
}

Point centerOf(Polyline polyline) {
	Point center = newPoint(0, 0);
	int i;
	for (i = 0; i < polyline.pointCount; i++) {
		center.x += polyline.points[i].x;
		center.y += polyline.points[i].y;
	}
	center.x /= (float) polyline.pointCount;
	center.y /= (float) polyline.pointCount;
	return center;
}

void freePolyline(Polyline polyline) {
	free(polyline.points);
}

ViewPort* createViewPort(Point pMin, Point pMax, Universe* u) {
	ViewPort* newViewPort;

	newViewPort = (ViewPort*) malloc(sizeof(ViewPort));

	if (newViewPort != NULL) {
		newViewPort->pMin = pMin;
		newViewPort->pMax = pMax;
		newViewPort->universe = u;
		newViewPort->pMinSRD = sru2srn(pMin, pMin, pMax);
		newViewPort->pMaxSRD = sru2srn(pMax, pMin, pMax);

	}

	return newViewPort;
}

char allocateViewPortBuffer(ViewPort* vp, int screenMaxWidth,
		int screenMaxHeight) {
	vp->pMinSRD = srn2srd(vp->pMinSRD, screenMaxWidth, screenMaxHeight);
	vp->pMaxSRD = srn2srd(vp->pMaxSRD, screenMaxWidth, screenMaxHeight);

	vp->width = (int) vp->pMaxSRD.x - vp->pMinSRD.x;
	vp->height = (int) vp->pMaxSRD.y - vp->pMinSRD.y;

	vp->buffer = (int*) calloc(vp->width * vp->height, sizeof(int));

	if (vp->buffer == NULL) {
		return 0;
	}

	return 1;
}

Point sru2srn(Point pSRU, Point pMin, Point pMax) {
	Point pSRN;
	pSRN.x = (pSRU.x - pMin.x) / (pMax.x - pMin.x);
	pSRN.y = (pSRU.y - pMin.y) / (pMax.y - pMin.y);

	return pSRN;
}

Point srn2srd(Point pSRN, int width, int height) {
	Point pSRD;

	pSRD.x = round(pSRN.x * (width));
	pSRD.y = round(pSRN.y * (height));

	return pSRD;
}

int insideViewPort(ViewPort* vp, Point* p) {
	return p->y >= vp->pMinSRD.y && p->y <= vp->pMaxSRD.y
			&& p->x >= vp->pMinSRD.x && p->x <= vp->pMaxSRD.x;
}

void drawPoint(Point* p, ViewPort* viewPort, int color) {
	int x, y;
	x = (int) p->x - viewPort->pMinSRD.x;
	y = viewPort->height - (int) p->y - viewPort->pMinSRD.y;
	// printf("%d\n", y * viewPort->width + x);
	viewPort->buffer[y * viewPort->width + x] = color; // TODO fix this line for p1(-100, 100) and p2(100, -100)
}

int readPoint(Point* p, ViewPort* viewPort) {
	int x, y;
	x = (int) p->x - viewPort->pMinSRD.x;
	y = viewPort->height - (int) p->y - viewPort->pMinSRD.y;
	// printf("%d\n", y * viewPort->width + x);
	return viewPort->buffer[y * viewPort->width + x]; // TODO fix this line for p1(-100, 100) and p2(100, -100)
}

void drawLine(Line line, ViewPort* viewPort, int color) {
	int yMin, yMax, i, xMin, xMax, height, width;
	float m, a, b, x, y;
	Point p;

	height = viewPort->height;
	width = viewPort->width;
	yMin = (line.p1.y < line.p2.y) ? (int) line.p1.y : (int) line.p2.y;
	yMax = (line.p1.y < line.p2.y) ? (int) line.p2.y : (int) line.p1.y;

	if (line.p1.x == line.p2.x) {
		if (line.p1.x >= viewPort->pMinSRD.x
				&& line.p1.x <= viewPort->pMaxSRD.x) {

			p.x = line.p1.x;

			for (i = yMin; i <= yMax; i++) {
				p.y = i;
				if (insideViewPort(viewPort, &p)) {
					drawPoint(&p, viewPort, color);
				}
				// if(i >= viewPort->pMinSRD.y && i <= viewPort->pMaxSRD.y){				
				// 	viewPort->buffer[i*width + (int) line.p1.x] = color;
				// }
			}
		}
	} else {
		//terminar alterações no algoritimo

		xMin = (line.p1.x < line.p2.x) ? (int) line.p1.x : (int) line.p2.x;
		xMax = (line.p1.x < line.p2.x) ? (int) line.p2.x : (int) line.p1.x;

		float dx, dy;
		dy = (line.p2.y - line.p1.y);
		dx = (line.p2.x - line.p1.x);

		a = dy / dx;

		if (-1 <= a && a <= 1) {
			b = line.p1.y - a * line.p1.x;

			x = xMin;
			y = a * x + b;

			while (x <= xMax) {
				p.y = (int) y;
				p.x = x;
				if (insideViewPort(viewPort, &p)) {
					drawPoint(&p, viewPort, color);
					// viewPort->buffer[(height - (int) y) * width + (int) x] = color;
				}
				y = a * (++x) + b;
			}
		} else {
			a = 1 / a;
			b = line.p1.x - a * line.p1.y;

			y = yMin;
			x = a * y + b;

			/* x = xMin;
			y = a * x + b; */

			while (y <= yMax) {
				p.x = (int) x;
				p.y = y;
				if (insideViewPort(viewPort, &p)) {
					drawPoint(&p, viewPort, color);
					// viewPort->buffer[(height - (int) y) * width + (int) x] = color;
				}
				x = a * (++y) + b;
			}
		}

	}
}

void drawPolyLine(Polyline polyline, ViewPort* viewPort, int color) {
	Line line;
	int i = 0;
	while (i < polyline.pointCount - 1) {
		line.p1 = polyline.points[i++];
		line.p2 = polyline.points[i];
		drawLine(line, viewPort, color);
	}
	if (polyline.closed) {
		line.p1 = polyline.points[0];
		drawLine(line, viewPort, color);
	}
}

void fill2(Point* point, ViewPort* viewPort, int color, int bgColor) {
	// printf("BG Color: %d\n", bgColor);
	if (readPoint(point, viewPort) == bgColor) {
		drawPoint(point, viewPort, color);


		Point p = newPoint(point->x + 1.0, point->y);
		fill2(&p, viewPort, color, bgColor);

		p = newPoint(point->x, point->y + 1.0);
		fill2(&p, viewPort, color, bgColor);

		p = newPoint(point->x - 1.0, point->y);
		fill2(&p, viewPort, color, bgColor);

		p = newPoint(point->x, point->y - 1.0);
		fill2(&p, viewPort, color, bgColor);

		// fill2(newPoint(point->x - 1, point->y - 1), viewPort, color, bgColor);
		// fill2(newPoint(point->x - 1, point->y + 1), viewPort, color, bgColor);
		// fill2(newPoint(point->x + 1, point->y - 1), viewPort, color, bgColor);
		// fill2(newPoint(point->x + 1, point->y + 1), viewPort, color, bgColor);
	}
}

void fill(Point* seed, ViewPort* viewPort, int color) {
	fill2(seed, viewPort, color, readPoint(seed, viewPort));

	/* Line line;
	int i = 0;
	while (i < polyline.pointCount - 1) {
		line.p1 = polyline.points[i++];
		line.p2 = polyline.points[i];
		drawLine(line, viewPort, color);
	}
	if (polyline.closed) {
		line.p1 = polyline.points[0];
		drawLine(line, viewPort, color);
	} */
}

void drawLineMidPoint(Line line, ViewPort* viewPort, int color) {
	printf("P1: %f, %f\nP2: %f %f\n", line.p1.x, line.p1.y, line.p2.x, line.p2.y);
	int x, y, i, sinal;
	float m, e, dx, dy;
	Point p;

	x = (int) line.p1.x;
	y = (int) line.p1.y;

	dx = (line.p2.x - line.p1.x);
	dy = (line.p2.y - line.p1.y);

	if (dx == 0) {
		int y1, y2;
		if (line.p1.y <= line.p2.y) {
			y1 = (int) line.p1.y;
			y2 = (int) line.p2.y;
		} else {
			y1 = (int) line.p2.y;
			y2 = (int) line.p1.y;
		}

		p.x = (int) line.p1.x;
		for (p.y = y1; p.y <= y2; p.y++) {
			if (insideViewPort(viewPort, &p)) {
				drawPoint(&p, viewPort, color);
			}
		}
	} else {
		m = dy / dx;
		e = m - 0.5;

		sinal = 1;
		if (m < 0) {
			sinal = -1;
			m = -m;
			e = m - 0.5;
		}

		if (m <= 1) {
			if (dx < 0) {
				x = (int) line.p2.x;
				y = (int) line.p2.y;
				dx = -dx;
			}

			// printf("%f\n", dx);
			for (i = 0; i <= dx; i++) {
				p.x = x;
				p.y = y;
				if (insideViewPort(viewPort, &p)) {
					drawPoint(&p, viewPort, color);
				}
				while (e >= 0) {
					y += sinal;
					e--;
				}
				x++;
				e = e + m;
			}
		} else {
			m = 1/m;
			if (dy < 0) {
				x = (int) line.p2.x;
				y = (int) line.p2.y;
				dy = -dy;
			}

			// printf("%f\n", dy);
			for (i = 0; i <= dy; i++) {
				p.x = x;
				p.y = y;
				if (insideViewPort(viewPort, &p)) {
					drawPoint(&p, viewPort, color);
				}
				while (e >= 0) {
					x += sinal;
					e--;
				}
				y++;
				e = e + m;
			}
		}
	}
}

void show(ViewPort* viewPort, ColorMap* colorMap) {
#define EventMask (KeyPressMask | ExposureMask)
	Display* display;
	XImage* ximage;
	Window window;
	XEvent an_event;
	GC gc;
	Visual* visual;
	XGCValues values;
	int m, n, screen, dplanes;
	int ret = 1;

	if ((display = XOpenDisplay(NULL)) == NULL)
		ret = 0;
	else {
		screen = DefaultScreen(display);
		dplanes = DisplayPlanes(display, screen);
		visual = XDefaultVisual(display, screen);

		if (!(window = XCreateSimpleWindow(display, RootWindow(display, screen),
				0, 0, viewPort->width, viewPort->height, 1,
				BlackPixel(display, screen), WhitePixel(display, screen))))
			ret = 0;
		else {
			XSelectInput(display, window, EventMask);
			XStoreName(display, window, "Monitor Virtual");
			gc = XCreateGC(display, window, 0, &values);

			XMapWindow(display, window);
			XSync(display, False);

			ximage = XCreateImage(display, visual, dplanes, ZPixmap, 0,
					malloc(viewPort->width * viewPort->height * sizeof(int)),
					viewPort->width, viewPort->height, 8, 0);

			for (m = 0; m < viewPort->height; m++) {
				for (n = 0; n < viewPort->width; n++) {

					Color* c;

					c = getColor(colorMap,
							viewPort->buffer[m * viewPort->width + n]);
					if (c == NULL) {
						printf(
								"[ERRO] nao foi possivel achar a cor no mapa de cores\n");
						return;
					}
					ximage->data[(m * 4) * viewPort->width + n * 4 + 0] =
							c->blue;
					ximage->data[(m * 4) * viewPort->width + n * 4 + 1] =
							c->green;
					ximage->data[(m * 4) * viewPort->width + n * 4 + 2] =
							c->red;
					ximage->data[(m * 4) * viewPort->width + n * 4 + 3] =
							c->alpha;
				}
			}

			XPutImage(display, window, gc, ximage, 0, 0, 0, 0, viewPort->width,
					viewPort->height);

			/* Trata os eventos */
			while (1) {
				XNextEvent(display, &an_event);
				switch (an_event.type) {
				case Expose:
					XPutImage(display, window, gc, ximage, 0, 0, 0, 0,
							viewPort->width, viewPort->height);
					break;
					/* outros eventos ... */
				}
			}

		}
	}

}

/*
 Entradas:
 	m1: primeira matriz
 	m2: segunda matriz

 Saída:
 	m1 = m1*m2
*/
void mult(float m1[3][3], float m2[3][3]){
	float aux[3][3];
	int i,j;

	for(i = 0; i < 3; i++){
	 	for(j = 0; j < 3; j++){
	 		aux[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j];
	 	}
	} 
	for(i = 0; i < 3; i++){
	 	for(j = 0; j < 3; j++){
	 		m1[i][j] = aux[i][j];
	 	}
	} 
}

/*
	Entrada:
		om: uma matriz 3x3
	Saída:
		om fica inicializada com a diagonal principal com 1
*/
void initOperationMatrix(float om[3][3]){
	int i,j;

	for(i = 0; i < 3; i++){
	 	for(j = 0; j < 3; j++){
	 		if(i == j){
	 			om[i][j] = 1;
	 		}else{
	 			om[i][j] = 0;
	 		}
	 	}
	} 
}

/*
	Adiciona a om uma rotação em sentindo anti-horario

	Entrada:
		om: operation matrix
		degrees: angulo de rotação
*/
void addRotationCCW(float om[3][3], float degrees){
	double theta = (degrees * M_PI) / 180; // converting degrees into radians
	float rotacao[3][3] = { 
					  {cos(theta), -sin(theta), 0},
					  {sin(theta), cos(theta),  0}, 
					  {0,		   0, 			1}
					};
	
	mult(om, rotacao);
}

/*
	Aplica a matriz de operações em um poligono

	Entrada:
		om: operation matrix
		polygon: poligono onde sera aplicada a transformação
*/
void applyOM(float om[3][3], Polyline *polygon){
	int i;
	float x,y;

	for(i = 0; i < polygon->pointCount; i++ ){
		x = polygon->points[i].x * om[0][0] + polygon->points[i].y * om[0][1] + om[0][2];
		y = polygon->points[i].x * om[1][0] + polygon->points[i].y * om[1][1] + om[1][2];	
		printf("(%f , %f)\n",x,y);
		polygon->points[i].x = x;
		polygon->points[i].y = y;	
	}
}