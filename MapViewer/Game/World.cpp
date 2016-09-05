#include "World.h"
#include "Chunk.h"

#include "core/stdafx.h"
#include "IOAndRes/agoc/CubeTileData.h"
#include "IOAndRes/agoc/cubeworld_interface.h"

#include "../Componenets/Camera.h"
#include "../Componenets/Mesh.h"

#include <thread>
#include <algorithm>

World* World::pInstance = nullptr;
World* World::GetInstance()
{
	//TODO: Check
	//if (pInstance != nullptr)
	{
		return pInstance;
	}
}
void World::StartThreading()
{
	m_pThread = new std::thread(&World::Threading, this);
}
void World::ExitThreading()
{
	using namespace std;
	using namespace glm;

	m_exitThread = true;
	m_conVar.notify_all();
	m_pThread->join();

	delete m_pThread;
}

struct near2FarSort
{
	inline bool operator() (const glm::vec2 &l, const glm::vec2 &r)
	{
		using namespace glm;
		using namespace Rendering;
		vec2 camPosChunkID(floor(Camera::cameraPos.x / 16),floor( Camera::cameraPos.z / 16));
		
		glm::vec2 temp = l - camPosChunkID;
		float distSqr_l = dot(temp, temp);

		temp = r - camPosChunkID;
		float distSqr_r = dot(temp, temp);

		return distSqr_l > distSqr_r;
	}
};

void World::Threading()
{
	using namespace glm;

	vec2 generatingChunkID;
	while (1)
	{	
		// polling way
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		//mutex
		{
			
			std::unique_lock<std::mutex> lk(loadingMutex);
			m_conVar.wait(lk, [this](){return (m_loadingChunks.size() > 0) || m_exitThread == true || m_unloadedChunks.size() > 0; });//the function only blocks if predicate returns false
				
			if (m_needToSort == true)
			{
				std::sort(m_loadingChunks.begin(), m_loadingChunks.end(), near2FarSort());
				m_needToSort = false;
			}

			// Delete unloaded chunks
			{
				for (int i = 0; i < m_unloadedChunks.size(); i++)
				{
					delete m_unloadedChunks[i];
				}
				m_unloadedChunks.clear();

			}


			// Exit the thread
			if (m_exitThread)
			{
				ClearAllDataOnExit();
				return;
			}
				

			// In case spurious wakeup happens
			if (m_loadingChunks.empty() == true)
				continue;

			generatingChunkID = m_loadingChunks[m_loadingChunks.size() - 1];
			m_loadingChunks.pop_back();
		}


		Chunk *pChunk = new Chunk();
		pChunk->Initilize(generatingChunkID);

		// Generate Data
		{
			unsigned short *pData = new unsigned short[65536];
			GetChunkByPos(generatingChunkID.x, generatingChunkID.y, pData);
			
			for (int ty = 0; ty < 16; ty++)
			{

				int* tileData = pChunk->GetTileByID(ty)->pData;

				for (int x = 0; x < 16; x++)
					for (int z = 0; z < 16; z++)
						for (int y = 0; y < 16; y++)
						{
							int chunkY = 16 * ty + y;
							if (pData[GetBlockArrayIndex(x, chunkY, z)] != 0)//TODO: different Block ID
								tileData[x * 16 * 16 + y * 16 + z] = pData[GetBlockArrayIndex(x, chunkY, z)];
							else
								tileData[x * 16 * 16 + y * 16 + z] = 0;
						}
			}

			delete[]pData;
		}
			

		//mutex
		{
			std::lock_guard<std::mutex> lk(loadedMutex);
			m_loadedChunks.push_back(pChunk);
		}

		

	}


}

void World::Intilize()
{
	using namespace  glm;
	vec3 cameraPos(0, 200, 0);
	vec3 target(0, 100, -100);
	vec3 up(0, 1, 0);

	Rendering::Camera::SetLookAt(cameraPos, target, up);
	Rendering::Camera::SetPerspective(70, 4.0f / 3.0f, 0.1f, 1000.0f);

	StartThreading(); 
	m_needToSort = true; 
	m_exitThread = false;

	neighborOffsets[0] = vec2(-1,1);
	neighborOffsets[1] = vec2(0,1);
	neighborOffsets[2] = vec2(1,1);
	neighborOffsets[3] = vec2(-1,0);
	neighborOffsets[4] = vec2(1,0);
	neighborOffsets[5] = vec2(-1,-1);
	neighborOffsets[6] = vec2(0,-1);
	neighborOffsets[7] = vec2(1,-1);
}
void World::ShutDown()
{
	ExitThreading();
}

void World::Update(float delta)//TODO: delta time
{
	using namespace glm;
	using namespace std;

	vec3 camDir = Rendering::Camera::GetForwardDir();
	vec3 camPos = Rendering::Camera::cameraPos;
	vec2 camChunkID(floor(camPos.x / 16), floor(camPos.z / 16));
	vec2 camDir2D(camDir.x,camDir.z);

	std::vector<glm::vec2> requests;
	int range = 8;
	for (int x = -range; x <= range; x++)
	{
		for (int z = -range; z <= range; z++)
		{
			vec2 chunkDir(x, z);
			float dir = dot(normalize(chunkDir), camDir2D);
			if (dir > 0.0f 
				||(x == 0 && z == 0 ))
			{
				vec2 chunkID = camChunkID + chunkDir;
				if (m_worldChunks.find(chunkID) == m_worldChunks.end()
					&& m_requestingChunks.find(chunkID) == m_requestingChunks.end())
				{
					requests.push_back(chunkID);
					m_requestingChunks.insert(chunkID);
				}
			}
		}
	}

	RequestChunks(requests);

	// Chunk loaded, remove from task list(requestingChunks)
	std::vector<Chunk*> loadedChunks = GetLoadedChunks();
	for (int i = 0; i < loadedChunks.size(); i++)
	{
		m_worldChunks.insert(pair<glm::vec2, Chunk*>(loadedChunks[i]->chunkID, loadedChunks[i]));
		m_requestingChunks.erase(loadedChunks[i]->chunkID);
	}

	// Try to activate all potential chunks
	int maxCountPerFrame = 2;
	{	
		std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.begin();
		while (it != m_worldChunks.end())
		{
			if (maxCountPerFrame > 0)
			{
				if (TryActivateChunk(it->second) == true)
					maxCountPerFrame--;
			}else
				break;
				
			it++;
		}
	}


	return;

	{
		vector<Chunk*> unloadList;
		std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.begin();
		while (it != m_worldChunks.end())
		{
			vec2 diff = camChunkID - it->first;
			float sqrDis = dot(diff, diff);
			if (sqrDis >= 15 * 15)
			{
				unloadList.push_back(it->second);
				it = m_worldChunks.erase(it);
			}
			else
				it++;
		}

		RemoveChunks(unloadList);
	}

}

void World::RequestChunks(std::vector<glm::vec2> &chunkIDs)
{
	if (chunkIDs.size() < 0)
		return;

	//mutex
	std::lock_guard<std::mutex> lk(loadingMutex);

	std::vector<glm::vec2>::iterator it = chunkIDs.begin();
	while (it != chunkIDs.end())
	{
		{
			m_needToSort = true;
			m_loadingChunks.push_back(*it);
			// Generate Mesh
			//printf("\nChunk %f %f requested ", (*it).x, (*it).y);
		}
		it++;
	}

	if (m_needToSort)
	{
		m_conVar.notify_one();//the function only blocks if predicate returns false,so I can notify once
	}
	
}

void World::RemoveChunks(std::vector<Chunk *> &chunks)
{
	if (chunks.size() < 0)
		return;

	std::lock_guard<std::mutex> lk(loadingMutex);

	for (int i = 0; i < chunks.size();i++)
	{
		m_unloadedChunks.push_back(chunks[i]);
	}

	m_conVar.notify_one();
}

std::vector<Chunk*> World::GetLoadedChunks()
{
	std::lock_guard<std::mutex> lk(loadedMutex);

	std::vector<Chunk*> result = m_loadedChunks;
	m_loadedChunks.clear();
	return result;
}

World::World()
{
	pInstance = this;
}

World::~World()
{
}

void World::Draw()
{
	std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.begin();
	while (it != m_worldChunks.end())
	{
		if (it->second->IsActive())
		{
			it->second->Draw();
		}

		it++;
	}
}

void World::ClearAllDataOnExit()
{
	std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.begin();
	while (it != m_worldChunks.end())
	{
		delete it->second;
		it = m_worldChunks.erase(it);
	}
	for (int i = 0; i < m_loadedChunks.size(); i++)
	{
		delete m_loadedChunks[i];
	}
	for (int i = 0; i < m_unloadedChunks.size(); i++)
	{
		delete m_unloadedChunks[i];
	}
}
//SDL?
bool World::TryActivateChunk(Chunk *pChunk)
{
	if (pChunk->IsActive())
	{
		return false;
	}

	using namespace glm;
	vec2 centerChunkID;
	bool isAllSet = true;
	for (int i = 0; i < 8; i++)
	{
		if (pChunk->pNeighbors[i] != nullptr)
			continue;

		vec2 chunkID = centerChunkID + neighborOffsets[i];
		std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.find(chunkID);
		if (it != m_worldChunks.end())
		{
			pChunk->pNeighbors[i] = it->second;
		}
		else
		{
			isAllSet = false;
		}
	}

	
	if (isAllSet)
	{
		pChunk->GenerateMesh();
		pChunk->SetActive(isAllSet);
		return true;
	}
	else
		return false;

}

