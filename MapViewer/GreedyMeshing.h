#pragma once
#include <vector>
#include <glm.hpp>

#include "Componenets/Mesh.h"
#include "Game/World.h"

#include <map>
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
			, std::vector<unsigned int>&io_indices
			, int AOValue);
	private:
		int lastMask[16 * 16];
		int thisMask[16 * 16];


		glm::vec3 normalDirs[6];
		static glm::vec3 Offsets_AO[72];
		static glm::vec3 neighborIndex6Side[6];
		struct vec3_cmp {
			inline bool operator()(const glm::vec3 &l, const glm::vec3 &r)
			{
				if (l.x < r.x)
				{
					return true;
				}
				else if (l.x == r.x)
				{
					if (l.y < r.y)
					{
						return true;
					}
					else if (l.y == r.y)
					{
						return l.z < r.z;
					}
				}
				return false;
			}
		};
		std::map<glm::vec3, int,vec3_cmp> NeightborOffsetIndexMap;

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

		int AOLevel(int side1,int side2,int corner)
		{
			if (side1 != 0 && side2 != 0)
			{
				return 0;
			}
			return 3 - (side1 + side2 + corner);
		}

		int GetTileDataByIndex(int x_local, int y_local, int z_local, int*data)
		{
			if (x_local < 0 || x_local > 15
				|| y_local < 0 || y_local > 15
				|| z_local < 0 || z_local > 15)
				return 0;

			return data[x_local * 16 * 16 + y_local * 16 + z_local];
		}
	};



}
