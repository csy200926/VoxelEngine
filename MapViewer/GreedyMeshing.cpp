#include "GreedyMeshing.h"

namespace Rendering
{
	/*
				Z = 0,
				NZ = 1,
				NY = 2,
				Y = 3,
				NX = 4,
				X = 5

				y+
				|   /z-
				|  /
				| /
				|/______ x+

	*/
	glm::vec3 GreedyMesshing::neighborIndex6Side[6] = { 
		glm::vec3(0, 0, -1) ,//TODO: why doesn't need to adjust???
		glm::vec3(0, 0, 1) ,//
		glm::vec3(0, -1, 0) ,
		glm::vec3(0, 1, 0) ,
		glm::vec3(-1, 0, 0) ,
		glm::vec3(1, 0, 0) };

	glm::vec3 GreedyMesshing::Offsets_AO[72] = {
		// NZ face
		glm::vec3(0, -1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 0, -1),

		glm::vec3(-1, 0, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(0, 1, -1),

		glm::vec3(0, 1, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(1, 0, -1),

		glm::vec3(1, 0, -1),
		glm::vec3(1, -1, -1),
		glm::vec3(0, -1, -1),

		// Z face
		glm::vec3(0, -1, 1),
		glm::vec3(-1, -1, 1),
		glm::vec3(-1, 0, 1),

		glm::vec3(-1, 0, 1),
		glm::vec3(-1, 1, 1),
		glm::vec3(0, 1, 1),

		glm::vec3(0, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 0, 1),

		glm::vec3(1, 0, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(0, -1, 1),

		// NY face
		glm::vec3(0, -1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1, 0),

		glm::vec3(-1, -1, 0),
		glm::vec3(-1, -1, 1),
		glm::vec3(0, -1, 1),

		glm::vec3(0, -1, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, -1, 0),

		glm::vec3(1, -1, 0),
		glm::vec3(1, -1, -1),
		glm::vec3(0, -1, -1),

		// Y face
		glm::vec3(0, 1, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(-1, 1, 0),

		glm::vec3(-1, 1, 0),
		glm::vec3(-1, 1, 1),
		glm::vec3(0, 1, 1),

		glm::vec3(0, 1, 1),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 1, 0),

		glm::vec3(1, 1, 0),
		glm::vec3(1, 1, -1),
		glm::vec3(0, 1, -1),

		// NX face
		glm::vec3(-1, -1, 0),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, 0, -1),

		glm::vec3(-1, 0, -1),
		glm::vec3(-1, 1, -1),
		glm::vec3(-1, 1, 0),

		glm::vec3(-1, 1, 0),
		glm::vec3(-1, 1, 1),
		glm::vec3(-1, 0, 1),

		glm::vec3(-1, 0, 1),
		glm::vec3(-1, -1, 1),
		glm::vec3(-1, -1, 0),

		// X face
		glm::vec3(1, -1, 0),
		glm::vec3(1, -1, -1),
		glm::vec3(1, 0, -1),

		glm::vec3(1, 0, -1),
		glm::vec3(1, 1, -1),
		glm::vec3(1, 1, 0),

		glm::vec3(1, 1, 0),
		glm::vec3(1, 1, 1),
		glm::vec3(1, 0, 1),

		glm::vec3(1, 0, 1),
		glm::vec3(1, -1, 1),
		glm::vec3(1, -1, 0),
	};

	GreedyMesshing::GreedyMesshing()
	{
		using namespace glm;
		normalDirs[0] = vec3(0, 0, 1);
		normalDirs[1] = vec3(0, 0, -1);
		normalDirs[2] = vec3(0, -1, 0);
		normalDirs[3] = vec3(0, 1, 0);
		normalDirs[4] = vec3(0, 0,-1);
		normalDirs[5] = vec3(0, 0, 1);

		for (int i = 0; i < 26; i++)
		{
			NeightborOffsetIndexMap.insert(std::pair<vec3, int>(Tile::neighborOffsets_26[i], i));
		}
	}


	GreedyMesshing::~GreedyMesshing()
	{
	}

	void GreedyMesshing::GenerateMesh(Tile* i_pTile
		, std::vector<Vertex> &io_vertices
		, std::vector<unsigned int>&io_indices
		, glm::vec3& i_worldPos)
	{
		if (i_pTile == nullptr)
		{
			return;
		}

		World *pWorld = World::GetInstance();
		int coord0 = 0, coord1 = 1, coord2 = 2;
		for (int i = 0; i < 6; i++)
		{
			int *pNeighborDate = nullptr;
			{

				int index = NeightborOffsetIndexMap[neighborIndex6Side[i]];
				if (i_pTile->pNeighbors[index] != nullptr)
				{
					pNeighborDate = i_pTile->pNeighbors[index]->pData;//pTile->pData;
				}
				
			}
			
			QuadDir dir = (QuadDir)i;//0 1 2

			GenerateQuads(coord0, coord1, coord2, dir, i % 2 == 0, i_pTile, pNeighborDate, i_worldPos, io_vertices, io_indices);

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
		Tile *i_pTile,
		int *i_pTileDataNeighbor,
		glm::vec3& worldPos,
		std::vector<Vertex> &io_vertices,
		std::vector<unsigned int>&io_indices)
	{
		using namespace glm;
		int dir_int = (int)dir;
		int coord[3];// = new int[3];
		int *i_pTileData = i_pTile->pData; 

		float offset[] = {-0.5f, -0.5f, -0.5f};
		if (isBackFace == false)
			offset[coordZ] = 0.5f;
		vec3 iterateOffset = vec3(offset[0], offset[1], offset[2]) + worldPos;

		//x * 16 * 16 + y * 16 + z
		{
			if (i_pTileDataNeighbor != nullptr)
			{
				int coord_local[3] = {0,0,0};
				
				if (isBackFace == true)
					coord_local[coordZ] = 15;
				else
					coord_local[coordZ] = 0;

				for (coord_local[coordX] = 0; coord_local[coordX] < 16; coord_local[coordX]++)
					for (coord_local[coordY] = 0; coord_local[coordY] < 16; coord_local[coordY]++)
					{
						lastMask[coord_local[coordX] * 16 + coord_local[coordY]] = i_pTileDataNeighbor[coord_local[0] * 16 * 16 + coord_local[1] * 16 + coord_local[2]];
					}

			}
			else
			{
				for (int i = 0; i < 16 * 16; i++)
				{
					lastMask[i] = 0;
				}
			}
	
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

			int AOValue = 0, lastAOValue = 0;

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
							
							AOValue = 0;
							lastAOValue = INT_MAX;
							for (localCoor[coordY] = startPosArray[coordY]; localCoor[coordY] < 16; localCoor[coordY]++)
							{
								int index_local = localCoor[0] * 16 * 16 + localCoor[1] * 16 + localCoor[2];
								int data_local = i_pTileData[index_local];
								
								// Calculate AO
								if (data_local != 0)
								{
									AOValue = 0;
									for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
									{

										int neighborsInfo[] = { 0, 0, 0 };
										int cornerOffsetCounter = 0;
										while (cornerOffsetCounter < 3)
										{
											int offsetIndex = dir_int * 12 + cornerIndex * 3 + cornerOffsetCounter;
											vec3 offset_AO = Offsets_AO[offsetIndex];

											int index_local_AO =
												(localCoor[0] + (int)offset_AO.x) * 16 * 16 +
												(localCoor[1] + (int)offset_AO.y) * 16 +
												(localCoor[2] + (int)offset_AO.z);

											int data_local_AO = GetTileDataByIndex(
												localCoor[0] + (int)offset_AO.x,
												localCoor[1] + (int)offset_AO.y,
												localCoor[2] + (int)offset_AO.z,
												i_pTile);

											neighborsInfo[cornerOffsetCounter] = data_local_AO == 0 ? 0 : 1;

											cornerOffsetCounter++;
										}
										int cornerAO = AOLevel(neighborsInfo[0], neighborsInfo[2], neighborsInfo[1]);
										AOValue |= cornerAO << (8 * cornerIndex);
									}
								}
								else
									AOValue = INT_MAX;

								if (height != 0
									&& AOValue != lastAOValue)
								{
									AOValue = lastAOValue;
									break;
								}
								else
									lastAOValue = AOValue;
								
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
								AOValue = 0;
								lastAOValue = INT_MAX;
								int localCoor[3] = { coord[0], coord[1], coord[2] };
								for (localCoor[coordX] = startPosArray[coordX]; localCoor[coordX] < 16; localCoor[coordX]++)
								{
									bool stopSign = false;
									for (localCoor[coordY] = startPosArray[coordY]; localCoor[coordY] < startPosArray[coordY] + height; localCoor[coordY]++)
									{
										int index_local = localCoor[0] * 16 * 16 + localCoor[1] * 16 + localCoor[2];
										int data_local = i_pTileData[index_local];

										// Calculate AO
										if (data_local != 0)
										{
											AOValue = 0;
											for (int cornerIndex = 0; cornerIndex < 4; cornerIndex++)
											{

												int neighborsInfo[] = { 0, 0, 0 };
												int cornerOffsetCounter = 0;
												while (cornerOffsetCounter < 3)
												{
													int offsetIndex = dir_int * 12 + cornerIndex * 3 + cornerOffsetCounter;
													vec3 offset_AO = Offsets_AO[offsetIndex];

													int index_local_AO =
														(localCoor[0] + (int)offset_AO.x) * 16 * 16 +
														(localCoor[1] + (int)offset_AO.y) * 16 +
														(localCoor[2] + (int)offset_AO.z);

													int data_local_AO = GetTileDataByIndex(
														localCoor[0] + (int)offset_AO.x,
														localCoor[1] + (int)offset_AO.y,
														localCoor[2] + (int)offset_AO.z,
														i_pTile);

													neighborsInfo[cornerOffsetCounter] = data_local_AO == 0 ? 0 : 1;

													cornerOffsetCounter++;
												}
												int cornerAO = AOLevel(neighborsInfo[0], neighborsInfo[2], neighborsInfo[1]);
												AOValue |= cornerAO << (8 * cornerIndex);
											}
										}
										else
											AOValue = INT_MAX;

										if (width != 0 && AOValue != lastAOValue)
										{
											stopSign = true;
										}
										else
											lastAOValue = AOValue;

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
							GenerateAlignedQuad(min + iterateOffset, max + iterateOffset, width, height, data, dir, io_vertices, io_indices, lastAOValue);

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
			, std::vector<unsigned int>&io_indices
			, int AOValue)
	{
		using namespace glm;
		Vertex vertex;
		if (texID > 255)
		{
			texID = 0;
		}
		if (texID == 42)
		{
			texID = 0;
		}
		else if (texID == 40)
		{
			texID = 2;
		}
		else if (texID == 139)
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

			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_1);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}


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
			

			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_1);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}

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
			

			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_3);

				// Triangle_2
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}

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
			
			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_3);

				// Triangle_2
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}


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
			

			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_0);
				io_indices.push_back(index_1);
				io_indices.push_back(index_3);

				// Triangle_2
				io_indices.push_back(index_1);
				io_indices.push_back(index_2);
				io_indices.push_back(index_3);
			}

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
			

			int vertSize = io_vertices.size();
			int AOCounter = 0;
			int AOValues[4];
			while (AOCounter < 4)
			{
				unsigned char cornerAO = (unsigned char)(AOValue >> (8 * AOCounter));
				AOValues[AOCounter] = cornerAO;
				
				Vertex &vert = io_vertices[vertSize - 4 + AOCounter];
				vert.color[3] = cornerAO;
				AOCounter++;
			}
			if (AOValues[1] + AOValues[3] < AOValues[0] + AOValues[2])
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_0);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}
			else
			{
				// Triangle_1
				io_indices.push_back(index_3);
				io_indices.push_back(index_1);
				io_indices.push_back(index_0);

				// Triangle_2
				io_indices.push_back(index_3);
				io_indices.push_back(index_2);
				io_indices.push_back(index_1);
			}
			break;
		}




		}

	}
}

