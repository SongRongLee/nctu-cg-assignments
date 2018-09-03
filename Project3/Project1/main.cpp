#define _USE_MATH_DEFINES
#define MOVING_SPEED 1.5

#include <direct.h>
#include <cmath>

#include "Scene.h"
#include "Light.h"
#include "View.h"
#include "ShaderLoader.h"

//file loaders
Scene *scene;
Light *lights;
View *view;
GLhandleARB MyShader;

//sene manipulation params
float theta;
float radius;
int LEVEL = 0;
int windowSize[2];

//self-defined function
void light();
void LoadShaders();
//bind functions
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);

int main(int argc, char** argv)
{
	//change here to test another Scene
	_chdir(".\\p4");

	//initialize
	glutInit(&argc, argv);
	FreeImage_Initialise();

	//load files
	lights = new Light("light.light");
	view = new View("view.view");

	glutInitWindowSize(view->vp[2], view->vp[3]);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Project3");
	glewInit();
	
	scene = new Scene("scene.scene");
	FreeImage_DeInitialise();
	LoadShaders();

	//calculate initial params
	theta = atan((view->eye[2] - view->vat[2]) / (view->eye[0] - view->vat[0])) * 180 / M_PI;
	radius = sqrt(pow((view->eye[0] - view->vat[0]), 2) + pow((view->eye[2] - view->vat[2]), 2));

	//set functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
void LoadShaders()
{
	MyShader = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "../P3Shading.vert", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "../P3Shading.geom", GL_GEOMETRY_SHADER);
		ShaderLoad(MyShader, "../P3Shading.frag", GL_FRAGMENT_SHADER);
	}
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
	case 'f'://move light decrease y
		for (int i = 0; i < lights->light_list.size(); i++) {
			lights->light_list[i].pos[1] -= 5;
		}
		glutPostRedisplay();
		break;
	case 'r'://move light increase y
		for (int i = 0; i < lights->light_list.size(); i++) {
			lights->light_list[i].pos[1] += 5;
		}
		glutPostRedisplay();
		break;
	default://change level
		if (key >= '1'&&key <= '3') {
			LEVEL = key - '1';
		}
		glutPostRedisplay();
		break;
	}

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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

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

	light();										   //注意light位置的設定，要在gluLookAt之後

	//start drawing
	int lastMaterial = -1;
	for (int k = 0; k < scene->objects.size(); k++) {
		//set shader
		glUseProgram(MyShader);
		glUniform1i(glGetUniformLocation(MyShader, "lightNumber"), lights->light_list.size());
		glUniform1i(glGetUniformLocation(MyShader, "colorTexture"), 0);
		glUniform1i(glGetUniformLocation(MyShader, "Level"), LEVEL);
		glUniform1f(glGetUniformLocation(MyShader, "Radius"), 1.0f);
		glUniform4f(glGetUniformLocation(MyShader, "Centroid"), 0.0f, 1.0f, 0.0f, 1.0f);
		//set texture type
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, scene->text_obj[scene->objects[k].text_ind[0]]);
		//transform matrix
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

			//draw poly
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glMultiTexCoord2fv(GL_TEXTURE0, scene->objects[k].mymesh->tList[scene->objects[k].mymesh->faceList[i][j].t].ptr);
				glNormal3fv(scene->objects[k].mymesh->nList[scene->objects[k].mymesh->faceList[i][j].n].ptr);
				glVertex3fv(scene->objects[k].mymesh->vList[scene->objects[k].mymesh->faceList[i][j].v].ptr);
			}
			glEnd();
		}		
		glPopMatrix();
		lastMaterial = -1;
		//disable texture
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glutSwapBuffers();		
}
void reshape(GLsizei w, GLsizei h)
{
	view->vp[2] = w;
	view->vp[3] = h;
}