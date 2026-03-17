#include "POTSlabAllocator.h"

using namespace sngl::core;

FixedSizeBlock::FixedSizeBlock()
	: m_freeList(nullptr),
	m_blockSize(0),
	m_pageSize(0)
{ }

void FixedSizeBlock::init(size_t blockSize)
{
	m_pageSize = sngl::platform::GetPageSize();
	assert(math::isPOT(blockSize)); // blockSize should be a power of 2
	m_blockSize = blockSize;
}

void* FixedSizeBlock::pop()
{
	if (m_freeList == nullptr)
		return nullptr; // supply a page to partition

	Node* currentNode = m_freeList;
	m_freeList = m_freeList->next;

	return m_freeList;
}

void FixedSizeBlock::push(void* ptr)
{
	if (!ptr)
		return;

	Node* newNode = reinterpret_cast<Node*>(ptr);
	newNode->next = m_freeList;
	m_freeList = newNode;
}

void FixedSizeBlock::supplyNewMemory(void* ptr, size_t memSize)
{
	assert(memSize % m_pageSize == 0); // Block size should be a multiple of system page size
	const size_t nodeCount = memSize / m_blockSize;
	uint8_t* const startPtr = static_cast<uint8_t* const>(ptr);

	for (size_t i = 0; i < nodeCount; i++)
	{
		Node* node = reinterpret_cast<Node*>(startPtr + (i * m_blockSize));
		node->next = m_freeList;
		m_freeList = node;
	}
}