#include "Chunk.h"

#include "../GreedyMeshing.h"
#include "../Componenets/Mesh.h"
#include <vector>

glm::vec2 Chunk::neighborOffsets[8] = { glm::vec2(-1, 1), glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(-1, 0)
									   ,glm::vec2(1, 0), glm::vec2(-1, -1), glm::vec2(0, -1), glm::vec2(1,-1) };
int Chunk::neighborIndexMutual[8] = {7 , 6 ,5 ,4 ,3 ,2 ,1 ,0};

void Chunk::GenerateMesh()
{
	using namespace std;
	using namespace Rendering;

	vector<Vertex> vertices;
	vector<unsigned int>indices;

	GreedyMesshing greedyMeshing;


	for (int i = 0; i < 16; i++)
	{
		greedyMeshing.GenerateMesh(pTiles[i]->pData, vertices, indices, glm::vec3(chunkID.x * 16, i * 16, chunkID.y * 16));
	}


	m_Mesh.LoadFromList(vertices, indices);
}

/*
neighborOffsets[0] = vec2(-1,1);
neighborOffsets[1] = vec2(0,1);
neighborOffsets[2] = vec2(1,1);
neighborOffsets[3] = vec2(-1,0);
neighborOffsets[4] = vec2(1,0);
neighborOffsets[5] = vec2(-1,-1);
neighborOffsets[6] = vec2(0,-1);
neighborOffsets[7] = vec2(1,-1);

neighborIndexMutual[0] = 7;
neighborIndexMutual[1] = 6;
neighborIndexMutual[2] = 5;
neighborIndexMutual[3] = 4;
neighborIndexMutual[4] = 3;
neighborIndexMutual[5] = 2;
neighborIndexMutual[6] = 1;
neighborIndexMutual[7] = 0;
*/
