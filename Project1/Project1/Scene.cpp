#include "Scene.h"

Scene::Scene() {
}
Scene::Scene(string scene_file) {
	LoadScene(scene_file);
}
Scene::~Scene()
{
}
void Scene::LoadScene(string scene_file)
{
	string token, obj_file;
	ifstream in(scene_file);

	cout << scene_file << endl;

	while (in >> token)
	{
		if (token == "model")
		{
			in >> obj_file;
			OBJ newOBJ(new mesh(obj_file));
			in >> newOBJ.Sx >> newOBJ.Sy >> newOBJ.Sz;
			in >> newOBJ.Angle >> newOBJ.Rx >> newOBJ.Ry >> newOBJ.Rz;
			in >> newOBJ.Tx >> newOBJ.Ty >> newOBJ.Tz;
			objects.push_back(newOBJ);
		}
	}
	in.close();
	cout << endl << "Read Sene File done." << endl;
}