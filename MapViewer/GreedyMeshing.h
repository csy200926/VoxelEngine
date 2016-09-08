#pragma once
#include <vector>
#include <glm.hpp>

#include "Componenets/Mesh.h"
#include "Game/World.h"
namespace Rendering
{

	class GreedyMesshing
	{
	public:
		enum QuadDir
		{
			Z = 0,
			NZ = 1,
			NY = 2,
			Y = 3,
			NX = 4,
			X = 5
		};


		GreedyMesshing();
		~GreedyMesshing();

		// Meshing as a tile(16 * 16 * 16)
		void GenerateMesh(Tile* i_pTile
			,std::vector<Vertex> &io_vertices
			,std::vector<unsigned int>&io_indices
			,glm::vec3& worldPos);

		void GenerateAlignedQuad(
			glm::vec3& i_min
			, glm::vec3& i_max
			, int width
			, int height
			, int texID
			, QuadDir i_direction
			, std::vector<Vertex> &io_vertices
			, std::vector<unsigned int>&io_indices);
	private:
		int lastMask[16 * 16];
		int thisMask[16 * 16];

		glm::vec3 normalDirs[6];
		
		static glm::vec3 neighborIndex6Side[6];

		bool CheckCubeSur(int thisBlockID, int &thisMask, int lastMask);

		void GenerateQuads(
			int coordX, 
			int coordY, 
			int coordZ, 
			QuadDir dir, 
			bool isBackFace, 
			int *i_pTileData,
			int *i_pTileDataNeighbor,
			glm::vec3& worldPos,
			std::vector<Vertex> &io_vertices,
			std::vector<unsigned int>&io_indices);



	};



}
