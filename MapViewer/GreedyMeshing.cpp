#include "GreedyMeshing.h"
namespace Rendering
{
	GreedyMesshing::GreedyMesshing()
	{
		using namespace glm;
		normalDirs[0] = vec3(0, 0, 1);
		normalDirs[1] = vec3(0, 0, -1);
		normalDirs[2] = vec3(0, -1, 0);
		normalDirs[3] = vec3(0, 1, 0);
		normalDirs[4] = vec3(0, 0,-1);
		normalDirs[5] = vec3(0, 0, 1);

	}


	GreedyMesshing::~GreedyMesshing()
	{
	}

	void GreedyMesshing::GenerateMesh(int *i_pTileData
		, std::vector<Vertex> &io_vertices
		, std::vector<unsigned int>&io_indices
		, glm::vec3& i_worldPos)
	{
		int coord0 = 0, coord1 = 1, coord2 = 2;
		for (int i = 0; i < 6; i++)
		{


			QuadDir dir = (QuadDir)i;//0 1 2

			GenerateQuads(coord0, coord1, coord2, dir, i % 2 == 0, i_pTileData, i_worldPos, io_vertices,io_indices);

			if (i % 2 != 0)
			{
				int coord0_pre = coord0, coord1_pre = coord1, coord2_pre = coord2;
				coord0 = coord2_pre;
				coord1 = coord0_pre;
				coord2 = coord1_pre;
			}

		}
	}

	bool GreedyMesshing::CheckCubeSur(int thisBlockID, int &thisMask, int lastMask)
	{
		if (thisMask != 0)
			return false;

		if ((thisBlockID != 0 && lastMask != 0)
			|| (thisBlockID == 0 && lastMask == 0))
		{
			if (thisBlockID != 0)
				thisMask = thisBlockID;
			return false;
		}
		else
		{
			if (thisBlockID == 0)
				return false;
			return true;
		}


	}

	void GreedyMesshing::GenerateQuads(
		int coordX,
		int coordY,
		int coordZ,
		QuadDir dir,
		bool isBackFace,
		int *i_pTileData,
		glm::vec3& worldPos,
		std::vector<Vertex> &io_vertices,
		std::vector<unsigned int>&io_indices)
	{
		using namespace glm;

		int coord[3];// = new int[3];

		float offset[] = {-0.5f, -0.5f, -0.5f};
		if (isBackFace == false)
			offset[coordZ] = 0.5f;
		vec3 iterateOffset = vec3(offset[0], offset[1], offset[2]) + worldPos;


		for (int i = 0; i < 16 * 16; i++)
		{
			lastMask[i] = 0;
		}

		int sliceCoord = 0;
		while (sliceCoord < 16)
		{
			if (isBackFace == false)
				coord[coordZ] = 15 - sliceCoord;
			else
				coord[coordZ] = sliceCoord;
			//Clear mask
			for (int i = 0; i < 16 * 16; i++)
			{
				thisMask[i] = 0;
			}

			for (coord[coordX] = 0; coord[coordX] < 16; coord[coordX]++)
				for (coord[coordY] = 0; coord[coordY] < 16; coord[coordY]++)
				{
					int index = coord[0] * 16 * 16 + coord[1] * 16 + coord[2];
					int data = i_pTileData[index];

					int startPosArray[3] = { coord[0], coord[1], coord[2] };
					if (data != 0)
					{
						int height = 0;
						{

							int localCoor[3] = { coord[0], coord[1], coord[2] };

							for (localCoor[coordY] = startPosArray[coordY]; localCoor[coordY] < 16; localCoor[coordY]++)
							{
								int index_local = localCoor[0] * 16 * 16 + localCoor[1] * 16 + localCoor[2];
								int data_local = i_pTileData[index_local];
								if (data_local == data&&CheckCubeSur(data_local, thisMask[localCoor[coordX] * 16 + localCoor[coordY]], lastMask[localCoor[coordX] * 16 + localCoor[coordY]]))
								{
									height++;
								}
								else
									break;
							}
						}


						int width = 0;
						{
							if (height != 0)
							{
								int localCoor[3] = { coord[0], coord[1], coord[2] };
								for (localCoor[coordX] = startPosArray[coordX]; localCoor[coordX] < 16; localCoor[coordX]++)
								{
									bool stopSign = false;
									for (localCoor[coordY] = startPosArray[coordY]; localCoor[coordY] < startPosArray[coordY] + height; localCoor[coordY]++)
									{
										int index_local = localCoor[0] * 16 * 16 + localCoor[1] * 16 + localCoor[2];
										int data_local = i_pTileData[index_local];

										if (data_local != data || !CheckCubeSur(data_local, thisMask[localCoor[coordX] * 16 + localCoor[coordY]], lastMask[localCoor[coordX] * 16 + localCoor[coordY]]))
										{
											stopSign = true;
											break;
										}

									}
									if (stopSign == true)
										break;
									width++;
								}
							}

						}
						if (height != 0 && width != 0)
						{
							int quadWH[3] = { 0, 0, 0 };
							quadWH[coordX] = width; quadWH[coordY] = height;

							vec3 startPos(coord[0], coord[1], coord[2]);
							vec3 min = startPos;
							vec3 max = startPos + vec3(quadWH[0], quadWH[1], quadWH[2]);
							GenerateAlignedQuad(min + iterateOffset, max + iterateOffset,width,height,data ,dir, io_vertices,io_indices);

							{
								int minArray[3] = { (int)min.x, (int)min.y, (int)min.z };
								int maxArray[3] = { (int)max.x, (int)max.y, (int)max.z };
								int localCoor[3]= { coord[0], coord[1], coord[2] };

								for (int maskX = minArray[coordX]; maskX < maxArray[coordX]; maskX++)
									for (int maskY = minArray[coordY]; maskY < maxArray[coordY]; maskY++)
									{
										localCoor[coordX] = maskX;
										localCoor[coordY] = maskY;
										int index_local = localCoor[0] * 16 * 16 + localCoor[1] * 16 + localCoor[2];

										thisMask[16 * maskX + maskY] = i_pTileData[index_local];
									}
							}


						}
						//Skip generated area
						if (height > 0)
							coord[coordY] += height - 1;
					}


				}

			//Set last mask
			for (int i = 0; i < 16 * 16; i++)
			{
				lastMask[i] = thisMask[i];
			}

			sliceCoord++;
		}

	}

	void GreedyMesshing::GenerateAlignedQuad(
			  glm::vec3& i_min
			, glm::vec3& i_max
			, int width
			, int height
			, int texID
			, QuadDir i_direction
			, std::vector<Vertex> &io_vertices
			, std::vector<unsigned int>&io_indices)
	{
		using namespace glm;
		Vertex vertex;
		if (texID>255)
		{
			texID = 1;
		}
		if (texID==42)
		{
			texID = 1;
		}else if (texID==40)
		{
			texID = 2;
		}else if (texID==139)
		{
			texID = 1;
		}
		vec2 texTileOffset = vec2(texID % 16,15 - texID / 16);

		switch (i_direction)
		{
		case NX:
		{
			vec3 leftTop(i_min.x, i_max.y, i_min.z);
			vec3 rightBtm(i_min.x, i_min.y, i_max.z);

			int temp = height;
			height = width;
			width = temp;

			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);
			
			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);
			

			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);

			// Triangle_1
			io_indices.push_back(index_3);
			io_indices.push_back(index_1);
			io_indices.push_back(index_0);

			// Triangle_2
			io_indices.push_back(index_3);
			io_indices.push_back(index_2);
			io_indices.push_back(index_1);

			break;
		}

		case NY:
		{
			vec3 leftTop(i_min.x, i_min.y, i_max.z);
			vec3 rightBtm(i_max.x, i_min.y, i_min.z);

			int temp = height;
			height = width;
			width = temp;

			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);

			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);


			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);
			

			// Triangle_1
			io_indices.push_back(index_3);
			io_indices.push_back(index_1);
			io_indices.push_back(index_0);

			// Triangle_2
			io_indices.push_back(index_3);
			io_indices.push_back(index_2);
			io_indices.push_back(index_1);
			break;
		}

		case Z:
		{
			vec3 leftTop(i_min.x, i_max.y, i_min.z);
			vec3 rightBtm(i_max.x, i_min.y, i_min.z);

			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);

			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);


			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);
			

			// Triangle_1
			io_indices.push_back(index_0);
			io_indices.push_back(index_1);
			io_indices.push_back(index_3);

			// Triangle_2
			io_indices.push_back(index_1);
			io_indices.push_back(index_2);
			io_indices.push_back(index_3);
			break;
		}
		case X:
		{
			vec3 leftTop(i_min.x, i_max.y, i_min.z);
			vec3 rightBtm(i_min.x, i_min.y, i_max.z);


			int temp = height;
			height = width;
			width = temp;

			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);

			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);


			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);
			

			// Triangle_1
			io_indices.push_back(index_0);
			io_indices.push_back(index_1);
			io_indices.push_back(index_3);

			// Triangle_2
			io_indices.push_back(index_1);
			io_indices.push_back(index_2);
			io_indices.push_back(index_3);

			break;
		}

		case Y:
		{
			vec3 leftTop(i_min.x, i_min.y, i_max.z);
			vec3 rightBtm(i_max.x, i_min.y, i_min.z);

			int temp = height;
			height = width;
			width = temp;

			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);

			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);


			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);
			

			// Triangle_1
			io_indices.push_back(index_0);
			io_indices.push_back(index_1);
			io_indices.push_back(index_3);

			// Triangle_2
			io_indices.push_back(index_1);
			io_indices.push_back(index_2);
			io_indices.push_back(index_3);
			break;
		}

		case NZ:
		{
			vec3 leftTop(i_min.x, i_max.y, i_min.z);
			vec3 rightBtm(i_max.x, i_min.y, i_min.z);


			unsigned int index_0 = io_vertices.size();
			vertex.Set(i_min, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, 0));
			io_vertices.push_back(vertex);

			unsigned int index_1 = io_vertices.size();
			vertex.Set(leftTop, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, 0, height));
			io_vertices.push_back(vertex);


			unsigned int index_2 = io_vertices.size();
			vertex.Set(i_max, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, height));
			io_vertices.push_back(vertex);

			unsigned int index_3 = io_vertices.size();
			vertex.Set(rightBtm, normalDirs[(int)i_direction], vec4(texTileOffset.x, texTileOffset.y, width, 0));
			io_vertices.push_back(vertex);
			

			// Triangle_1
			io_indices.push_back(index_3);
			io_indices.push_back(index_1);
			io_indices.push_back(index_0);

			// Triangle_2
			io_indices.push_back(index_3);
			io_indices.push_back(index_2);
			io_indices.push_back(index_1);
			break;
		}




		}

	}
}

