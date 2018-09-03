#ifndef LIGHT_H
#define LIGHT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "glut.h"

using namespace std;

class Light
{
	class Lightsource
	{
	public:
		int id;
		float pos[4];
		float ambient[3];
		float diffuse[3];
		float spec[3];
	};
public:
	vector<Lightsource> light_list;
	float env_ambient[3];
	Light();
	virtual ~Light();
	Light(string light_file);
	void LoadLight(string light_file);
};

#endif
