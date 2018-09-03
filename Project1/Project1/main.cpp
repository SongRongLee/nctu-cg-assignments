#define _USE_MATH_DEFINES
#define MOVING_SPEED 1.5
#include <direct.h>
#include <cmath>
#include "Scene.h"
#include "Light.h"
#include "View.h"
#include "glut.h"

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

void light();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void passivefunc(int, int);
void motionfunc(int, int);

int main(int argc, char** argv)
{
	//change here to test another Scene
	_chdir(".\\TestScene2");
	//---------------------------------

	//load files
	scene = new Scene("scene.scene");
	lights = new Light("light.light");
	view = new View("view.view");
	//calculate initail params
	theta = atan((view->eye[2] - view->vat[2]) / (view->eye[0] - view->vat[0])) * 180 / M_PI;
	radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
	selected = 0;

	glutInit(&argc, argv);
	glutInitWindowSize(view->vp[2], view->vp[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Project1");
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
		view->eye[0] *= 0.9;
		view->eye[1] *= 0.9;
		view->eye[2] *= 0.9;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	case 's'://zoom out
		view->eye[0] *= 1.1;
		view->eye[1] *= 1.1;
		view->eye[2] *= 1.1;
		radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));
		glutPostRedisplay();
		break;
	default://select object 1~9
		if (key >= '1'&&key <= '9') {
			selected = key - '1';
			if (selected >= scene->objects.size()) {
				selected = 0;
				cout << "Selection out of range!" << endl;
			}
			else {
				cout << "Object No." << key << " is selected." << endl;
			}
		}
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
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	// viewport transformation
	glViewport(view->vp[0], view->vp[1], view->vp[2], view->vp[3]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view->fovy, (GLfloat)view->vp[2] / (GLfloat)view->vp[3], view->dnear, view->dfar);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(view->eye[0], view->eye[1], view->eye[2],// eye
		view->vat[0], view->vat[1], view->vat[2],      // center
		view->vup[0], view->vup[1], view->vup[2]);     // up

	//注意light位置的設定，要在gluLookAt之後
	light();

	int lastMaterial = -1;
	for (int k = 0; k < scene->objects.size(); k++) {
		glPushMatrix();
			glTranslatef(scene->objects[k].Tx, scene->objects[k].Ty, scene->objects[k].Tz);
			glRotatef(scene->objects[k].Angle, scene->objects[k].Rx, scene->objects[k].Ry, scene->objects[k].Rz);
			glScalef(scene->objects[k].Sx, scene->objects[k].Sy, scene->objects[k].Sz);
			for (size_t i = 0; i < scene->objects[k].mymesh->fTotal; ++i)
			{
				// set material property if this face used different material
				if (lastMaterial != scene->objects[k].mymesh->faceList[i].m)
				{					
					lastMaterial = (int)scene->objects[k].mymesh->faceList[i].m;
					glMaterialfv(GL_FRONT, GL_AMBIENT, scene->objects[k].mymesh->mList[lastMaterial].Ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, scene->objects[k].mymesh->mList[lastMaterial].Kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR, scene->objects[k].mymesh->mList[lastMaterial].Ks);
					glMaterialfv(GL_FRONT, GL_SHININESS, &scene->objects[k].mymesh->mList[lastMaterial].Ns);
				}
				
				glBegin(GL_TRIANGLES);
				
				for (size_t j = 0; j<3; ++j)
				{
					//textex corrd. object->tList[object->faceList[i][j].t].ptr
					glNormal3fv(scene->objects[k].mymesh->nList[scene->objects[k].mymesh->faceList[i][j].n].ptr);
					glVertex3fv(scene->objects[k].mymesh->vList[scene->objects[k].mymesh->faceList[i][j].v].ptr);
				}				
				glEnd();				
			}
		glPopMatrix();
		lastMaterial = -1;
	}
	

	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	view->vp[2] = w;
	view->vp[3] = h;
}

