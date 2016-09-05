#pragma once
#include <glm.hpp>
#include <vector>
#include "../Componenets/Mesh.h"
#include "../Utilities/Debugging.h"
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
	
	enum ChunkState
	{
		Loaded = 0,
		GeneratingMesh,
		AllSet,

	};

	Rendering::Mesh m_Mesh;

	Tile **pTiles; //16 length

	Chunk **pNeighbors;// 8 length neighbors

	glm::vec2 chunkID;


	ChunkState state;

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
		state = Loaded;
		pTiles = nullptr;
	}

	~Chunk()
	{
		for (int i = 0; i < 16; i++)
		{
			delete pTiles[i];
		}

		delete[] pTiles;
		delete[] pNeighbors;
	}

	void Initilize(glm::vec2 &i_chunkID)
	{
		chunkID = i_chunkID;
		pTiles = new Tile *[16];
		for (int i = 0; i < 16; i ++)
		{
			pTiles[i] = new Tile();
		}

		pNeighbors = new Chunk*[8];
		for (int i = 0; i < 8; i++)
		{
			pNeighbors[i] = nullptr;
		}
	}

	void Draw()
	{
		m_Mesh.Draw();
	}

	// Is activated for rendering
	// Only when all neighbors are generated can this chunk be loaded


	void GenerateMesh();
};