#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class View
{
public:
	float eye[3];
	float vat[3];
	float vup[3];
	float fovy, dnear, dfar;
	float vp[4];
	View();
	virtual ~View();
	View(string view_file);
	void LoadView(string view_file);
};

#endif
