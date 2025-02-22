#pragma once
#include <vector>

constexpr char LIVE_CELL = '#';
constexpr char DEAD_CELL = '.';

struct Coordinates
{
	int x;
	int y;
};

using Cells = std::vector<char>;

struct Field
{
	int width;
	int height;
	Cells cells;
};

class Life
{
public:
	Life(Field field, int threadsNum);
	void NextStep();
	[[nodiscard]] Field GetField() const;

private:
	[[nodiscard]] char GetCell(Coordinates coords) const;
	[[nodiscard]] int GetCellNeighborsNumber(size_t pos) const;
	void NextStepForBlock(size_t begin, size_t end, Cells& result) const;

	static char GetLiveCellNextState(int neighboursNumber);
	static char GetDeadCellNextState(int neighboursNumber);

private:
	Cells m_cells;
	int m_width;
	int m_height;
	int m_threadsNum;
};