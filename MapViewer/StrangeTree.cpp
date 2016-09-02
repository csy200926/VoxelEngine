#include "StrangeTree.h"

void StrangeTree::Clear()
{
	m_list.clear();

	if (m_nodes != nullptr)
	{
		for (int i = 0; i < 8; i++)//Size should be eight
		{
			m_nodes[i].Clear();
		}
		delete[] m_nodes;
	}

}

void StrangeTree::Initilize(int i_level, BoundingBox &i_bb)
{
	m_level = i_level;
	m_boundingBox = i_bb;
}

/*	
		__________ max
	   /	 /	   /|
      /__7__/__6__/
	 /	   /	 /|
	/__4__/__5__/    
	|	  |	    |
		upper

		|_____|_____|
	   /	 /	   /
     |/__3__/__2_|/
	 /	   /	 /
 min/__0_|/__1_|/

	    lower

*/
/*
* Determine which node the object belongs to. -1 means
* object cannot completely fit within a child node and is part
* of the parent node
*/
int StrangeTree::GetIndex(BoundingBox &i_bb) const
{
	int index = -1;
	if (m_nodes == nullptr)
		return index;

	for (int i = 0; i < 8; i++)
	{
		StrangeTree& subTree = m_nodes[i];

		if (subTree.m_boundingBox.Inside(i_bb))
		{
			index = i;
			break;
		}
		
	}
	return index;
}

void StrangeTree::Insert(BoundingBox &i_bb)
{
	if (m_nodes != nullptr)
	{
		int index = GetIndex(i_bb);

		if (index != -1)
		{
			m_nodes[index].Insert(i_bb);
		}
		return;
	}

	m_list.push_back(i_bb);


	if (m_level < MAX_LEVELS && m_list.size() > MAX_OBJECTS)
	{
		if (m_nodes == nullptr)
			Split();

		int i = m_list.size() - 1;
		while (i >= 0)
		{
			int index = GetIndex(m_list[i]);
			if (index != -1)
			{
				m_nodes[index].Insert(m_list[i]);
				m_list.erase(m_list.begin() + i);
			}
			
			i--;
			
		}

	}
}

void StrangeTree::RetrieveAll(std::vector<BoundingBox> &returnObjs)
{
	if (m_nodes != nullptr)
	{
		int i = 0;
		while (i < 8)
		{
			m_nodes[i].RetrieveAll(returnObjs);
			i++;
		}

	}

	int i = 0;
	while (i < m_list.size())
	{
		returnObjs.push_back(m_list[i]);
		i++;
	}

}

void StrangeTree::Retrieve(std::vector<BoundingBox> &returnObjs, BoundingBox &i_bb)
{
	int index = GetIndex(i_bb);
	if (index != -1)
	{
		m_nodes[index].Retrieve(returnObjs, i_bb);
	}
	else
	{
		RetrieveAll(returnObjs);
		return;
	}

	int i = 0;
	while (i < m_list.size())
	{
		returnObjs.push_back(m_list[i]);
		i++;
	}

}


void StrangeTree::Split()
{
	using namespace glm;

	vec3 centerPos = m_boundingBox.GetCenterPos();
	vec3 minPos = m_boundingBox.GetMinPos();
	vec3 maxPox = m_boundingBox.GetMaxPos();

	vec3 subSize = centerPos - minPos;
	vec3 right(subSize.x, 0, 0);
	vec3 forward(0, 0, subSize.z);
	vec3 up(0, subSize.y, 0);

	m_nodes = new StrangeTree[8];

	int newLevel = m_level + 1;

	vec3 subMin;

	// lower
	m_nodes[0].Initilize(newLevel, BoundingBox(minPos, centerPos));

	subMin = vec3(minPos.x + subSize.x, minPos.y, minPos.z);
	m_nodes[1].Initilize(newLevel, BoundingBox(subMin, subMin + subSize));

	subMin = vec3(minPos.x + subSize.x, minPos.y, minPos.z + subSize.z);
	m_nodes[2].Initilize(newLevel, BoundingBox(subMin, subMin + subSize));

	subMin = vec3(minPos.x, minPos.y, minPos.z + subSize.z);
	m_nodes[3].Initilize(newLevel, BoundingBox(subMin, subMin + subSize));

	// upper
	subMin = vec3(minPos.x, minPos.y + subSize.y, minPos.z);
	m_nodes[4].Initilize(newLevel, BoundingBox(minPos, centerPos));

	subMin = vec3(minPos.x + subSize.x, minPos.y + subSize.y, minPos.z);
	m_nodes[5].Initilize(newLevel, BoundingBox(minPos, centerPos));

	m_nodes[6].Initilize(newLevel, BoundingBox(centerPos, maxPox));

	subMin = vec3(minPos.x, minPos.y + subSize.y, minPos.z + subSize.z);
	m_nodes[7].Initilize(newLevel, BoundingBox(minPos, centerPos));

}

StrangeTree::StrangeTree(int i_level, BoundingBox &i_bb) :m_level(i_level), m_boundingBox(i_bb), m_nodes(nullptr)
{

}


StrangeTree::~StrangeTree()
{
}
