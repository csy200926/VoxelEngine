#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <glm.hpp>
#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include "BitMap/BitMap.h"
#include "detail/func_common.hpp"

#include "Utilities/Timing.h"
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


#include "Componenets/InputManager.h"
#include "Componenets/GUI.h"
#include "SDL/SDL.h"


using namespace AGE;

using namespace SYSR;
using namespace glm;


#pragma region static variables
GLFWwindow* window;
unsigned int ScreenHeight = 768*1.8f, ScreenWidth = 1024*1.8f;
bool isGameRunning = true;
#pragma endregion


#pragma region input Call backs

// Input helpers
bool s_wireframeMode = false;

static void mousePos_callback()//(GLFWwindow* window, double x, double y)
{
	using namespace glm;
	using namespace Rendering;
	InputManager *pInputManger = InputManager::GetInstance();

	int x = 0, y = 0;
	SDL_GetRelativeMouseState(&x,&y);
	vec2 offset(x, y);

	static const float factor = 0.001f; 
	offset *= factor;

	// get the axis to rotate around the x-axis. 
	vec3 Axis = cross(Rendering::Camera::viewVector - Rendering::Camera::cameraPos, vec3(0, 1, 0));
	// To be able to use the quaternion conjugate, the axis to
	// rotate around must be normalized.
	Axis = normalize(Axis);

	Rendering::Camera::RotateCamera(-offset.y, Axis.x, Axis.y, Axis.z);// rotate around local x axis
	Rendering::Camera::RotateCamera(-offset.x, 0, 1, 0);//rotate around local y axis

}

void InputUpdates()
{
	using namespace glm;
	using namespace Rendering;

	float speed = 0.1f;

	InputManager *pInputManger = InputManager::GetInstance();

	if (pInputManger->isKeyPressed(SDLK_LSHIFT))//if (GetKey(GLFW_KEY_LEFT_SHIFT))
		speed = 1.5f;
	vec3 forwarDir = Rendering::Camera::GetForwardDir();

	vec3 move(0, 0, 0);
	if (pInputManger->isKeyPressed(SDLK_w))//if (GetKey(GLFW_KEY_W))
	{
		move += Rendering::Camera::GetForwardDir() * speed;
	}
	if (pInputManger->isKeyPressed(SDLK_s))//if (GetKey(GLFW_KEY_S))
	{
		move -= Rendering::Camera::GetForwardDir() * speed;
	}
	if (pInputManger->isKeyPressed(SDLK_a))//if (GetKey(GLFW_KEY_A))
	{
		move -= Rendering::Camera::GetLeftDir() * speed;
	}
	if (pInputManger->isKeyPressed(SDLK_d))//if (GetKey(GLFW_KEY_D))
	{
		move += Rendering::Camera::GetLeftDir() * speed;
	}
	if (pInputManger->isKeyPressed(SDLK_q))//if (GetKey(GLFW_KEY_Q))
	{
		move -= vec3(0, 1, 0) * speed;
	}
	if (pInputManger->isKeyPressed(SDLK_e))//if (GetKey(GLFW_KEY_E))
	{
		move += vec3(0, 1, 0) * speed;
	}

	Rendering::Camera::cameraPos += move;
	Rendering::Camera::viewVector = Rendering::Camera::cameraPos + forwarDir * 10.0f;

	Rendering::Camera::Update();

	s_wireframeMode = false;
	if (pInputManger->isKeyPressed(SDLK_k))//if (GetKey(GLFW_KEY_K))
	{
		s_wireframeMode = true;
	}
	if (pInputManger->isKeyPressed(SDLK_ESCAPE))//if (GetKey(GLFW_KEY_K))
	{
		isGameRunning = false;
	}
	mousePos_callback();
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



int main(int argc, char** argv) {

	
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		SDL_Window *window = SDL_CreateWindow("STAY or TO GO", 200, 200, ScreenWidth, ScreenHeight, SDL_WINDOW_OPENGL);
		SDL_GLContext glContext = SDL_GL_CreateContext(window);
		
		CubeWorldInit("Base", "Skyland");

		// Start GL context and O/S window using the GLFW helper library
		if (!glfwInit()) {
			fprintf(stderr, "ERROR: could not start GLFW3\n");
			return 1;
		}

		// start GLEW extension handler
		glewExperimental = GL_TRUE;
		glewInit();
	
		Rendering::Texture texture;
		texture.LoadFromPath("Images/terrain.png");

		Rendering::Material material;
		material.Init("Shaders/vert_regular.shader", "Shaders/frag_atlas.shader");
		material.SetTexture(&texture);

		Rendering::Material material_linemode;
		material_linemode.Init("Shaders/vert_linemode.shader", "Shaders/frag_linemode.shader");

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		float timer = 0.0f;
	
		World world;
		world.Intilize(); 
		Utilities::Timing timing;
		timing.init(60);

		//Input stuff
		Rendering::InputManager inputManager;
		SDL_Event inputEvent;

		SDL_SetRelativeMouseMode(SDL_TRUE);
		while (isGameRunning)//(!glfwWindowShouldClose(window))
		{
			timing.begin();

			float delta = Utilities::Timing::getDelta();
			timer += delta;

			//Will keep looping until there are no more events to process
			while (SDL_PollEvent(&inputEvent)) {
				switch (inputEvent.type) {
				case SDL_QUIT:
					isGameRunning = false;
					break;
				case SDL_MOUSEMOTION:
					inputManager.setMouseCoords((float)inputEvent.motion.x, (float)inputEvent.motion.y);
					break;
				case SDL_KEYDOWN:
					inputManager.pressKey(inputEvent.key.keysym.sym);
					break;
				case SDL_KEYUP:
					inputManager.releaseKey(inputEvent.key.keysym.sym);
					break;
				case SDL_MOUSEBUTTONDOWN:
					inputManager.pressKey(inputEvent.button.button);
					break;
				case SDL_MOUSEBUTTONUP:
					inputManager.releaseKey(inputEvent.button.button);
					break;
				}
			}
		

			// Normal GL draw methods
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			if (s_wireframeMode == true)
			{
				glEnable(GL_POLYGON_OFFSET_LINE);
				glPolygonOffset(-1, -1);
				material_linemode.Activate();
				glPolygonMode(GL_FRONT, GL_LINE);
				world.Draw();
				glPolygonMode(GL_FRONT, GL_FILL);
				glDisable(GL_POLYGON_OFFSET_LINE);
				
			}

			material.Activate();
			


			world.Draw();

			glEnable(GL_PROGRAM_POINT_SIZE);

			SDL_GL_SwapWindow(window);

			world.Update(delta);

			if (timer > 0.03f)
			{
				timer = 0.0f;
				InputUpdates();
			}
			
			
			timing.end();
		}

		world.ShutDown();

		// close GL context and any other GLFW resources
		glfwTerminate();

	
	}
	
	_CrtDumpMemoryLeaks();

	return 0;
}