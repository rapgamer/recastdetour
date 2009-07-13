
#include "DetourNode.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////
dtNodePool::dtNodePool(int maxNodes, int hashSize) :
	m_maxNodes(maxNodes),
	m_hashSize(hashSize),
	m_nodes(0),
	m_first(0),
	m_next(0)
{
	m_nodes = new dtNode[m_maxNodes];
	m_next = new unsigned short[m_maxNodes];
	m_first = new unsigned short[hashSize];
	memset(m_first, 0xff, sizeof(unsigned short)*m_hashSize);
	memset(m_next, 0xff, sizeof(unsigned short)*m_maxNodes);
}

dtNodePool::~dtNodePool()
{
	delete [] m_nodes;
	delete [] m_next;
	delete [] m_first;
}

void dtNodePool::clear()
{
	memset(m_first, 0xff, sizeof(unsigned short)*m_hashSize);
	m_nodeCount = 0;
}

const dtNode* dtNodePool::findNode(unsigned int id) const
{
	unsigned int bucket = hashint(id) & (m_hashSize-1);
	unsigned short i = m_first[bucket];
	while (i != 0xffff)
	{
		if (m_nodes[i].id == id)
			return &m_nodes[i];
		i = m_next[i];
	}
	return 0;
}

dtNode* dtNodePool::getNode(unsigned int id)
{
	unsigned int bucket = hashint(id) & (m_hashSize-1);
	unsigned short i = m_first[bucket];
	dtNode* node = 0;
	while (i != 0xffff)
	{
		if (m_nodes[i].id == id)
			return &m_nodes[i];
		i = m_next[i];
	}
	
	if (m_nodeCount >= m_maxNodes)
		return 0;
	
	i = (unsigned short)m_nodeCount;
	m_nodeCount++;
	
	// Init node
	node = &m_nodes[i];
	node->pidx = 0;
	node->cost = 0;
	node->total = 0;
	node->id = id;
	node->flags = 0;
	
	m_next[i] = m_first[bucket];
	m_first[bucket] = i;
	
	return node;
}


//////////////////////////////////////////////////////////////////////////////////////////
dtNodeQueue::dtNodeQueue(int n) :
	m_capacity(n),
	m_size(0),
	m_heap(0)
{
	m_heap = new dtNode*[m_capacity+1];
}

dtNodeQueue::~dtNodeQueue()
{
	delete [] m_heap;
}

void dtNodeQueue::bubbleUp(int i, dtNode* node)
{
	int parent = (i-1)/2;
	// note: (index > 0) means there is a parent
	while ((i > 0) && (m_heap[parent]->total > node->total))
	{
		m_heap[i] = m_heap[parent];
		i = parent;
		parent = (i-1)/2;
	}
	m_heap[i] = node;
}

void dtNodeQueue::trickleDown(int i, dtNode* node)
{
	int child = (i*2)+1;
	while (child < m_size)
	{
		if (((child+1) < m_size) && 
			(m_heap[child]->total > m_heap[child+1]->total))
		{
			child++;
		}
		m_heap[i] = m_heap[child];
		i = child;
		child = (i*2)+1;
	}
	bubbleUp(i, node);
}
