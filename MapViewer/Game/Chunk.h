#pragma once
#include <glm.hpp>
#include <memory.h>
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif
class Tile
{
public:
	Tile *pNeighbors;
	int *pData;//4096 length

	Tile()
	{
		pNeighbors = nullptr;
		pData = new int[4096];

		memset(pData, 1, sizeof(int) * 4096);
	}

	~Tile()
	{
		if (pData != nullptr)
		{
			delete[] pData;
		}

	}
};
namespace Rendering
{
	class Mesh;
}


class Chunk
{
public:
	Rendering::Mesh* pMesh;
	Tile **pTiles; //16 length

	glm::vec2 chunkID;
	Tile* GetTileByID(int ID)
	{
		if (ID < 16 && ID >= 0)
		{
			return pTiles[ID];
		}

		return nullptr;
	}

	Chunk()
	{

	}

	~Chunk()
	{
		for (int i = 0; i < 16; i++)
		{
			delete pTiles[i];
		}
		delete pMesh;
		delete[] pTiles;
	}

	void Initilize(glm::vec2 &i_chunkID)
	{
		chunkID = i_chunkID;
		pTiles = new Tile *[16];
		for (int i = 0; i < 16; i ++)
		{
			pTiles[i] = new Tile();
		}
	}

	void UpdateNeighborInfo();

	void GenerateMesh();
};