#define _USE_MATH_DEFINES
#define MOVING_SPEED 1.5
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

void light();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void passivefunc(int, int);
void motionfunc(int, int);

int main(int argc, char** argv)
{
	//change here to test another Scene
	_chdir(".\\park");

	//initialize
	glutInit(&argc, argv);
	FreeImage_Initialise();

	//load files
	lights = new Light("light.light");
	view = new View("view.view");

	glutInitWindowSize(view->vp[2], view->vp[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Project1 Phase2");
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
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glDisable(GL_CULL_FACE);
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
			//choose texture mode
			if (scene->objects[k].texture_mode == "single-texture") {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, scene->text_obj[scene->objects[k].text_ind[0]]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}
			else if (scene->objects[k].texture_mode == "multi-texture") {
				//first texture
				glActiveTexture(GL_TEXTURE0);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, scene->text_obj[scene->objects[k].text_ind[0]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
				//second texture
				glActiveTexture(GL_TEXTURE1);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, scene->text_obj[scene->objects[k].text_ind[1]]);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
				glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
			}
			else if (scene->objects[k].texture_mode == "cube-map") {
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glEnable(GL_TEXTURE_GEN_S);glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, scene->text_obj[scene->objects[k].text_ind[0]]);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			}

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
					if (scene->objects[k].texture_mode == "single-texture" || scene->objects[k].texture_mode == "cube-map") {
						glTexCoord2fv(scene->objects[k].mymesh->tList[scene->objects[k].mymesh->faceList[i][j].t].ptr);
					}
					else if (scene->objects[k].texture_mode == "multi-texture") {
						glMultiTexCoord2fv(GL_TEXTURE0, scene->objects[k].mymesh->tList[scene->objects[k].mymesh->faceList[i][j].t].ptr);
						glMultiTexCoord2fv(GL_TEXTURE1, scene->objects[k].mymesh->tList[scene->objects[k].mymesh->faceList[i][j].t].ptr);
					}
					glNormal3fv(scene->objects[k].mymesh->nList[scene->objects[k].mymesh->faceList[i][j].n].ptr);
					glVertex3fv(scene->objects[k].mymesh->vList[scene->objects[k].mymesh->faceList[i][j].v].ptr);
				}				
				glEnd();				
			}
			//unbind texture
			if (scene->objects[k].texture_mode == "single-texture") {
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			else if (scene->objects[k].texture_mode == "cube-map") {
				glDisable(GL_TEXTURE_GEN_S);glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);glDisable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);				
			}
			else if (scene->objects[k].texture_mode == "multi-texture") {
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
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

