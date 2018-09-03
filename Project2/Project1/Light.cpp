#include "Light.h"

Light::Light() {
}
Light::Light(string light_file) {
	LoadLight(light_file);
}
Light::~Light()
{
}
void Light::LoadLight(string light_file)
{
	string token;
	int light_count = 0;
	ifstream in(light_file);

	cout << endl <<  light_file << endl;

	while (in >> token)
	{
		if (token == "light")
		{
			Lightsource newlight;
			in >> newlight.pos[0] >> newlight.pos[1] >> newlight.pos[2];
			in >> newlight.ambient[0] >> newlight.ambient[1] >> newlight.ambient[2];
			in >> newlight.diffuse[0] >> newlight.diffuse[1] >> newlight.diffuse[2];
			in >> newlight.spec[0] >> newlight.spec[1] >> newlight.spec[2];
			newlight.id = GL_LIGHT0 + light_count;
			newlight.pos[3] = 1;
			light_count++;
			light_list.push_back(newlight);
		}
		else {
			in >> env_ambient[0] >> env_ambient[1] >> env_ambient[2];
		}
	}
	in.close();
	cout << endl << "Read Light File done." << endl;
}