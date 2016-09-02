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

#include "Componenets/Camera.h"
#include "Componenets/MeshPoints.h"
#include "Componenets/Material.h"
#include "Componenets/Mesh.h"
#include "Componenets/Texture.h"
#include "GreedyMeshing.h"
#include "Game/World.h"

#include "core/stdafx.h"
#include "IOAndRes/agoc/CubeTileData.h"
#include "IOAndRes/agoc/cubeworld_interface.h"
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

#include <windows.h>



using namespace AGE;

using namespace SYSR;
using namespace glm;


#pragma region static variables
GLFWwindow* window;
unsigned int ScreenHeight = 768, ScreenWidth = 1024;

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

	
	{

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
	
		Rendering::Texture texture;
		texture.LoadFromPath("Images/terrain.png");

		Rendering::Material material;
		material.Init("Shaders/vert_regular.shader", "Shaders/frag_atlas.shader");
		material.SetTexture(&texture);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		float counter = 0.0f; 
	
		World world;

		world.Intilize(); 

		while (!glfwWindowShouldClose(window))
		{
			world.Update(1);

			InputUpdates(); 
		
			material.Activate();

			// Normal GL draw methods
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			world.Draw();

			if (s_wireframeMode == false)
			{
				glPolygonMode(GL_FRONT, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT, GL_FILL);
			}
		

			glEnable(GL_PROGRAM_POINT_SIZE);

			// update other events like input handling 
			glfwPollEvents();
			// put the stuff we've been drawing onto the display
			glfwSwapBuffers(window);
		}

		world.ShutDown();

		// close GL context and any other GLFW resources
		glfwTerminate();

	
	}
	
	_CrtDumpMemoryLeaks();

	return 0;
}