#include "Chunk.h"

#include "../GreedyMeshing.h"
#include "../Componenets/Mesh.h"
#include <vector>
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

void Chunk::UpdateNeighborInfo()
{

}

