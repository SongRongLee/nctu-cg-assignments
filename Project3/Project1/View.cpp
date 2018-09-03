#include "View.h"

View::View() {
}
View::View(string view_file) {
	LoadView(view_file);
}
View::~View()
{
}
void View::LoadView(string view_file)
{
	string token;

	ifstream in(view_file);

	cout << endl << view_file << endl;

	while (in >> token)
	{
		if (token == "eye")
		{
			in >> eye[0] >> eye[1] >> eye[2];
		}
		else if(token == "vat"){
			in >> vat[0] >> vat[1] >> vat[2];
		}
		else if (token == "vup") {
			in >> vup[0] >> vup[1] >> vup[2];
		}
		else if (token == "fovy") {
			in >> fovy;
		}
		else if (token == "dnear") {
			in >> dnear;
		}
		else if (token == "dfar") {
			in >> dfar;
		}
		else if (token == "viewport") {
			in >> vp[0] >> vp[1] >> vp[2] >> vp[3];
		}
	}
	in.close();
	cout << endl << "Read View File done." << endl;
}