#pragma once
#include <glm.hpp>
#include <vector>

class BoundingBox
{
public:
	glm::vec3 GetMinPos() const
	{
		return min;
	};
	glm::vec3 GetMaxPos() const
	{
		return max;
	};
	
	glm::vec3 GetSize() const
	{
		return max - min;
	}
	glm::vec3 GetCenterPos() const
	{
		return (max - min) * 0.5f;
	}

	BoundingBox(int minX, int minY, int minZ, int maxX, int maxY, int maxZ)
	{
		min = glm::vec3(minX, minY, minZ);
		max = glm::vec3(maxX, maxY, maxZ);
	}

	bool Inside(const  glm::vec3 &pos) const
	{
		if (pos.x >= min.x && pos.x <= max.x &&
			pos.y >= min.y && pos.y <= max.y &&
			pos.z >= min.z && pos.z <= max.z)
			return true;
		return false;
	}

	bool Inside(const BoundingBox &boundingBox) const
	{
		return Inside(boundingBox.min) && Inside(boundingBox.max);
	}

	bool IntersectWith(const BoundingBox &boundingBox) const
	{
		if (boundingBox.min.x < max.x && min.x < boundingBox.max.x &&
			boundingBox.min.y < max.y && min.y < boundingBox.max.y &&
			boundingBox.min.z < max.z && min.z < boundingBox.max.z)
			return true;
		return false;
	}

	BoundingBox(){};// :min(glm::vec3(0, 0, 0)), max(glm::vec3(0, 0, 0)) {};
	BoundingBox(glm::vec3 &minPos, glm::vec3 &maxPos)// : min(minPos), max(maxPos) 
	{
		min = minPos;
		max = maxPos;
	};

private:
	glm::vec3 min;
	glm::vec3 max;
};


#define MAX_LEVELS 5
#define MAX_OBJECTS 2

class StrangeTree
{
public:
	StrangeTree(){ m_nodes = nullptr; };
	StrangeTree(int i_level, BoundingBox &i_bb);
	~StrangeTree();

	void Initilize(int i_level, BoundingBox &i_bb);

	void Retrieve(std::vector<BoundingBox> &returnObjs, BoundingBox &i_bb);
	void RetrieveAll(std::vector<BoundingBox> &returnObjs);

	void Clear();
	void Split();
	void Insert(BoundingBox &i_bb);

	int GetIndex(BoundingBox &i_bb) const;

private:
	BoundingBox m_boundingBox;
	std::vector<BoundingBox> m_list;
	StrangeTree* m_nodes;
	int m_level;



};

