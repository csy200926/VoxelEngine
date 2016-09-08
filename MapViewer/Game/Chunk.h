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
	glm::vec3 tileID;
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

	/*
	__________
	|_0_|_1_|_2_|
	|_3_|_C_|_4_|
	|_5_|_6_|_7_|
	*/
	static glm::vec2 neighborOffsets[8];
	static int neighborIndexMutual[8];
	
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
			pTiles[i]->tileID = glm::vec3(chunkID.x,i,chunkID.y);
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

	void ClearRefTiNeighbors()
	{
		if (pNeighbors != nullptr)
		{
			for (int i = 0; i < 8; i++)
			{
				Chunk *pNeighbor = pNeighbors[i];
				if (pNeighbor != nullptr)
				{
					pNeighbor->pNeighbors[neighborIndexMutual[i]] = nullptr;

				}
			}



		}

	};
};