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
	string token, obj_file, texture_mode;
	char texture_file[50];
	vector<int> tmp_ind;
	ifstream in(scene_file);
	std::cout << scene_file << endl;

	while (in >> token){		
		if (token == "model")
		{
			//set position
			in >> obj_file;
			OBJ newOBJ(new mesh(obj_file));
			in >> newOBJ.Sx >> newOBJ.Sy >> newOBJ.Sz;
			in >> newOBJ.Angle >> newOBJ.Rx >> newOBJ.Ry >> newOBJ.Rz;
			in >> newOBJ.Tx >> newOBJ.Ty >> newOBJ.Tz;

			//set texture
			newOBJ.texture_mode = texture_mode;
			newOBJ.text_ind = tmp_ind;
			objects.push_back(newOBJ);
		}
		else {
			int textnum = 0;
			tmp_ind.clear();
			texture_mode = token;
			if (token == "single-texture") {
				in >> texture_file;
				tmp_ind.push_back(CreateTexture(texture_file));
			}
			else if (token == "multi-texture") {
				for (int i = 0; i < 2; i++) {
					in >> texture_file;
					tmp_ind.push_back(CreateTexture(texture_file));
				}				
			}
			else if (token == "cube-map") {
				char cube_files[6][50];
				for (int i = 0; i < 6; i++) {					
					in >> cube_files[i];
				}
				tmp_ind.push_back(CreateCubeTexture(cube_files));
			}
		}
	}
	in.close();
	std::cout << endl << "Read Sene File done." << endl;
}
int Scene::CreateTexture(char *texture_file) {
	int re_index;
	unsigned int tmp;
	glGenTextures(1, &tmp);
	text_obj.push_back(tmp);
	re_index = text_obj.size() - 1;

	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(texture_file), texture_file);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, text_obj[re_index]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
	return re_index;
}

int Scene::CreateCubeTexture(char texture_files[6][50]) {
	int re_index;
	unsigned int tmp;
	glGenTextures(1, &tmp);
	text_obj.push_back(tmp);
	re_index = text_obj.size() - 1;

	FIBITMAP* pImage;
	FIBITMAP* p32BitsImage[6];
	int iWidth[6];
	int iHeight[6];
	for (int i = 0; i < 6; i++) {
		pImage = FreeImage_Load(FreeImage_GetFileType(texture_files[i]), texture_files[i]);
		p32BitsImage[i] = FreeImage_ConvertTo32Bits(pImage);
		iWidth[i] = FreeImage_GetWidth(p32BitsImage[i]);
		iHeight[i] = FreeImage_GetHeight(p32BitsImage[i]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, text_obj[re_index]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, iWidth[0], iHeight[0], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[0]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, iWidth[1], iHeight[1], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[1]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, iWidth[2], iHeight[2], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[2]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, iWidth[3], iHeight[3], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[3]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, iWidth[4], iHeight[4], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[4]));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, iWidth[5], iHeight[5], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage[5]));

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	for (int i = 0; i < 6; i++) {
		FreeImage_Unload(p32BitsImage[i]);
	}	
	FreeImage_Unload(pImage);
	return re_index;
}