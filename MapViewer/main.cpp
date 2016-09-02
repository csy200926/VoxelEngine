#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "BitMap/BitMap.h"
#include "detail/func_common.hpp"

#include "Utilities/WinTiming.h"
#include "Utilities.h"
#include "Componenets/Camera.h"

#include "FractalNoise.h"

#include "core/stdafx.h"
#include "IOAndRes/agoc/CubeTileData.h"
#include "IOAndRes/agoc/cubeworld_interface.h"
#include "Componenets/Camera.h"
#include "Componenets/MeshPoints.h"
#include "Componenets/Material.h"
#include "Componenets/Mesh.h"
#include "Componenets/Texture.h"
#include "GreedyMeshing.h"
#include "Game/World.h"

#include "core/core.h"
#include "stdio.h"
#include "stdlib.h"
#include <time.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#include <windows.h>

#include "StrangeTree.h"

#define NEW_RENDERER
//#define ENABLE_NETWORK

using namespace AGE;

using namespace SYSR;
using namespace glm;

unsigned int ScreenHeight = 768, ScreenWidth = 1024;

#pragma region static variables
GLFWwindow* window;


#pragma endregion


#pragma region input Call backs

// Input helpers
int keyStates[400];
int mouseState[8];
vec2 s_mouePos, s_lastMousePos(400, 300);
bool s_wireframeMode = false;
#define KEY_HELD 1
#define KEY_RELEASED 0 

// Document says do not use this to implement camera control?
void SetMousePos(double x, double y)
{
	glfwSetCursorPos(window, x, y);
}

static void mousePos_callback(GLFWwindow* window, double x, double y)
{

	s_mouePos.x = x; s_mouePos.y = y;
	vec2 offset = s_mouePos - s_lastMousePos;
	s_lastMousePos = s_mouePos;

	static const float factor = 0.001f; 
	offset *= factor;

	//PrintVec2("offset", lastMousePos);

	// get the axis to rotate around the x-axis. 
	vec3 Axis = cross(Rendering::Camera::viewVector - Rendering::Camera::cameraPos, vec3(0, 1, 0));
	// To be able to use the quaternion conjugate, the axis to
	// rotate around must be normalized.
	Axis = normalize(Axis);

	Rendering::Camera::RotateCamera(-offset.y, Axis.x, Axis.y, Axis.z);// rotate around local x axis
	Rendering::Camera::RotateCamera(-offset.x, 0, 1, 0);//rotate around local y axis

}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action != GLFW_RELEASE)
	{
		keyStates[key] = KEY_HELD;
	}
	else
	{
		keyStates[key] = KEY_RELEASED;
	}
}


static void mouse_callback(GLFWwindow* window, int key, int action, int mods)
{
	if (action != GLFW_RELEASE)
	{
		mouseState[key] = KEY_HELD;
	}
	else
	{
		mouseState[key] = KEY_RELEASED;
	}
}


bool GetKey(int key)
{
	return keyStates[key] == KEY_HELD;
}
bool GetMouse(int key)
{
	return mouseState[key] == KEY_HELD;
}

void InputUpdates()
{
	using namespace glm;
	float speed = 0.1f;

	if (GetKey(GLFW_KEY_LEFT_SHIFT))
		speed = 1.5f;
	vec3 forwarDir = Rendering::Camera::GetForwardDir();

	vec3 move(0, 0, 0);
	if (GetKey(GLFW_KEY_W))
	{
		move += Rendering::Camera::GetForwardDir() * speed;
	}
	if (GetKey(GLFW_KEY_S))
	{
		move -= Rendering::Camera::GetForwardDir() * speed;
	}
	if (GetKey(GLFW_KEY_A))
	{
		move -= Rendering::Camera::GetLeftDir() * speed;
	}
	if (GetKey(GLFW_KEY_D))
	{
		move += Rendering::Camera::GetLeftDir() * speed;
	}
	if (GetKey(GLFW_KEY_Q))
	{
		move -= vec3(0, 1, 0) * speed;
	}
	if (GetKey(GLFW_KEY_E))
	{
		move += vec3(0, 1, 0) * speed;
	}

	Rendering::Camera::cameraPos += move;
	Rendering::Camera::viewVector = Rendering::Camera::cameraPos + forwarDir * 10.0f;

	Rendering::Camera::Update();

	s_wireframeMode = false;
	if (GetKey(GLFW_KEY_K))
	{
		s_wireframeMode = true;
	}
}



// Called when the window is resized
void window_resized(GLFWwindow* window, int width, int height) {
	// Use red to clear the screen
	glClearColor(0, 0, 0, 1);

	// Set the viewport
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

#pragma endregion
#pragma region Applications

void Initilize()
{
	vec3 cameraPos(0, 200, 0);
	vec3 target(0, 100, -100);
	vec3 up(0, 1, 0);

	Rendering::Camera::SetLookAt(cameraPos, target,up);
	Rendering::Camera::SetPerspective(70, 4.0f / 3.0f, 0.1f, 1000.0f);
}



void Render()
{

}

//[  1  1  1  1           1  1  1  1       1  1  1  1  1  1  1  1 ]
int GetBlockArrayIndex(const int biasX, const int y, const int biasZ)
{
	const int k = 256;
	return biasX * k * 16 | biasZ * k | y;
}

void FillMeshPoints(Rendering::MeshPoints &meshPoints)
{
	using namespace Rendering;

	int *pHeightMap = new int[16 * 16];
	unsigned short *pData = new unsigned short[65536];

	int chunkCount = 10;

	std::vector<MeshPoints::Vertex> pointList; 
	for (int chunkX = -chunkCount; chunkX < chunkCount + 1; chunkX++)
	{
		for (int chunkZ = -chunkCount; chunkZ < chunkCount + 1; chunkZ++)
		{

			//GetHeightMapByChunk(chunkX, chunkZ, pHeightMap);

			vec3 startPoint(chunkX * 16, 0, chunkZ * 16);

			GetChunkByPos(chunkX, chunkZ, pData);

			for (int x = 0; x < 16; x++)
				for (int z = 0; z < 16; z++)
					for (int y = 0; y < 256; y++)
					{
						if (x + 1 == 16 || z + 1 == 16 || y + 1 == 256)
							continue;

						int index0 = GetBlockArrayIndex(x, y, z);

						int index1 = GetBlockArrayIndex(x + 1, y, z);

						int index2 = GetBlockArrayIndex(x, y + 1, z);

						int index3 = GetBlockArrayIndex(x + 1, y + 1, z);

						int index4 = GetBlockArrayIndex(x + 1, y, z + 1);

						int index5 = GetBlockArrayIndex(x, y + 1, z + 1);

						int index6 = GetBlockArrayIndex(x + 1, y + 1, z + 1);

						int index7 = GetBlockArrayIndex(x, y, z + 1);

						if (pData[index0] == 0)
							continue;

						if (pData[index1] == 0)
							continue;

						if (pData[index2] == 0)
							continue;

						if (pData[index3] == 0)
							continue;

						if (pData[index4] == 0)
							continue;

						if (pData[index5] == 0)
							continue;

						if (pData[index6] == 0)
							continue;

						if (pData[index7] == 0)
							continue;

						pData[index0] = 0;
						pData[index1] = 0;
						pData[index2] = 0;
						pData[index3] = 0;
						pData[index4] = 0;
						pData[index5] = 0;
						pData[index6] = 0;
						pData[index7] = 0;
					}

			for (int x = 0; x < 16; x++)
				for (int z = 0; z < 16; z++)
					for (int y = 0; y < 256; y++)
					{
						int index = GetBlockArrayIndex(x, y, z);
						if (pData[index] != 0)
						{
							vec3 point = startPoint + vec3(x, y, z);

							MeshPoints::Vertex vertex;
							vertex.position[0] = point.x;
							vertex.position[1] = point.y;
							vertex.position[2] = point.z;

							vec3 color;
							if (pData[index] == 40)//Dirt
							{
								color = vec3(0.72f, 0.5f, 0.18f);
							}
							else if (pData[index] == 42)//Grass
							{
								color = vec3(0.23f, 0.73f, 0.18f);
							}
							else if (pData[index] == 139)//Stone
							{
								color = vec3(0.65f, 0.65f, 0.51f);
							}
							else if (pData[index] == 41)//Snow
							{
								color = vec3(1.0f, 1.0f, 1.0f);
							}
							else if (pData[index] == 44)//Sand
							{
								color = vec3(1.0f, 1.0f, 0.0f);
							}
							else
								color = vec3(1.0f, 1.0f, 1.0f);

							vertex.color[0] = color.r;
							vertex.color[1] = color.g;
							vertex.color[2] = color.b;

							pointList.push_back(vertex);

							if (pData[index] == 54)// water?
							{
								color = vec3(0.0f, 0.8f, 1.0f);
								vertex.color[0] = color.r;
								vertex.color[1] = color.g;
								vertex.color[2] = color.b;

								int counter = 0;
								while (counter <= 1)
								{
									vertex.position[0] = point.x;
									vertex.position[1] = point.y - counter;
									vertex.position[2] = point.z;

									pointList.push_back(vertex);

									counter++;
								}
							}
						}

					}


		}

	}

	meshPoints.LoadFromPointList(pointList);
}

#pragma endregion

#pragma region Experiment

void MeshFilling(Rendering::Mesh &mesh,int chunkX,int chunkZ )
{
	using namespace Rendering;
	using namespace std;
	using namespace glm;


	vector<Vertex> vertices;


	vector<unsigned int>indices;
	

	
	unsigned short *pData = new unsigned short[65536];
	GetChunkByPos(chunkX, chunkZ, pData);
	GreedyMesshing greedyMeshing;


	vector<int*> chunk;
	for (int ty = 0; ty < 16; ty++)
	{
		int* tileData = new int[16 * 16 * 16];
		chunk.push_back(tileData);
	}

	for (int ty = 0; ty < 16; ty++)
	{

		int* tileData = chunk[ty];

		for (int x = 0; x < 16; x++)
			for (int z = 0; z < 16; z++)
				for (int y = 0; y < 16; y++)
				{
					int chunkY = 16 * ty + y;
					if (pData[GetBlockArrayIndex(x, chunkY, z)] != 0)//TODO: different Block ID
						tileData[x * 16 * 16 + y * 16 + z] =  pData[GetBlockArrayIndex(x, chunkY, z)];
					else
						tileData[x * 16 * 16 + y * 16 + z] = 0;
				}
	}
	
	for (int i = 0; i < chunk.size(); i++)
	{
	
		int* cubeData = chunk[i];


		
		greedyMeshing.GenerateMesh(cubeData, vertices, indices, glm::vec3(chunkX * 16, i * 16, chunkZ*16));
	}
	

	mesh.LoadFromList(vertices, indices);
	delete[]pData;
	for (int i = 0; i < chunk.size(); i++)
	{

		delete[] chunk[i];
	}
}

std::map<glm::vec2, Rendering::Mesh> s_LoadedChunks;
std::set<glm::vec2> s_requestingChunks;

void Update()
{}

#pragma endregion

#pragma region Cubeworld


std::string LoadAssetFile(const std::string& assetPath, bool binary = false)
{
	char strExePath[MAX_PATH] = { 0 };
	char* strLastSlash = NULL;
	GetModuleFileName(NULL, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;

	// 再砍一层目录
	strLastSlash = _tcsrchr(strExePath, L'\\');
	if (strLastSlash)
		*strLastSlash = 0;

	char blockInfoPath[MAX_PATH] = { 0 };
	strcpy(blockInfoPath, strExePath);
	strcat(blockInfoPath, assetPath.c_str());

	if (binary)
	{
		std::ifstream blockInfoStream(blockInfoPath, std::ios::binary);
		std::string blockInfoContent((std::istreambuf_iterator<char>(blockInfoStream)), std::istreambuf_iterator<char>());
		return blockInfoContent;
	}
	else
	{
		std::ifstream blockInfoStream(blockInfoPath);
		std::string blockInfoContent((std::istreambuf_iterator<char>(blockInfoStream)), std::istreambuf_iterator<char>());
		return blockInfoContent;
	}
}

void LoadAssetFilesInDir(const std::string& assetDir, const char* pattern, std::vector<std::string>& outFileContents, bool binary = false)
{
	char strExePath[MAX_PATH] = { 0 };
	char* strLastSlash = NULL;
	GetModuleFileName(NULL, strExePath, MAX_PATH);
	strExePath[MAX_PATH - 1] = 0;

	// 再砍一层目录
	strLastSlash = _tcsrchr(strExePath, L'\\');
	if (strLastSlash)
		*strLastSlash = 0;

	char blockInfoPath[MAX_PATH] = { 0 };
	strcpy(blockInfoPath, strExePath);
	strcat(blockInfoPath, assetDir.c_str());
	strcat(blockInfoPath, pattern);

	outFileContents.clear();

	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(blockInfoPath, &findFileData);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if (0 == (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				const char* filename = findFileData.cFileName;

				strcpy(blockInfoPath, strExePath);
				strcat(blockInfoPath, assetDir.c_str());
				strcat(blockInfoPath, "/");
				strcat(blockInfoPath, filename);

				if (binary)
				{
					std::ifstream blockInfoStream(blockInfoPath, std::ios::binary);
					std::string content((std::istreambuf_iterator<char>(blockInfoStream)), std::istreambuf_iterator<char>());
					outFileContents.push_back(content);
				}
				else
				{
					std::ifstream blockInfoStream(blockInfoPath);
					std::string content((std::istreambuf_iterator<char>(blockInfoStream)), std::istreambuf_iterator<char>());
					outFileContents.push_back(content);
				}
			}
		} while (FindNextFile(hFind, &findFileData));
		FindClose(hFind);
	}
}

void __stdcall MyErrorReportCallback(const char* filename, AGE::int32 line, const char* msg)
{
	printf("Error: %s\nFile = %s, line = %d", msg, filename, line);
}

void CubeWorldInit(const std::string& modName, const std::string& worldName)
{
	printf("\n------------------------------- init -------------------------------\n");

	InitEngineSystem("/");
	RegisterErrorReportCallBack(MyErrorReportCallback);

	BeforeLoadMods();

	std::string modsDir = "/Assets/Resources/Mods";

	std::string blockidMappingXml = LoadAssetFile(modsDir + "/BlockIDMapping.xml");
	std::string itemidMappingXml = LoadAssetFile(modsDir + "/ItemIDMapping.xml");
	BIDM_PrepareLoadMod(BIDOM_Runtime, blockidMappingXml.c_str(), itemidMappingXml.c_str());

	std::string genericBlockGroupsXml = LoadAssetFile(modsDir + "/" + "Internal" + "/GenericBlockGroups.xml");
	std::string genericShapeGroupsXml = LoadAssetFile(modsDir + "/" + "Internal" + "/GenericShapeGroups.xml");
	std::string genericMaterialGroupsXml = LoadAssetFile(modsDir + "/" + "Internal" + "/GenericMaterialGroups.xml");
	LoadMods(genericBlockGroupsXml.c_str(), genericShapeGroupsXml.c_str(), genericMaterialGroupsXml.c_str(), "", "", "");

	std::vector<std::string> bgTables;
	std::vector<std::string> sgTables;
	std::vector<std::string> mgTables;
	std::vector<std::string> biTables;
	LoadAssetFilesInDir(modsDir + "/" + modName + "/BlockGroups/", "*.xml", bgTables);
	LoadAssetFilesInDir(modsDir + "/" + modName + "/ShapeGroups/", "*.xml", sgTables);
	LoadAssetFilesInDir(modsDir + "/" + modName + "/MaterialGroups/", "*.xml", mgTables);
	LoadAssetFilesInDir(modsDir + "/" + modName + "/BiomeGenerators/", "*.xml", biTables);

	for (int i = 0; i < sgTables.size(); i++)
		LoadMods("", sgTables[i].c_str(), "", "", "", "");
	for (int i = 0; i < mgTables.size(); i++)
		LoadMods("", "", mgTables[i].c_str(), "", "", "");
	for (int i = 0; i < bgTables.size(); i++)
		LoadMods(bgTables[i].c_str(), "", "", "", "", "");
	for (int i = 0; i < biTables.size(); i++)
		LoadMods("", "", "", "", biTables[i].c_str(), "");

	InitAfterAllModLoaded();

	std::string BiomeGeneratorXml = LoadAssetFile(modsDir + "/" + modName + "/Worlds/" + worldName + "/BiomeGenerators/BiomeGenerator.xml");
	std::string DecoratorXml = LoadAssetFile(modsDir + "/" + modName + "/Worlds/" + worldName + "/Decorator.xml");

	int seed = 0;
	InitBiomeGenerator(BiomeGeneratorXml.c_str(), seed);
	SetDecoratorConfig(seed, DecoratorXml.c_str());
	CV_CalculateVillagesPos();

}



#pragma endregion



int main() {

	

	CubeWorldInit("Base", "Skyland");

	// Start GL context and O/S window using the GLFW helper library
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// Create a window
	window = glfwCreateWindow(ScreenWidth, ScreenHeight, "This is not a game engine!", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	
	// Bind input callbacks
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, mousePos_callback);
	glfwSetWindowSizeCallback(window, window_resized);

	// used for camera control
	glfwSetCursorPos(window, ScreenWidth / 2, ScreenHeight/2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
	
	//Rendering::MeshPoints meshPoints;
	//FillMeshPoints(meshPoints);

	std::vector<Rendering::Mesh*> meshes;
	{
		int chunkCount = 10;
		for (int chunkX = -chunkCount; chunkX < chunkCount ; chunkX++)
		{
			for (int chunkZ = -chunkCount; chunkZ < chunkCount; chunkZ++)
			{
				//Rendering::Mesh* pMesh = new Rendering::Mesh();
				//MeshFilling(*pMesh,chunkX,chunkZ);
				//meshes.push_back(pMesh);
			}
		}
	}
	
	//// Texture experiment
	//Rendering::Mesh texturedMesh;
	//{
	//	using namespace Rendering;
	//	using namespace std;
	//	using namespace glm;
	//	vector<Vertex> vertices;
	//	vector<unsigned int>indices;


	//	int texTileID = 246;
	//	vec2 texTileOffset = vec2(texTileID % 16,texTileID / 16);
	//	vec2 quadSize(10,12);
	//	

	//	Vertex index0;
	//	index0.Set(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec4(texTileOffset.x, texTileOffset.y, 0, 0));


	//	Vertex index1;
	//	index1.Set(vec3(0.0f, quadSize.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec4(texTileOffset.x, texTileOffset.y, 0, quadSize.y));


	//	Vertex index2;
	//	index2.Set(vec3(quadSize.x, quadSize.y, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec4(texTileOffset.x, texTileOffset.y, quadSize.x, quadSize.y));


	//	Vertex index3;
	//	index3.Set(vec3(quadSize.x, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec4(texTileOffset.x, texTileOffset.y, quadSize.x,0));

	//	vertices.push_back(index0);
	//	vertices.push_back(index1);
	//	vertices.push_back(index2);
	//	vertices.push_back(index3);

	//	indices.push_back(3); indices.push_back(1); indices.push_back(0);

	//	indices.push_back(3); indices.push_back(2); indices.push_back(1);

	//	texturedMesh.LoadFromList(vertices, indices);

	//}
	Rendering::Texture texture;
	texture.LoadFromPath("Images/terrain.png");

	Rendering::Material material;
	material.Init("Shaders/vert_regular.shader", "Shaders/frag_atlas.shader");
	material.SetTexture(&texture);

	Initilize();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glPolygonMode(GL_FRONT, GL_LINE);
	float counter = 0.0f; World world; world.Intilize(); 
	while (!glfwWindowShouldClose(window))
	{
		world.Update(1);

		InputUpdates(); 
		
		material.Activate();

		// Normal GL draw methods
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//meshPoints.Draw();
		//mesh.Draw();

		world.Draw();
		if (s_wireframeMode == true)
		{
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}

		
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->Draw();
		}

		

		glEnable(GL_PROGRAM_POINT_SIZE);

		// update other events like input handling 
		glfwPollEvents();
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();

	world.ShutDown();
	
	_CrtDumpMemoryLeaks();

	return 0;
}