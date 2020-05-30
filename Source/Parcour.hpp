#pragma once

#include <vector>
#include <stdint.h>
#include <limits>
#include <iostream>

namespace Soon
{
	struct Block
	{
		uint32_t offsetNext;
		uint32_t offsetPrevious;
		bool free;
	};

	class Iterator
	{
	private:
		std::vector<Block> &buffer;
		uint32_t m_Size;

	public:
		Iterator(void) = delete;
		Iterator(std::vector<Block> &pBuffer, uint32_t beginId, uint32_t size) : buffer(pBuffer), id(beginId), m_Size(size)
		{
		}

		uint32_t GetSize(void)
		{
			return m_Size;
		}

		Iterator &operator++();
		Iterator &operator--();

		uint32_t id;
	};

	Iterator &Iterator::operator++()
	{
		id += buffer[id].offsetNext;
		return *this;
	}

	Iterator &Iterator::operator--()
	{
		id -= buffer[id].offsetPrevious;
		return *this;
	}

	class Parcour
	{
	private:
		std::vector<Block> m_Buffer;
		std::vector<uint32_t> m_FreeId;
		uint32_t size = 0;
		uint32_t beginId;

	public:
		Parcour(void)
		{
			beginId = std::numeric_limits<uint32_t>::max();
		}

		uint32_t Add(void)
		{
			uint32_t id;
			if (!m_FreeId.empty())
			{
				id = m_FreeId.back();
				m_FreeId.pop_back();
				Add(id);
			}
			else
			{
				//size += 1;
				//if (m_Buffer.size() == 0)
				//	m_Buffer.push_back({0, 1, false});
				//else
				m_Buffer.push_back({0, 0, false});
				id = Add(m_Buffer.size() - 1);
				//if (beginId == std::numeric_limits<uint32_t>::max())
				//	beginId = id;
			}
			return id;
		}

		uint32_t Add(uint32_t id)
		{
			m_Buffer[id].offsetPrevious = 1;
			m_Buffer[id].offsetNext = 1;
			size += 1;

			m_Buffer[id].free = false;
			if (id == 0)
			{
				m_Buffer[id].offsetPrevious = 0;
				beginId = id;
				for (uint32_t index2 = id + 1; index2 < m_Buffer.size(); index2++)
				{
					if (m_Buffer[index2].free)
						m_Buffer[id].offsetNext += 1;
					else
					{
						m_Buffer[index2].offsetPrevious = m_Buffer[id].offsetNext;
						return id;
					}
				}
				return id;
			}

			for (uint32_t index = id - 1; index >= 0; index -= 1)
			{
				if (index == 0)
				{
					if (!m_Buffer[0].free)
					{
						m_Buffer[id].offsetNext = m_Buffer[index].offsetNext - m_Buffer[id].offsetPrevious;
						m_Buffer[index].offsetNext = m_Buffer[id].offsetPrevious;
						beginId = 0;
						break;
					}
					else
					{
						for (uint32_t index2 = id + 1; index2 < m_Buffer.size(); index2++)
						{
							if (m_Buffer[index2].free)
								m_Buffer[id].offsetNext += 1;
							else
							{
								m_Buffer[index2].offsetPrevious = m_Buffer[id].offsetNext;
								return id;
							}
							return id;
						}
					}
				}
				if (m_Buffer[index].free)
					m_Buffer[id].offsetPrevious += 1;
				else
				{
					if (id < beginId)
						beginId = id;
					m_Buffer[id].offsetNext = m_Buffer[index].offsetNext - m_Buffer[id].offsetPrevious;
					m_Buffer[index].offsetNext = m_Buffer[id].offsetPrevious;
					break;
				}
			}
			return id;
		}

		void Remove(uint32_t id)
		{
			if (id > m_Buffer.size() || m_Buffer[id].free)
				return;
			size -= 1;

			if (id != 0 && !m_Buffer[id + m_Buffer[id].offsetPrevious].free)
				beginId = id + m_Buffer[id].offsetPrevious;
			else if (!m_Buffer[id + m_Buffer[id].offsetNext].free)
				beginId = id + m_Buffer[id].offsetNext;
			else
				beginId = std::numeric_limits<uint32_t>::max();

			if (id != 0 && (id - m_Buffer[id].offsetPrevious) >= 0)
				m_Buffer[id - m_Buffer[id].offsetPrevious].offsetNext += m_Buffer[id].offsetNext;
			m_Buffer[id + m_Buffer[id].offsetNext].offsetPrevious += m_Buffer[id].offsetPrevious;
			m_Buffer[id].free = true;
			m_FreeId.push_back(id);
		}

		Iterator GetIterator(void)
		{
			Iterator it(m_Buffer, beginId, size);
			return it;
		}

		uint32_t GetSize(void)
		{
			return size;
		}
	};
} // namespace Soon