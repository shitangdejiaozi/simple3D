#ifndef LOADMODEL_H
#define LOADMODEL_H
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdlib.h>
#include <fstream>
#include <string>
#include "BasicMath.h"
#include "Device.h"

using namespace Assimp;
using namespace std;

OBJECT LoadModelToObject(const char * path, POINT4D pos);
void processNode(aiNode *node, const aiScene *scene, OBJECT_PTR obj, int &vertexNum, int &polyNum);
void processMesh(aiMesh *mesh, const aiScene *scene, OBJECT_PTR obj, int &vertexNum, int &polyNum);

void printPoint(POINT4D point)
{
	std::cout << point.x << ":" << point.y << ":" << point.z << ":" << point.w << std::endl;
}
//载入模型
OBJECT LoadModelToObject(const char * path, POINT4D pos)
{

	OBJECT obj;
	int vertexNum = 0;
	int polyNum = 0;
	FILE *file;
	errno_t error;
	
	string s;
	ifstream fin(path);
	if (!fin)
	{
		std::cout << "cant open file" << std::endl;
		return obj;
	}
	

	while (fin >> s)
	{
		const char *flag = s.c_str();
		if (strcmp(flag, "v") == 0)
		{
			POINT4D point;
			fin >> point.x >> point.y >> point.z;
			
			obj.vlist_local[vertexNum].x = point.x;
			obj.vlist_local[vertexNum].y = point.y;
			obj.vlist_local[vertexNum].z = point.z;
			obj.vlist_local[vertexNum].w = 1;
			vertexNum++;
			std::cout << point.x << ":" << point.y << ":" << point.z << std::endl;
		}
		else if (strcmp(flag, "f") == 0)
		{
			int vertexIndex[3];
			fin >> vertexIndex[0] >> vertexIndex[1] >> vertexIndex[2];
			obj.plist[polyNum].state = POLY_STATE_ACTIVE;
			obj.plist[polyNum].attr = POLY_ATTR_SHADE_MODE_FLAT;
			obj.plist[polyNum].color = RGBABIT(0, 255, 0, 0);
			obj.plist[polyNum].vertex_list = obj.vlist_local;
			obj.plist[polyNum].vert[0] = vertexIndex[0] - 1;  //obj的indice从1开始
			obj.plist[polyNum].vert[1] = vertexIndex[1] - 1;
			obj.plist[polyNum].vert[2] = vertexIndex[2] - 1;

			polyNum++;
			std::cout << vertexIndex[0] << ":" << vertexIndex[1] << ":" << vertexIndex[2] << std::endl;
		}
	}
	obj.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;
	obj.world_pos = pos;
	obj.num_polys = polyNum;
	obj.num_vertices = vertexNum;
	return obj;
}


OBJECT LoadModel(const char * path, POINT4D pos)
{

	OBJECT obj;
	int vertexNum = 0;
	int polyNum = 0;
	FILE *file;
	errno_t error;

	if (error = fopen_s(&file, path, "r"))
	{
		cout << "cant open" << endl;
		return obj;
	}

	while (1)
	{
		char header[128];
		int res = fscanf_s(file, "%s", header, 128);
		if (res == EOF)
			break;
		if (strcmp(header, "v") == 0)
		{
			POINT4D point;
			fscanf_s(file, "%f %f %fn", &point.x, &point.y, &point.z);
			obj.vlist_local[vertexNum].x = point.x;
			obj.vlist_local[vertexNum].y = point.y;
			obj.vlist_local[vertexNum].z = point.z;
			obj.vlist_local[vertexNum].w = 1;
			vertexNum++;
			std::cout << point.x << ":" << point.y << ":" << point.z << std::endl;
		}
		else if (strcmp(header, "vt") == 0)
		{
			
		}
		else if (strcmp(header, "vn") == 0)
		{
			VECTOR3D  normal;
			fscanf_s(file, "%f %f %fn", &normal.x, &normal.y, &normal.z);
		}
		else if (strcmp(header, "f") == 0)
		{
			int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%dn", &vertexIndex[0], &uvIndex[0],&normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1],&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			obj.plist[polyNum].state = POLY_STATE_ACTIVE;
			obj.plist[polyNum].attr = POLY_ATTR_SHADE_MODE_FLAT;
			obj.plist[polyNum].color = RGBABIT(0, 255, 0, 0);
			obj.plist[polyNum].vertex_list = obj.vlist_local;
			obj.plist[polyNum].vert[0] = vertexIndex[0] - 1;  //obj的indice从1开始
			obj.plist[polyNum].vert[1] = vertexIndex[1] - 1;
			obj.plist[polyNum].vert[2] = vertexIndex[2] - 1;
			std::cout << vertexIndex[0] << ":" << vertexIndex[1] << ":" << vertexIndex[2] << std::endl;

			polyNum++;
		}
		
	}

	obj.state = OBJECT_STATE_ACTIVE | OBJECT_STATE_VISIBLE;
	obj.world_pos = pos;
	obj.num_polys = polyNum;
	obj.num_vertices = vertexNum;
	return obj;
}

//递归遍历所有的节点
void processNode(aiNode *node, const aiScene *scene, OBJECT_PTR obj, int &vertexNum, int &polyNum)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene, obj, vertexNum, polyNum);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, obj, vertexNum, polyNum);
	}
}

void processMesh(aiMesh *mesh, const aiScene *scene, OBJECT_PTR obj, int &vertexNum, int &polyNum)
{
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertexNum++;
		obj->vlist_local[vertexNum].x = mesh->mVertices[i].x;
		obj->vlist_local[vertexNum].y = mesh->mVertices[i].y;
		obj->vlist_local[vertexNum].z = mesh->mVertices[i].z;
		obj->vlist_local[vertexNum].w = 1;
		printPoint(obj->vlist_local[vertexNum]);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		polyNum++;
		aiFace face = mesh->mFaces[i];
		obj->plist[polyNum].state = POLY_STATE_ACTIVE;
		obj->plist[polyNum].attr = POLY_ATTR_SHADE_MODE_FLAT;
		obj->plist[polyNum].color = RGBABIT(0, 255, 0, 0);
		obj->plist[polyNum].vertex_list = obj->vlist_local;
		std::cout << "face: " << std::endl;
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			obj->plist[polyNum].vert[j] = face.mIndices[j];
			std::cout << "indice :" << face.mIndices[j] +1 << std::endl;
		}
	}
}


#endif
