//Eric La
#include<FL/Fl.h>
#include<FL/Fl_Window.h>
#include<FL/Fl_Button.h>
#include<FL/Fl_Float_Input.h>
#include<FL/Fl_Value_Slider.h>
#include<Fl/Fl_Scroll.h>
#include<Fl/Fl_Check_Button.h>
#include<FL/glut.H>
#include "draw.h"
#include<iostream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<list>
#include<vector>
#include<ctime>
#include<ratio>
#include<chrono>
using namespace std;

GLsizei winWidth = 800, winHeight = 800;

GLfloat xwcMin = -400.0 , xwcMax = 400.0;
GLfloat ywcMin = -400.0 , ywcMax = 400.0;

int MainWindow;
Fl_Window* mainWin;
list<Point> pixels;
list<Point*> selection;
list<Point> tempPix;
list<Curve> curves;
int px, py;//current mouse coords
int xs, ys;
float tVal = 0;
int tol = 5;
bool drag = false;
double error = 0.0001;


Fl_Button* buttons[20];
Fl_Button* modes[10];
Fl_Check_Button* params[20];
Fl_Float_Input* textfields[10];
Fl_Value_Slider* slides[10];
Fl_Scroll* scrolls[10];
Fl_Group* groups[10];
vector<Fl_Check_Button*> checks(100);

int nBox = 0;
enum {SELECT, SET, MOVE, UPDATE}; 
enum {POINTS, CURVES, LINES, POLY, DIVIDES, TSLIDE};
bool pars[6] = {1,1,1,0,0,1};
int mode = SELECT;
void updateBoxes();
void compare();
void mode_cb(Fl_Widget* o, void*);
void move();

void clearAll(){
	pixels.clear();
	curves.clear();
	selection.clear();
	tempPix.clear();
	updateBoxes();
}

void but_cb(Fl_Widget* o, void*){
  Fl_Button* b=(Fl_Button*)o;
  if(b == buttons[0]){
	if(tempPix.size() > 1){
		curves.push_back(Curve(tempPix.size(), tempPix));
		updateBoxes();
		curves.back().tVal = slides[0]->value();//initialize t value
		tempPix.clear();
	}
	else{
		cout << "Add more points (at least 2) to create a curve\n";
	}
  }

  if(b == buttons[1]){
	clearAll();
  }

  if(b == buttons[2]){
	if(tempPix.size() > 0){
		tempPix.pop_back();
	}else{
	   if(curves.size() > 0){
		tempPix.splice(tempPix.end(),curves.back().ctlPts);
		curves.pop_back();
		scrolls[0]->remove(checks[--nBox]);
		scrolls[0]->redraw();	
	   }//undo curve
	}
  }

  if(b== buttons[3]){
	compare();
	for(list<Curve>::iterator it = curves.begin(); it != curves.end(); ++it){
		if(it->active){
			it->degreeRaise();
		}
}
	
  }

  if(b == buttons[4]){
	exit(0);
  }

  if(b == buttons[7]){
	Point* temp;
	if(selection.size() < 1)
		cout << "Please select a point on curve\n";
	for(list<Curve>::iterator it = curves.begin(); it != curves.end(); ++it){
		if(it->active){
			temp = it->insertAt(*selection.front());
			if(temp->id != (unsigned long)-1){
				selection.clear();
				selection.push_back(temp);
				modes[MOVE]->value(true);
				for(int i = 0; i < 3; ++i){
					if(i != MOVE){
						modes[i]->value(false);
					}
				}
				mode = MOVE;
			}
			else{
				cout << "Please select a point on curve\n";
				delete temp;
			}
		}
	}
  }

  if(b == buttons[8]){
	if(selection.size() == 0)
		cout << "Nothing selected\n";
	for(list<Curve>::iterator it = curves.begin(); it != curves.end(); ++it){
		it->deletePoint(selection.front());
	}

	if(selection.size() > 0)
	for(list<Point>::iterator it = tempPix.begin(); it != tempPix.end(); ++it){
		if(it->equal(*selection.front())){//it->id == selection.front()->id){
			tempPix.erase(it);
			break;
			glutPostRedisplay();
		}
	}
	selection.clear();
  }

  glutPostRedisplay();
}

void mode_cb(Fl_Widget* o, void*){
	Fl_Light_Button* a = (Fl_Light_Button*)o;
	bool allFalse = true;
	for(int i = 0; i < 3; ++i){
		if(modes[i] != a){
			modes[i]->value(false);
		}else{
			if(mode == MOVE){
				selection.clear();
			}
			allFalse = false;
			mode = i;
		}
	}
	if(allFalse){
		modes[SELECT]->value(1);
		mode = SELECT;
	}
}

void algo_cb(Fl_Widget *o, void*){
	Fl_Button* a = (Fl_Button*)o;
	if(buttons[6] == a){
	for(list<Curve>::iterator it = curves.begin(); it != curves.end(); ++it){
		if(it->active){
			it->degreeRaise();
		}
}
	
  }
	if(buttons[5] == a){
		for(list<Curve>::iterator it = curves.begin(); it != curves.end(); ++it){
		    if(it->active){
			    it->degreeLower();
		    }
        }	
    }
}

void text_cb(Fl_Widget* o, void*){
}

void tValue_Slider_cb(Fl_Widget* o, void*){//set T value for the curve
	Fl_Value_Slider* slider = (Fl_Value_Slider*)o;
	if(slider == slides[0]){
		tVal = slider->value();
		for(list<Curve>::iterator ptr = curves.begin(); ptr != curves.end(); ++ptr){
			ptr->tVal = slider->value();
		}
	}
	if(slider == slides[1]){
		error = slider->value();
	}
	glutPostRedisplay();
}

void check_cb(Fl_Widget* o, void*){
	Fl_Check_Button* b = (Fl_Check_Button*)o;
	glutPostRedisplay();
}

void param_cb(Fl_Widget* o, void*){
	Fl_Check_Button* b = (Fl_Check_Button*)o;
	if(b == params[POINTS]) pars[POINTS] = b->value();
	if(b == params[CURVES]) pars[CURVES] = b->value();
	if(b == params[LINES]) pars[LINES] = b->value();
	if(b == params[POLY]) pars[POLY] = b->value();
	if(b == params[DIVIDES]) pars[DIVIDES] = b->value();
	if(b == params[TSLIDE]) pars[TSLIDE] = b->value();
	glutPostRedisplay();	
}

int guiInit(){
  Fl_Window win(300,200,"Draw Window");
  win.begin();
    buttons[0] = new Fl_Button(10,150,70,30, "Graph");
    textfields[0] = new Fl_Float_Input(100,150,70,30, "X");
    //values[0] = new Fl_Valuator(10,100,200,30, "T");
  win.end();
  buttons[0]->callback(but_cb);
  //textfields[0]->when(FL_WHEN_CHANGED);
  //textfields[0]->callback(text_cb);
  win.show();
  return Fl::run();
}

void init (){
        glClearColor(1,1,1,0);
        glClear (GL_COLOR_BUFFER_BIT);
}

void setPixels(){
	list<Point>::iterator p = tempPix.begin();
	list<Point*>::iterator s = selection.begin();

	glColor3f(0,0,0);
	glPointSize(13);/*
	for(;s != selection.end(); ++s){
		plotPoint(*(*s));
	}*/
	if(selection.size() > 0)
	plotPoint(*selection.front());

	glColor3f(1,0,0);
	glPointSize(8);
	for(;p != tempPix.end(); ++p){
		plotPoint(*p);
	}

	/*glPointSize(5);
	glColor3f(1,0,0);
	for(p = ctrl.begin() ;p != ctrl.end(); ++p){
		plotPoint(*p);
	}*/

	/*glColor3f(0,0,1);
	for(p = pixels.begin() ;p != pixels.end(); ++p){
		plotPoint(*p);
	}*/
}//set individual points

void display(void){//display function
        glClear (GL_COLOR_BUFFER_BIT);
	updateBoxes();
	glPointSize(5);
	glColor3f(0,0,0);
	if(curves.size() > 0){
		for(list<Curve>::iterator ptr = curves.begin(); ptr != curves.end(); ++ptr){
			if(ptr->active){
				//if(spines)
					//ptr->drawProcess();
				if(pars[LINES]) ptr->drawProcess();
				//glColor3f(0,.8,0);
				//if(pars[TSLIDE])ptr->drawT();
				//ptr->drawCastel();
				if(pars[CURVES]) ptr->draw();//ptr->drawBezier();
				if(pars[POLY]) ptr->drawControlPoly();
				//ptr->draw();
				if(pars[POINTS]){
					glColor3f(0,.6,.6);
					glPointSize(7);
					ptr->drawPoints();
				}
				if(pars[DIVIDES]) ptr->drawDivide();
				if(pars[TSLIDE]) ptr->drawAit();//ptr->drawT();
			}
		}
	}
	setPixels();
        glFlush();
}



void mousePassive(int x, int y){
	px = x-winWidth/2;
	py = winHeight/2-y;
	move();
	//char* ar,ar2;
}

void select(){
	selection.clear();
	for(list<Curve>::iterator ptr = curves.begin(); ptr != curves.end(); ++ptr){
		for(list<Point>::iterator ptr2 = ptr->ctlPts.begin(); ptr2 != ptr->ctlPts.end(); ++ptr2){
			if(ptr2->x <= px + tol && ptr2->x >= px - tol && ptr2->y <= py + tol && ptr2->y >= py - tol){
				selection.push_back(&(*ptr2));
			}
		}
	}

	for(list<Point>::iterator it = tempPix.begin(); it != tempPix.end(); ++it){
		if(it->x <= px + tol && it->x >= px - tol && it->y <= py + tol && it->y >= py - tol){
			selection.push_back(&(*it));
		}
	}
	glutPostRedisplay();
}

void move(){
	if(drag){
	/*for(list<Point*>::iterator ptr = selection.begin(); ptr != selection.end(); ++ptr){
			(*ptr)->x = px;
			(*ptr)->y = py;
		}*/
		selection.front()->x = px;
		selection.front()->y = py;
		glutPostRedisplay();
	}
	
}

void mouseMotion(GLint x, GLint y){
	/*if(drag){
		for(list<Point>::iterator ptr = selection.begin(); ptr != selection.end(); ++ptr){
			ptr->x += x - xs;
			ptr->y += y - ys;
		}
	}*/
}

void processMouseClick(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		cout << "clicked: " <<  px << " " << py << endl;
		xs = x; ys = y;
		glColor3f(0,0,0);
		switch(mode){
			case SELECT:
				select(); drag = false; break;
			case SET:
				tempPix.push_back(Point(x-winWidth/2,winHeight/2-y,0)); drag = false; break;
			case MOVE:
				drag = !drag; break;
				//move();	
			case UPDATE: break;
			default: drag = false;
		}
	}
	glutPostRedisplay();
}

void winReshapeFcn(GLint newWidth, GLint newHeight){
        glViewport(0,0,newWidth,newHeight);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(xwcMin, xwcMax, ywcMin, ywcMax, -1, 1);

        glClear(GL_COLOR_BUFFER_BIT);
}

void compare(){//find intersections and give output
	if(curves.size() < 2){
		cout << "Error: Too few lines, please try again" << endl;
		return;
	}
	list<Curve>::iterator ptr = curves.begin();
	list<Point> l,out;
	Curve** c;
	c = new Curve*[2];

	int i;
	for(i = 0; ptr != curves.end() && i < 2; ++ptr){
		if(ptr->active){
			c[i] = &(*ptr);
			++i;
		}
	}

	if(i == 1){
		cout << "Error: not enough curves are active, please activate more" << endl;
		return;
	}
	l = c[0]->intersection(c[1], error);

	if(l.size() < 1){
	  cout << "No Intersections Found\n";
	  return;
	}
	//else
	  //cout << "success " << l.size() << "\n";

	for(list<Point>::iterator it = l.begin(); it != l.end(); ++it){
		list<Point>::iterator it2 = it;
		for(++it2; it2 != l.end(); ++it2){
			if(abs(it->x - it2->x) < 2 && abs(it->y - it2->y) < 2){
				break;
			}
			if(++it2 == l.end()){
				out.push_back(*it);
			}
			else{
				--it2;
			}
		}
	}

	if(out.size() == 0)
		out.push_back(*l.begin());
	
	cout << "Intersections: ";
	for(list<Point>::iterator it = out.begin(); it != out.end(); ++it){
		cout << "( " << it->x << ", " << it->y << ")" ;
	}
	cout << '\n';

	delete c;
}

void updateBoxes(){
	list<Curve>::iterator ptr = curves.begin();
	char* ar;
	int offset = 40;
	int o2 = 0, o1 = nBox;
	if(nBox > 4){
		o1 = nBox - 5;
		o2 = 80;
	}
	string str = "C";
	scrolls[0]->type(Fl_Scroll::VERTICAL);
	if(nBox < curves.size()){
		scrolls[0]->begin();
		while(nBox < curves.size()){
			str = "C";
			str += to_string(nBox);
			checks[nBox] = new Fl_Check_Button(830 + o2,470 + offset*(o1+1),70,40, str.c_str());//create the new button
			checks[nBox]->value(true);
			ar = new char[str.length() + 1];
			strcpy(ar,str.c_str());
			checks[nBox]->label(ar);
			//delete ar;
			checks[nBox]->callback(check_cb);
			scrolls[0]->add(checks[nBox]);
			++nBox;
			//scrolls[0]->redraw();
		}
		scrolls[0]->begin();
	}
	else if(nBox > curves.size()){
		checks.clear();
		scrolls[0]->clear();
		for(int i = 0; i < nBox; ++i){
			delete checks[nBox];//delete old buttons
		}
		nBox = 0;
		scrolls[0]->redraw();
	}
	else{
		for(int i = 0; ptr != curves.end(); ++ptr, ++i){
			ptr->active = checks[i]->value();
		}
	}
	scrolls[0]->redraw();
}

int main(int argc, char** argv){
	//guiInit();
	Fl_Window win(1000,800,"Testing");
	win.show(argc,argv);
	win.begin();

	  buttons[0] = new Fl_Button(830,160,70,30, "Graph");
	  buttons[1] = new Fl_Button(910,160,70,30, "Clear");
	  buttons[2] = new Fl_Button(830,200,70,30, "Undo");
	  buttons[3] = new Fl_Button(910,200,70,30, "Intrsct");
	  buttons[4] = new Fl_Button(830,750,150,30, "Exit");

	  buttons[0]->callback(but_cb);
	  buttons[1]->callback(but_cb);
	  buttons[2]->callback(but_cb);
	  buttons[3]->callback(but_cb); 
	  buttons[4]->callback(but_cb); 

	  modes[0] = new Fl_Light_Button(830,10,70,30, "Select");
	  modes[1] = new Fl_Light_Button(910,10,70,30, "Set");
	  modes[2] = new Fl_Light_Button(830,50,70,30,"Move");
	  buttons[8] = new Fl_Button(830,85,120,30, "DeletePt");
	  buttons[8]->callback(but_cb);

	  buttons[5] = new Fl_Button(830,120,70,30, "DegLower");
	  buttons[6] = new Fl_Button(910,120,70,30, "DegRaise");
	  buttons[7] = new Fl_Button(910,50,70,30, "InsertPt");

	  modes[0]->value(true);
	  //modes[5]->value(true);
	
	  modes[0]->callback(mode_cb);
	  modes[1]->callback(mode_cb);
	  modes[2]->callback(mode_cb);
	  //modes[3]->callback(mode_cb);
	  buttons[5]->callback(algo_cb);
	  buttons[6]->callback(algo_cb);
	  buttons[7]->callback(but_cb);

	  params[POINTS] =  new Fl_Check_Button(830, 360, 120, 30, "control points");
	  params[CURVES] =  new Fl_Check_Button(830, 380, 120, 30, "curves");
	  params[LINES] =  new Fl_Check_Button(830, 400, 120, 30, "lines");
	  params[POLY] =  new Fl_Check_Button(830, 420, 120, 30, "polygons");
	  params[DIVIDES] =  new Fl_Check_Button(830, 440, 120, 30, "subdivides");
	  params[TSLIDE] =  new Fl_Check_Button(830, 460, 120, 30, "t-point");
	 for(int i = 0; i < 6; ++i){
	 	params[i]->callback(param_cb);
		params[i]->value(pars[i]);
	 }

  	  //textfields[0] = new Fl_Float_Input(830,125,50,30, "X");
  	  //textfields[1] = new Fl_Float_Input(910,125,50,30, "Y");

    	  slides[0] = new Fl_Value_Slider(820,250,160,30, "T");
	  slides[0]->slider_size(.1);
	  slides[0]->type(FL_HORIZONTAL);
	  slides[0]->range(0,1);
	  slides[0]->step(.0001);
	  slides[0]->callback(tValue_Slider_cb);

    	  slides[1] = new Fl_Value_Slider(820,320,160,30, "E");
	  slides[1]->slider_size(.1);
	  slides[1]->type(FL_HORIZONTAL);
	  slides[1]->range(.0001,20);
	  slides[1]->step(.0001);
	  slides[1]->value(.0001);
	  slides[1]->callback(tValue_Slider_cb);

	  scrolls[0] = new Fl_Scroll(830, 500, 150, 200, "Curves");
	  scrolls[0]->begin();
	  scrolls[0]->end();
	  updateBoxes();

	  //textfields[0]->when(FL_WHEN_CHANGED);
	  //textfields[1]->when(FL_WHEN_CHANGED);
	  /*textfields[0]->maximum_size(4);
	  textfields[1]->maximum_size(4);
	  textfields[0]->callback(text_cb);
	  textfields[1]->callback(text_cb);*/

          glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
          glutInitWindowPosition(0,0);
          glutInitWindowSize(winWidth, winHeight);
          MainWindow = glutCreateWindow("Main");
	win.end();

        init();
        glutDisplayFunc (display);
        glutReshapeFunc (winReshapeFcn);

        //set call back function to the one we want
        glutMouseFunc(processMouseClick);
        glutPassiveMotionFunc(mousePassive);
        glutMotionFunc(mouseMotion);
        
        //Create and attach menu to the window
        //createMenu();  
        glutMainLoop();
        //win.run();
        return 0;
}
        
