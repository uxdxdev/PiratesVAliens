/*

Copyright (c) 2016 David Morton

*/
#ifndef BASE_SEARCH_CELL_H
#define BASE_SEARCH_CELL_H

#include <math.h>

namespace base {

class SearchCell{
public:
	int m_iCoordinateX;
	int m_iCoordinateZ;
	int m_iId;
	SearchCell *m_pParent;
	float m_fCostSoFarG;
	float m_fEstimatedCostToGoalH;

	SearchCell()
	{
		m_iCoordinateX = 0;
		m_iCoordinateZ = 0;
		m_iId = 0;
		m_pParent = 0;
		m_fCostSoFarG = 0;
		m_fEstimatedCostToGoalH = 0;
	}

	SearchCell(int x, int z, SearchCell *parent = 0, int worldWidth = 0)
	{
		m_iCoordinateX = x;
		m_iCoordinateZ = z;
		m_iId = (z * worldWidth + x);
		m_pParent = parent;
		m_fCostSoFarG = 0;
		m_fEstimatedCostToGoalH = 0;
	}

	float GetEstimatedTotalCostF()
	{
		return m_fCostSoFarG + m_fEstimatedCostToGoalH;
	}

	void SetId(int worldWidth)
	{
		m_iId = (m_iCoordinateZ * worldWidth + m_iCoordinateX);
	}

	float ManhattanDistance(SearchCell *nodeEnd)
	{
		float x = (float)(fabs((float)this->m_iCoordinateX - nodeEnd->m_iCoordinateX));
		float z = (float)(fabs((float)this->m_iCoordinateZ - nodeEnd->m_iCoordinateZ));

		return x + z;
	}
};

}

#endif
