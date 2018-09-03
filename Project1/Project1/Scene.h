#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "mesh.h"
using namespace std;

class Scene
{
	class OBJ			
	{
	public:
		mesh *mymesh;
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
	Scene();
	virtual ~Scene();
	Scene(string scene_file);
	void LoadScene(string scene_file);
};

#endif
