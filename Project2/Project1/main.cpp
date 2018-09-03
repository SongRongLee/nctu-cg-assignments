#define _USE_MATH_DEFINES
#define MOVING_SPEED 1.5
#define PASS_NUM 5
#define DOF_NUM 8
#define APERTURE 0.1

#include <direct.h>
#include <cmath>

#include "Scene.h"
#include "Light.h"
#include "View.h"

//file loaders
Scene *scene;
Light *lights;
View *view;

//sene manipulation params
float theta;
float radius;
int selected;
int prepos[2];
int windowSize[2];

//self-defined function
void light();
void draw_obj(int, int, bool);
void draw_objs(int, bool);
void draw_mirror(int, bool);
vector<float> cross(vector<float>, vector<float>);
void normalize(vector<float>&);

//bind functions
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void passivefunc(int, int);
void motionfunc(int, int);

int main(int argc, char** argv)
{
	//change here to test another Scene
	_chdir(".\\cornell");

	//initialize
	glutInit(&argc, argv);
	FreeImage_Initialise();

	//load files
	lights = new Light("light.light");
	view = new View("view.view");

	glutInitWindowSize(view->vp[2], view->vp[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM | GLUT_STENCIL);
	glutCreateWindow("Project2");
	glewInit();
	
	scene = new Scene("scene.scene");
	FreeImage_DeInitialise();

	//calculate initial params
	theta = atan((view->eye[2] - view->vat[2]) / (view->eye[0] - view->vat[0])) * 180 / M_PI;
	radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
	selected = 0;

	//set functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(passivefunc);
	glutMotionFunc(motionfunc);
	glutMainLoop();

	return 0;
}
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a'://circle the center
		theta += 5;
		if (theta >= 360)theta -= 360;
		view->eye[0] = radius*cos(theta*M_PI / 180) + view->vat[0];
		view->eye[2] = radius*sin(theta*M_PI / 180) + view->vat[2];
		glutPostRedisplay();
		break;
	case 'd'://circle the center
		theta -= 5;
		if (theta <= 0)theta += 360;
		view->eye[0] = radius*cos(theta*M_PI / 180) + view->vat[0];
		view->eye[2] = radius*sin(theta*M_PI / 180) + view->vat[2];
		glutPostRedisplay();
		break;
	case 'w'://zoom in
		view->eye[0] = view->eye[0] - (view->eye[0] - view->vat[0])*0.1;
		view->eye[1] = view->eye[1] - (view->eye[1] - view->vat[1])*0.1;
		view->eye[2] = view->eye[2] - (view->eye[2] - view->vat[2])*0.1;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	case 's'://zoom out
		view->eye[0] = view->eye[0] + (view->eye[0] - view->vat[0])*0.1;
		view->eye[1] = view->eye[1] + (view->eye[1] - view->vat[1])*0.1;
		view->eye[2] = view->eye[2] + (view->eye[2] - view->vat[2])*0.1;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	case '1'://zoom out
		view->vat[0] = -10;
		view->vat[1] = 12;
		view->vat[2] = 0;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	case '2'://zoom out
		view->vat[0] = -50;
		view->vat[1] = 12;
		view->vat[2] = 0;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	case '3'://zoom out
		view->vat[0] = -400;
		view->vat[1] = 12;
		view->vat[2] = 0;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;	
	}

}
void passivefunc(int x, int y) {
	prepos[0] = x;
	prepos[1] = y;
}
void motionfunc(int x, int y) {
	//move selected object while dragging
	scene->objects[selected].Tx += (x - prepos[0]) / view->vp[2] * radius * MOVING_SPEED;
	scene->objects[selected].Ty += (prepos[1] - y) / view->vp[3] * radius * MOVING_SPEED;
	prepos[0] = x;
	prepos[1] = y;
	glutPostRedisplay();
}
void light()
{
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);
	// enable lighting
	glEnable(GL_LIGHTING);
	// set lights property
	for (int i = 0; i < lights->light_list.size(); i++) {
		glEnable(lights->light_list[i].id);
		glLightfv(lights->light_list[i].id, GL_POSITION, lights->light_list[i].pos);
		glLightfv(lights->light_list[i].id, GL_DIFFUSE, lights->light_list[i].diffuse);
		glLightfv(lights->light_list[i].id, GL_SPECULAR, lights->light_list[i].spec);
		glLightfv(lights->light_list[i].id, GL_AMBIENT, lights->light_list[i].ambient);		
	}	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lights->env_ambient);
}

void display()
{
	//enable tests
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
	glClearStencil(PASS_NUM);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);	
	glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

	// viewport transformation
	glViewport(view->vp[0], view->vp[1], view->vp[2], view->vp[3]);

	//DOF
	for (int dof = 0; dof < DOF_NUM; dof++) {
		glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		// projection transformation
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(view->fovy, (GLfloat)view->vp[2] / (GLfloat)view->vp[3], view->dnear, view->dfar);
	
		//calculate jitter--------------------------------
		float jitter[3];
		//calculate new coord.
		vector<float> forward, right, up, tmpup;
		for (int i = 0; i < 3; i++) {
			forward.push_back(view->vat[i] - view->eye[i]);
			tmpup.push_back(view->vup[i]);
		}
		normalize(forward);
		right = cross(forward,tmpup);
		up = cross(right, forward);
		//get jitter position
		float angle = 2 * M_PI * dof / DOF_NUM;
		for (int i = 0; i < 3; i++) {
			jitter[i] = view->eye[i] + right[i] * APERTURE * cos(angle) + up[i] * APERTURE * sin(angle);
		}
		//------------------------------------------------

		// viewing and modeling transformation
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(jitter[0], jitter[1], jitter[2],		   // eye
			view->vat[0], view->vat[1], view->vat[2],      // center
			view->vup[0], view->vup[1], view->vup[2]);     // up

		light();										   //注意light位置的設定，要在gluLookAt之後

		//set stencil buffer------------------------------
		//for front mirror
		bool isfront = true;
		if (view->eye[0] > -40) {
			for (int pass = 0; pass < PASS_NUM; pass++) {
				//mirror part
				glStencilFunc(GL_EQUAL, PASS_NUM - pass, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				draw_mirror(pass, isfront);
				//objs part
				glStencilFunc(GL_EQUAL, PASS_NUM - pass - 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				draw_objs(pass, isfront);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		//for back mirror
		if (view->eye[0] < 40) {
			isfront = false;
			for (int pass = 0; pass < PASS_NUM; pass++) {
				//mirror part
				glStencilFunc(GL_EQUAL, PASS_NUM + pass, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
				draw_mirror(pass, isfront);
				//objs part
				glStencilFunc(GL_EQUAL, PASS_NUM + pass + 1, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
				draw_objs(pass, isfront);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		//------------------------------------------------------

		//draw scene--------------------------------------------
		int pass_num = PASS_NUM;
		/*if (view->eye[0] < -40 || view->eye[0] > 40) {
			pass_num = 1;
			glDisable(GL_STENCIL_TEST);
			draw_mirror(0, true);
			draw_mirror(0, false);
		}*/
		//check if eye is out of the box
		if (view->eye[0] > -40) {
			//draw front mirror part
			isfront = true;
			for (int pass = 0; pass < pass_num; pass++) {
				glStencilFunc(GL_EQUAL, PASS_NUM - pass, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
				draw_objs(pass, isfront);
			}
		}
		if (view->eye[0] < 40) {
			//draw back mirror part
			isfront = false;
			for (int pass = 0; pass < pass_num; pass++) {
				glStencilFunc(GL_EQUAL, PASS_NUM + pass, 0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
				draw_objs(pass, isfront);
			}
		}
		//------------------------------------------------------
		glAccum(dof ? GL_ACCUM : GL_LOAD, 1.0 / DOF_NUM);
	}
	glAccum(GL_RETURN, 1.0);
	glutSwapBuffers();		
}
void draw_objs(int pass, bool front) {
	for (int k = 0; k < scene->objects.size() - 2; k++) {		
		draw_obj(k, pass, front);
	}
}
void draw_mirror(int pass, bool front) {
	int k;	
	//set mirror idx
	if (pass % 2 == 0) {
		if (front) {
			k = scene->objects.size() - 2;
		}
		else {
			k = scene->objects.size() - 1;
		}
	}
	else {
		if (front) {
			k = scene->objects.size() - 1;
		}
		else {
			k = scene->objects.size() - 2;
		}
	}	
	draw_obj(k, pass, front);
}
void draw_obj(int k, int pass, bool front) {
	float trans_factor, scale_factor;
	int lastMaterial = -1;
	//set direction
	if (front) {
		trans_factor = -1;
	}
	else {
		trans_factor = 1;
	}
	//set facing
	if (pass % 2 == 0) {
		scale_factor = 1;
	}
	else {
		scale_factor = -1;
	}

	//start drawing
	glPushMatrix();

	//cal reflection
	glTranslatef(80 * trans_factor * pass, 0, 0);
	glScalef(scale_factor, 1, 1);

	//origin ops
	glTranslatef(scene->objects[k].Tx, scene->objects[k].Ty, scene->objects[k].Tz);
	glRotatef(scene->objects[k].Angle, scene->objects[k].Rx, scene->objects[k].Ry, scene->objects[k].Rz);
	glScalef(scene->objects[k].Sx, scene->objects[k].Sy, scene->objects[k].Sz);

	for (size_t i = 0; i < scene->objects[k].mymesh->fTotal; ++i)
	{
		// set material property if this face used different material
		if (lastMaterial != scene->objects[k].mymesh->faceList[i].m) {
			lastMaterial = (int)scene->objects[k].mymesh->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, scene->objects[k].mymesh->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, scene->objects[k].mymesh->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, scene->objects[k].mymesh->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &scene->objects[k].mymesh->mList[lastMaterial].Ns);
		}
		glBegin(GL_TRIANGLES);

		for (size_t j = 0; j<3; ++j) {
			glNormal3fv(scene->objects[k].mymesh->nList[scene->objects[k].mymesh->faceList[i][j].n].ptr);
			glVertex3fv(scene->objects[k].mymesh->vList[scene->objects[k].mymesh->faceList[i][j].v].ptr);
		}
		glEnd();
	}
	glPopMatrix();
	lastMaterial = -1;
}
void reshape(GLsizei w, GLsizei h)
{
	view->vp[2] = w;
	view->vp[3] = h;
}
vector<float> cross(vector<float> a, vector<float> b) {
	vector<float> ans;
	ans.push_back(a[1] * b[2] - a[2] * b[1]);
	ans.push_back(a[2] * b[0] - a[0] * b[2]);
	ans.push_back(a[0] * b[1] - a[1] * b[0]);
	normalize(ans);
	return ans;
}
void normalize(vector<float>& a) {
	float sum = 0;
	for (int i = 0; i < 3; i++) {
		sum += a[i];
	}
	if(sum != 0)
	for (int i = 0; i < 3; i++) {
		a[i] /= sum;
	}
}