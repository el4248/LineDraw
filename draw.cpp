//Eric La
#include"draw.h"
const int NPOINTS = 1000;

Point::Point(GLfloat x, GLfloat y, GLfloat z){
	id = inCount++;
	this->x = x;
	this->y = y;
	this->z = z;
}

Point::Point(){
	//id = inCount++;
}

Point& Point::operator=(const Point& other){
	//this->id = other.id;
	this->x = other.x;
	this->y = other.y;
	this->z = other.z;

	return *this;
}

bool Point::operator==(const Point& other){
	if(this->id == other.id && this->x == other.x && this->y == other.y && this->z == other.z)
		return true;
	else
		return false;
}

bool Point::equal(const Point& other){
	if(this->id == other.id && this->x == other.x && this->y == other.y && this->z == other.z)
		return true;
	else
		return false;
}

void plotPoint(Point bezCurvePt){
	glBegin (GL_POINTS);
		glVertex2f(bezCurvePt.x, bezCurvePt.y);
	glEnd();
}

Curve::Curve(int numPts, list<Point> ctlPts){
	//this->drawFlags = 0;
	active = true;
	this->nCurvePts = NPOINTS;
	this->numPts = numPts;
	this->ctlPts = ctlPts;
}

Curve::Curve(){
	this->active = true;
	this->nCurvePts = 1000;
	this->numPts = 0;
}

Curve::~Curve(){
	ctlPts.clear();
	subCurves.clear();
}

Curve& Curve::operator=(const Curve& other){
	//this->drawFlags = other.drawFlags;
	this->active = other.active;
	this->nCurvePts = other.nCurvePts;
	this->numPts = other.numPts;
	this->ctlPts = other.ctlPts;
	return *this;
}

void Curve::binomialCoeffs (GLint n, GLint* C){
	GLint k, j;

	for(k = 0; k <= n; ++k){//n!/k!
		C[k]= 1;
		for(j = n; j >= k + 1; --j)//n!/k!
			C[k] *= j;
		for(j = n - k; j >= 2; --j)//(n-k)!
			C[k] /= j;
	}
}

void Curve::computeBezPt(GLfloat u, Point* bezPt, GLint nCtrlPts, list<Point> ctrlPts, GLint* C){
	GLint k, n = nCtrlPts - 1;
	GLfloat bezBlendFcn;
	list<Point>::iterator it;

	bezPt->x = bezPt->y = bezPt->z = 0.0;
//hi eric - jarred
	for(it = ctrlPts.begin(), k = 0; k < nCtrlPts; ++it, ++k){
		bezBlendFcn = C[k] * pow(u,k) * pow(1 - u, n - k);
		bezPt->x += it->x * bezBlendFcn;
		bezPt->y += it->y * bezBlendFcn;
		bezPt->z += it->z * bezBlendFcn;
	}
}

void Curve::bezier(list<Point> ctrlPts, GLint nCtrlPts, GLint nBezCurvePts){	
	Point bezCurvePt;
	GLfloat u;
	GLint *C, k;


	C = new GLint [nCtrlPts];
	Point prev = *ctrlPts.begin();
	binomialCoeffs (nCtrlPts - 1, C);
	glBegin(GL_LINES);
	for(k = 0; k <= nBezCurvePts; ++k){
		u = GLfloat(k)/GLfloat(nBezCurvePts);
		computeBezPt(u, &bezCurvePt, nCtrlPts, ctrlPts, C);
                glVertex2f(prev.x,prev.y);
                glVertex2f(bezCurvePt.x,bezCurvePt.y);
		prev = bezCurvePt;
	}
	glEnd();

	delete [ ] C;
}



Point Curve::castel(float t){
	Point** arr;
	if(t > 1) t = 1;//clamp upper
	if(t < 0) t = 0;//clamp lower
	if(numPts < 2) return Point();

	arr = new Point* [numPts];

	for(int i = 0; i < numPts; ++i){
		arr[i] = new Point[numPts - i];
	}//create top row

	list<Point>::iterator ptr = ctlPts.begin();
	
	for(int j = 0; j < numPts; ++j){
		arr[0][j] = *ptr;
		++ptr;
	}

	for(int i = 1; i < numPts; ++i){//add values in column
		for(int j = 0; j < numPts - i; ++j){
			arr[i][j].x = (1-t) * arr[i-1][j].x + t * arr[i-1][j+1].x;
			arr[i][j].y = (1-t) * arr[i-1][j].y + t * arr[i-1][j+1].y;
		}//parse row
	}//parse columns
	
	Point a(arr[numPts - 1][0].x, arr[numPts - 1][0].y, 0);

	for(int i = 0; i < numPts; ++i){
		delete arr[i];
	}

	delete arr;

	return a;
}

float* Curve::findBB(){
	list<Point>::iterator p = ctlPts.begin();
	float xmax = p->x, xmin = p->x, ymax = p->y, ymin = p->y;
	float* out = new float[4];

	for(;p != ctlPts.end(); ++p){
		if(p->x > xmax) xmax = p->x;
		if(p->x < xmin) xmin = p->x;
		if(p->y > ymax) ymax = p->y;
		if(p->y < ymin) ymin = p->y;
	}

	out[0] = xmin;
	out[1] = xmax;
	out[2] = ymin;
	out[3] = ymax;
	return out;
}

bool isOverlap(float *a, float *b){//xmin,xmax,ymin,ymax
	if(a[1] < b[0]) return false; //xmin < xmax
	if(a[0] > b[1]) return false; //xmin > xmax
	if(a[3] < b[2]) return false; //ymax < ymin
	if(a[2] > b[3]) return false; //ymin > ymax
	return true;
}

list<Point> Curve::intersection(Curve* other, double e){
	static long count = 0;
	list<Point> out, temp;
	Curve* a = this;
	Curve* b = other;
	float* bb1, *bb2;//bounding boxes
	float* ar;
	Point proxy;

	++count;
	bb1 = a->findBB();
	bb2 = b->findBB();

	if(abs(bb1[0] - bb2[0]) <= e 
	&& abs(bb1[1] - bb2[1]) <= e
	&& abs(bb1[2] - bb2[2]) <= e
	&& abs(bb1[3] - bb2[3]) <= e){
		ar = new float[4];
		for(int i = 0; i < 4; ++i){
			ar[i] = (bb1[i] + bb2[i])/2;
		}
		proxy.x = (ar[0] + ar[1])/2;
		proxy.y = (ar[2] + ar[3])/2;
		proxy.z = 0;
		out.push_back(proxy);
		delete bb1;
		delete bb2;
		delete ar;
		--count;
		return out;
	}

	if(count >= 500){
		cout << "ERROR: Stuck in loop. Please Restart\n";
	}
	//if(t <= 0.00001){ return out;}
	if(isOverlap(bb1,bb2) && count < 500){
		//insert each below into out
		this->subDivide(.5);
		other->subDivide(.5);
		delete bb1;
		delete bb2;
		
		temp = subCurves.front().intersection(&(other->subCurves.front()),e);
		out.insert(out.end(),temp.begin(), temp.end());

		temp = subCurves.front().intersection(&(other->subCurves.back()),e);
		out.insert(out.end(),temp.begin(), temp.end());

		temp = subCurves.back().intersection(&(other->subCurves.front()),e);
		out.insert(out.end(),temp.begin(), temp.end());

		temp = subCurves.back().intersection(&(other->subCurves.back()),e);
		out.insert(out.end(),temp.begin(), temp.end());
	}

	--count;
	return out;		
}

void Curve::subDivide(float t){
	Point** arr;
	Curve L,R;
	subCurves.clear();

	if(t > 1) t = 1;//clamp upper
	if(t < 0) t = 0;//clamp lower
	if(numPts < 2) return;

	arr = new Point* [numPts];

	for(int i = 0; i < numPts; ++i){
		arr[i] = new Point[numPts - i];
	}//create top row

	list<Point>::iterator ptr = ctlPts.begin();
	
	for(int j = 0; j < numPts; ++j){
		arr[0][j] = *ptr;
		++ptr;
	}

	for(int i = 1; i < numPts; ++i){//add values in column
		for(int j = 0; j < numPts - i; ++j){
			arr[i][j].x = (1-t) * arr[i-1][j].x + t * arr[i-1][j+1].x;
			arr[i][j].y = (1-t) * arr[i-1][j].y + t * arr[i-1][j+1].y;
		}//parse row

	}

	L.numPts = R.numPts = numPts;
	L.tVal = R.tVal = t;
	for(int i = 0; i < numPts; ++i){
		L.ctlPts.push_back(arr[i][0]);
	}

	for(int i = 0; i < numPts; ++i){
		R.ctlPts.push_back(arr[i][numPts-i-1]);
	}

	subCurves.push_back(L);
	subCurves.push_back(R);
	
	for(int i = 0; i < numPts; ++i){
		delete arr[i];
	}

	delete arr;
}

void Curve::drawDivide(){
	subDivide(tVal);
	Curve L;
	L = *(subCurves.begin());
	Curve R;
	R = subCurves.back();
	glColor3f(.7,0,.7);
	glPointSize(10);
	L.drawPoints();
	glPointSize(10);
	glColor3f(.7,.7,0);
	R.drawPoints();
}

void Curve::drawControlLines(){
	list<Point>::iterator ptr, ptr2, ptr3;
	ptr = ptr2 = ctlPts.begin();
	++ptr2;

	glColor3f(0,.5,0);
	for(int i = 0; i < numPts - 1; ++i, ++ptr, ++ptr2){
		glBegin(GL_LINES);
		glVertex3f(ptr->x,ptr->y,0);
		glVertex3f(ptr2->x,ptr2->y,0);
		glEnd();
	}
}

void Curve::drawProcess(){
	Point** arr;
	if(tVal > 1) tVal = 1;//clamp upper
	if(tVal < 0) tVal = 0;//clamp lower
	if(numPts < 2) return;

	arr = new Point* [numPts];

	for(int i = 0; i < numPts; ++i){
		arr[i] = new Point[numPts - i];
	}//create top row

	list<Point>::iterator ptr = ctlPts.begin();
	
	for(int j = 0; j < numPts; ++j){
		arr[0][j] = *ptr;
		++ptr;
	}

	for(int i = 1; i < numPts; ++i){//add values in column
		for(int j = 0; j < numPts - i; ++j){
			arr[i][j].x = (1-tVal) * arr[i-1][j].x + tVal * arr[i-1][j+1].x;
			arr[i][j].y = (1-tVal) * arr[i-1][j].y + tVal * arr[i-1][j+1].y;
		}//parse row
	}//parse columns
	
	glColor3f(0,0,0);//set outermost lines
	for(int i = 1; i < numPts; ++i){
		glBegin(GL_LINES);
		glVertex3f(arr[0][i-1].x,arr[0][i-1].y,0);
		glVertex3f(arr[0][i].x,arr[0][i].y,0);
		glEnd();
	}

	for(int i = 0; i < numPts; ++i){//set inner lines
		for(int j = 1; j < numPts - i; ++j){
			glColor3f(.5,.5,.5);
			glBegin(GL_LINES);
			glVertex3f(arr[i][j - 1].x,arr[i][j - 1].y,0);
			glVertex3f(arr[i][j].x,arr[i][j].y,0);
			glEnd();
			glColor3f(0,1,0);
			glPointSize(8);
			glBegin(GL_POINTS);//set tPoints
			glVertex3f(arr[i][j - 1].x,arr[i][j - 1].y,0);
			glVertex3f(arr[i][j].x,arr[i][j].y,0);
			glEnd();
		}
	}


	for(int i = 0; i < numPts; ++i){
		delete arr[i];
	}

	delete arr;

}

void Curve::drawPoints(){
	glBegin(GL_POINTS);
	for(list<Point>::iterator ptr = ctlPts.begin(); ptr != ctlPts.end(); ++ptr){
		glVertex3f(ptr->x,ptr->y,0);
	}
	glEnd();

	
}

void Curve::drawControlPoly(){
	drawControlLines();
	glBegin(GL_LINES);
	glVertex3f((ctlPts.begin())->x, (ctlPts.begin())->y,0);
	glVertex3f(ctlPts.back().x, ctlPts.back().y,0);
	glEnd();
	
}

void Curve::drawBezier(){
	glColor3f(.5,.5,0);
	glPointSize(5);
	bezier(ctlPts, numPts, nCurvePts);
}

void Curve::drawCastel(){
	Point a,b;
	glColor3f(0,0,0);
	glPointSize(5);
	glBegin(GL_LINES);
	for(int i = 1; i < 1000; ++i){
		a = castel((((float)i)-1)/1000);
		b = castel(((float)i)/1000);
		glVertex3f(a.x,a.y,0);
		glVertex3f(b.x,b.y,0);
	}
	glEnd();
}

void Curve::drawT(){
	Point a;
	glPointSize(10);
	glColor3f(0,.8,0);
	a = castel(tVal);
	glBegin(GL_POINTS);
	glVertex3f(a.x,a.y,0);
	glEnd();
}

Point* Curve::insertAt(Point a){
	Point* out = new Point();
	out->id = inCount++;
	unsigned long tempId = out->id;
	out->id = -1;
	for(list<Point>::iterator ptr = ctlPts.begin(); ptr != ctlPts.end(); ++ptr){
		if(*ptr == a){
			out->id = tempId;
			out->x = ptr->x;
			out->y = ptr->y;
			++numPts;
			return &(*(ctlPts.insert(ptr,*out)));
		}
	}

	return out;		
}

void Curve::deletePoint(Point* p){
	if(ctlPts.size() < 3){
		cout << "This curve has too few points\n";
		return;
	}

	for(list<Point>::iterator ptr = ctlPts.begin(); ptr != ctlPts.end(); ++ptr){
		if(*p == *ptr){
			ctlPts.erase(ptr);
			--numPts;
			return;
		}
	}
	drawBezier();
	glutPostRedisplay();
}

void Curve::draw(){
	list<Point>::iterator ptr, ptr2, ptr3;
	ptr = ptr2 = ctlPts.begin();
	++ptr2;

	if(numPts < 2)
		return;

	Point a,b;

	
	glColor3f(.5,.5,0);
	glPointSize(5);
	//bezier(ctlPts, numPts, nCurvePts);
	drawCastel();
}

void Curve::degreeRaise(){
	list<Point> newPts;
	int n = this->numPts;
	float x, y;
	double iter = 1;
	list<Point>::iterator it, it2;
	it = ctlPts.begin();
	it2 = ctlPts.begin();
	++it2;
	Point* start = &ctlPts.front();
	Point* end = &ctlPts.back();
	newPts.push_back(*start);
	for(int i = 1; i < numPts; iter = ++i, ++it, ++it2){//acutally numPts+1
		x = (iter/(n+1))*it->x + (1 - iter/(n + 1))*it2->x;
		y = (iter/(n+1))*it->y + (1 - iter/(n + 1))*it2->y;
		newPts.push_back(Point(x,y,0));
	}
	newPts.push_back(*end);
	numPts += 1;
	ctlPts.clear();
	ctlPts = newPts;
	glutPostRedisplay();
}

void Curve::degreeLower(){
	list<Point> temp;
	list<Point> temp2;
	float n = numPts - 1;
	float x, y;
	list<Point>::reverse_iterator it = ctlPts.rbegin();
	list<Point>::iterator it2 = ctlPts.begin();
	//temp.push_back(ctlPts.back());
	for(int i = numPts; i > 2; --i, ++it, ++it2){
		x = n/i * it->x - (n - i)/i * it->x;
		y = n/i * it->y - (n - i)/i * it->y;
		temp.push_front(Point(x,y,0));
		x = n/(n - i + 2) * it2->x - (i - 2)/(n - i + 2) * it2->x;
		y = n/(n - i + 2) * it2->y - (i - 2)/(n - i + 2) * it2->y;
		temp2.push_back(Point(x,y,0));
	}

	temp.push_front(ctlPts.front());
	temp2.push_back(ctlPts.back());

	it2 = temp2.begin();
	for(list<Point>::iterator ptr = temp.begin(); ptr != temp.end(); ++ptr, ++it2){
		ptr->x = (ptr->x + it2->x)/2;
		ptr->y = (ptr->y + it2->y)/2;
	}

	ctlPts = temp;
	numPts = ctlPts.size();
	glutPostRedisplay();
}

void Curve::drawAit(){
	Point a;
	glPointSize(10);
	glColor3f(0,.8,0);
	a = aitkens(tVal);
	glBegin(GL_POINTS);
	glVertex3f(a.x,a.y,0);
	glEnd();
}

Point Curve::aitkens(float t){
	Point** arr;
	if(t > 1) t = 1;//clamp upper
	if(t < 0) t = 0;//clamp lower
	//if(numPts < 2) return Point();

	arr = new Point* [numPts];

	for(int i = 0; i < numPts; ++i){
		arr[i] = new Point[numPts - i];
	}//create top row

	list<Point>::iterator ptr = ctlPts.begin(), ptr2;
	
	for(int j = 0; j < numPts; ++j){
		arr[0][j] = *ptr;
		++ptr;
	}//set first column

	float* tAr = new float[numPts];
	float* dAr = new float[numPts];

	float dx, dy;
	ptr = ptr2 = ctlPts.begin();
	++ptr2;
	float total = 0, temp;
	for(int i = 0; i < numPts; ++i, ++ptr, ++ptr2){
		dx = ptr2->x - ptr->x;
		dy = ptr2->y - ptr->y;
		temp = sqrt(dx * dx + dy * dy);
		//tAr[i] = temp;
		total += temp;
		dAr[i] = temp;
		tAr[i] = total;
	}

	for(int i = 0; i < numPts; ++i){
		dAr[i]/=total;
		tAr[i]/= total;
		//cout << dAr[i] << ", ";
	}
	

	for(int i = 1; i < numPts; ++i){//add values in column
		for(int j = 0; j < numPts - i; ++j){
			arr[i][j].x = (tAr[j] - t)/(dAr[j] + dAr[j+1]) * arr[i - 1][j].x + (t - tAr[j])/(dAr[j] + dAr[j + 1]) * arr[i - 1][j + 1].x;
		}//parse row
		total = 0;
		for(int k = 0 ; k < numPts - i; ++k){
			dx = arr[i][k].x - arr[i][k+1].x;
			dy = arr[i][k].y - arr[i][k+1].y;
			temp = sqrt(dx * dx + dy * dy);
			//tAr[k] = temp;
			total += temp;
			dAr[i] = temp;
			tAr[k] = total;
		}

		for(int k = 0; k < numPts - i; ++k){
			dAr[i]/=total;
			tAr[k]/= total;
		}

	}//parse columns

	delete dAr;
	delete tAr;
	
	Point a(arr[numPts - 1][0].x, arr[numPts - 1][0].y, 0);
	/*cout << "begin\n";
	for(int i = 0; i < numPts; ++i){
		for(int j = 0; j < numPts; ++j){
			cout << arr[j][i].x << " " << arr[j][i].y << " | ";
		}
		cout << "\n";
	}
	cout << "end\n";*/

	for(int i = 0; i < numPts; ++i){
		delete arr[i];
	}

	delete arr;

	return a;
}
