//Eric La
#ifndef DRAW_H
#define DRAW_H
#include <FL/glut.H>
#include<iostream>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <armadillo>
using namespace std;
using namespace arma;
static unsigned long inCount = 0;

//Point class: represents a point in 3D space
class Point {
	public:
	unsigned long id;
	Point(GLfloat x, GLfloat y, GLfloat z);
	Point();
	GLfloat x, y, z;
	GLfloat vec[3];
	Point& operator=(const Point& other);
	bool operator==(const Point& other);
	bool equal(const Point& other);
};

//Holds values for a curve
class Curve {
	public:
	list<Point> ctlPts;
	list<Curve> subCurves;
	int numPts, nCurvePts;
	float tVal;
	bool active;
	bool changed;
	//unsigned int dFlags;//
	Curve(int numPts, list<Point> ctlPts);
	Curve();
	~Curve();
	void binomialCoeffs (GLint n, GLint* C);
	void computeBezPt(GLfloat u, Point* bezPt, GLint nCtrlPts, list<Point> ctrlPts, GLint* C);
	void bezier(list<Point> ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);
	void draw();
	Point* insertAt(Point a);
	vector<Point> intersect(Curve& other);
	//void draw(int options);
	void subDivide(float t);
	void drawDivide();
	void drawControlLines();
	void drawControlPoly();
	void drawT();
	void drawBezier();
	void drawCastel();
	void drawProcess();
	void drawPoints();
	void deletePoint(Point* p);
	void degreeRaise();
	void degreeLower();
	void setT(float y);
	Point aitkens(float t);
	void drawAit();
	float* findBB();
	list<Point> intersection(Curve* other, double e);
	Point castel(float t);
	Curve& operator=(const Curve& other);
};

void plotPoint(Point pt);
#endif
