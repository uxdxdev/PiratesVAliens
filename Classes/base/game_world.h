/*

Copyright (c) 2016 David Morton

*/
#ifndef BASE_GAME_WORLD_H_
#define BASE_GAME_WORLD_H_

#include <vector>

#define CELL_SIZE 1

namespace base{

class GameWorld{
public:
	enum CellState{
		CELL_OPEN,
		CELL_BLOCKED
	};

	GameWorld(int width, int height);

	int GetWorldWidth();

	int GetWorldHeight();

	int GetCellX(int x);

	int GetCellY(int y);

	int GetCellZ(int z);

	int GetCellState(int x, int y, int z);

	void SetCellState(int x, int y, int z, int state);

private:

	int m_iWorldWidth;
	int m_iWorldHeight;

	std::vector< std::vector<int> > m_Grid;
};

}
#endif
