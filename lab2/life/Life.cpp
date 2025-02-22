#include "Life.h"

#include <thread>
#include <utility>

Life::Life(Field field, const int threadsNum)
	: m_cells(std::move(field.cells))
	  , m_width(field.width)
	  , m_height(field.height)
	  , m_threadsNum(threadsNum)
{
}


void Life::NextStep()
{
	Cells result;
	const auto size = m_cells.size();
	result.resize(size);

	const auto blockSize = size / m_threadsNum;
	{
		std::vector<std::jthread> threads(m_threadsNum - 1);
		size_t blockStart = 0;
		for (size_t i = 0; i < m_threadsNum - 1; ++i)
		{
			auto blockEnd = blockStart;
			blockEnd += blockSize;
			threads[i] = std::jthread{ &Life::NextStepForBlock, this, blockStart, blockEnd, std::ref(result) };
			blockStart = blockEnd;
		}
		NextStepForBlock(blockStart, size, result);
	}

	m_cells = std::move(result);
}

void Life::NextStepForBlock(const size_t begin, const size_t end, Cells& result) const
{
	for (size_t i = begin; i < end; ++i)
	{
		const auto neighboursNum = GetCellNeighborsNumber(i);
		result[i] = m_cells[i] == LIVE_CELL
			? GetLiveCellNextState(neighboursNum)
			: GetDeadCellNextState(neighboursNum);
	}
}

Field Life::GetField() const
{
	return {
		m_width,
		m_height,
		m_cells,
	};
}

char Life::GetCell(const Coordinates coords) const
{
	const auto x = (coords.x + m_width) % m_width;
	const auto y = (coords.y + m_height) % m_height;
	return m_cells[y * m_width + x];
}

int Life::GetCellNeighborsNumber(const size_t pos) const
{
	const int x = pos % m_width;
	const int y = pos / m_width;
	int result = 0;
	for (int i = x - 1; i <= x + 1; i++)
	{
		for (int j = y - 1; j <= y + 1; j++)
		{
			if (!(i == x && j == y) && GetCell({ i, j }) == LIVE_CELL)
			{
				++result;
			}
		}
	}

	return result;
}

char Life::GetLiveCellNextState(const int neighboursNumber)
{
	return neighboursNumber == 2 || neighboursNumber == 3
		? LIVE_CELL
		: DEAD_CELL;
}

char Life::GetDeadCellNextState(const int neighboursNumber)
{
	return neighboursNumber == 3
		? LIVE_CELL
		: DEAD_CELL;
}