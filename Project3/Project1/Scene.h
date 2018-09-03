#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "FreeImage.h"
#include "glew.h"
#include "glut.h"

#include "mesh.h"
using namespace std;

class Scene
{
	class OBJ			
	{
	public:
		mesh *mymesh;
		string texture_mode;
		vector<int> text_ind; //index in text_obj		
		float Sx, Sy, Sz;
		float Angle, Rx, Ry, Rz;
		float Tx, Ty, Tz;
		OBJ(mesh *mymesh)
		{
			this->mymesh = mymesh;
		}
	};
public:
	vector<OBJ> objects;
	vector<unsigned int> text_obj;
	Scene();
	virtual ~Scene();
	Scene(string scene_file);
	void LoadScene(string scene_file);
	int CreateTexture(char *texture_file);				//return index in text_obj
	int CreateCubeTexture(char texture_files[6][50]);//return index in text_obj
};

#endif
