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
			Tile *i_pTile,
			int *i_pTileDataNeighbor,// this neighbor used for mesh merging only
			glm::vec3& worldPos,
			std::vector<Vertex> &io_vertices,
			std::vector<unsigned int>&io_indices);

		int AOLevel(int side1,int side2,int corner)
		{
			if (side1 != 0 && side2 != 0)
			{
				return 3;
			}
			return (side1 + side2 + corner);
		}

		int GetTileDataByIndex(int x_local, int y_local, int z_local, Tile *i_pTile)
		{
			using namespace glm;
			vec3 offset(0,0,0);

			if (x_local < 0)
			{
				offset.x--;
				x_local = 15;
			}
			else if (x_local > 15)
			{
				offset.x++;
				x_local = 0;
			}
				

			if (y_local < 0)
			{
				offset.y--;
				y_local = 15;
			}
				
			if (y_local > 15)
			{
				offset.y++;
				y_local = 0;
			}
				

			if (z_local < 0)
			{
				offset.z--;
				z_local = 15;
			}
				
			if (z_local > 15)
			{
				offset.z++;
				z_local = 0;
			}
				

			if (offset.x == 0 && offset.y == 0 && offset.z == 0)
			{
				return i_pTile->pData[x_local * 16 * 16 + y_local * 16 + z_local];
			}
			else
			{
				int index = NeightborOffsetIndexMap[offset];
				if (i_pTile->pNeighbors[index] == 0)
					return 0;
				return i_pTile->pNeighbors[index]->pData[x_local * 16 * 16 + y_local * 16 + z_local];
			}

			//if (x_local < 0 || x_local > 15
			//	|| y_local < 0 || y_local > 15
			//	|| z_local < 0 || z_local > 15)
			//	return 0;

	
		}
	};



}
