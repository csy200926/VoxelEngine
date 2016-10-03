#pragma once

#include <glm.hpp>

#include <condition_variable>
#include <mutex>
#include <vector>
#include <map>
#include <set>
#include <list>

#include "Chunk.h"

namespace std
{
	class thread;
}

class World
{
public:
	 
	static World* GetInstance();

	void Initilize();
	void ShutDown();

	void StartThreading();
	void ExitThreading();

	void Update(float delta);

	void Draw();

	World();
	~World();

	Tile* GetTileByTileID(glm::vec3 &i_tileID)
	{
		glm::vec2 chunkID(i_tileID.x,i_tileID.z);
		std::map<glm::vec2, Chunk*, vec2_cmp>::iterator it = m_worldChunks.find(chunkID);
		if (it != m_worldChunks.end())
		{
			Chunk *pChunk = it->second;
			return pChunk->GetTileByID(i_tileID.y);

		}
		return nullptr;

	}
	
private:
	void Threading();
	void RequestChunks(std::vector<glm::vec2> &chunkIDs);
	void RemoveChunks(std::vector<Chunk *> &chunks);
	std::vector<Chunk*> GetLoadedChunks();

	// Called by threading
	void ClearAllDataOnExit();

	// Return: if any new mesh generated
	bool TryActivateChunk(Chunk *);
	void GenerateChunkMeshes();
	
	struct vec2_cmp {
		inline bool operator()(const glm::vec2 &l, const glm::vec2 &r)
		{
			if (l.x < r.x)
			{
				return true;
			}
			else if (l.x == r.x)
			{
				return l.y < r.y;
			}
			return false;
		}
	};

	// Main
	std::map<glm::vec2, Chunk*, vec2_cmp> m_worldChunks;
	std::set<glm::vec2, vec2_cmp> m_requestingChunks;
	std::vector<glm::vec2> m_meshGeneratingChunks;

	// Locks
	std::mutex loadingMutex;
	std::mutex loadedMutex;
	std::mutex removeMutex;

	// Threading
	std::vector<Chunk*> m_loadedChunks;
	std::vector<Chunk*> m_unloadedChunks;
	std::vector<glm::vec2> m_loadingChunks;
	bool m_needToSort;
	bool m_exitThread;
	
	// Threading wait
	std::condition_variable m_conVar;
	

	std::thread *m_pThread;
	static World *pInstance;

	float timer;

	//[  1  1  1  1           1  1  1  1       1  1  1  1  1  1  1  1 ]
	int GetBlockArrayIndex(const int biasX, const int y, const int biasZ)
	{
		const int k = 256;
		return biasX * k * 16 | biasZ * k | y;
	}
};

