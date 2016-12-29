/*

Copyright (c) 2016 David Morton

*/
#include <sstream>

#include "ai/pathfinding_astar.h"
#include "manager/world_manager.h"

namespace ai {

AStar::AStar(int width, int height)
{
	//printf("ctor Pathfinding()\n");

	m_GoalCell = NULL;
	m_pGameWorld = std::unique_ptr<base::GameWorld> (new base::GameWorld(width, height));
	m_bInitializedStartGoal = false;
	m_eState = IDLE;
}

AStar::~AStar()
{
	CleanUp();
}

void AStar::PrintPath()
{
	printf("-----PATH Start-----\n");

	for(int i = 0; i < m_vPathToGoal.size(); i++)
	{
		printf("(%d, %d)\n", m_vPathToGoal[i]->m_iX, m_vPathToGoal[i]->m_iZ);
	}

	printf("-----PATH End-----\n");

}

void AStar::PrintStatus(std::string message)
{
	printf("--Pathfinder Status--\n");
	printf("State: %d\nOpen List: %d\nClosed List: %d\nPath list: %d\n", m_eState, (int)m_vOpenList.size(), (int)m_vClosedList.size(), (int)m_vPathToGoal.size());
	printf("Goal Cell: %d\n", ((m_GoalCell) ? m_GoalCell->m_iId : -1));
	printf("Message: %s\n\n", message.c_str());
}

// Get pathfinder state
AStar::State AStar::GetState()
{
	return m_eState;
}

void AStar::SetState(State state)
{
	m_eState = state;
}

base::GameWorld *AStar::GetGameWorld()
{
	return m_pGameWorld.get();
}

void AStar::CleanUp()
{
	ClearOpenList();
	ClearClosedList();
	ClearPathToGoal();
	delete m_GoalCell;
	m_GoalCell = NULL;
}

void AStar::ClearOpenList()
{
	for(unsigned int i = 0; i < m_vOpenList.size(); i++)
	{
		delete m_vOpenList[i];
	}
	m_vOpenList.clear();
}

void AStar::ClearClosedList()
{
	for(unsigned int i = 0; i < m_vClosedList.size(); i++)
	{
		delete m_vClosedList[i];
	}
	m_vClosedList.clear();
}

void AStar::ClearPathToGoal()
{
	for(unsigned int i = 0; i < m_vPathToGoal.size(); i++)
	{
		//delete m_vPathToGoal[i];
	}
	m_vPathToGoal.clear();
}

void AStar::FindPath(std::shared_ptr<base::Vector3> currentPos, std::shared_ptr<base::Vector3> targetPos)
{
	// Check if start or end goal cells are blocked
	if (m_pGameWorld->GetCellState(currentPos->m_iX, 0, currentPos->m_iZ) == base::GameWorld::CELL_BLOCKED)
	{
		//printf("-------------------------Start Cell Blocked X:%d Z:%d------------------------\n", currentPos->m_iX, currentPos->m_iZ);
		return;
	}
	else if (m_pGameWorld->GetCellState(targetPos->m_iX, 0, targetPos->m_iZ) == base::GameWorld::CELL_BLOCKED)
	{
		//printf("-------------------------Target Cell Blocked X:%d Z:%d------------------------\n", targetPos->m_iX, targetPos->m_iZ);
		return;
	}

	if(!m_bInitializedStartGoal)
	{
		//printf("Initializing Start Goal X:%d Z:%d\n", currentPos.m_iX, currentPos.m_iZ);
		CleanUp();

		// Initialize start
		base::SearchCell start;
		start.m_iCoordinateX = m_pGameWorld->GetCellX(currentPos->m_iX);
		start.m_iCoordinateZ = m_pGameWorld->GetCellZ(currentPos->m_iZ);

		// Initialize goal
		base::SearchCell goal;
		goal.m_iCoordinateX = m_pGameWorld->GetCellX(targetPos->m_iX);
		goal.m_iCoordinateZ = m_pGameWorld->GetCellZ(targetPos->m_iZ);

		SetStartAndGoal(start, goal);
		m_bInitializedStartGoal = true;
		m_eState = PROCESSING;

		PrintStatus("Request initialization complete");
	}

	if(m_bInitializedStartGoal)
	{
		// Start timer
		startTime = std::chrono::high_resolution_clock::now();
		Update();
	}
}

void AStar::SetStartAndGoal(base::SearchCell start, base::SearchCell goal)
{
	//printf("Called SetStartAndGoal()\n");
	m_GoalCell = new base::SearchCell(goal.m_iCoordinateX, goal.m_iCoordinateZ, &goal, m_pGameWorld->GetWorldWidth());

	base::SearchCell startCell;
	startCell.m_iCoordinateX = start.m_iCoordinateX;
	startCell.m_iCoordinateZ = start.m_iCoordinateZ;
	startCell.m_pParent = 0;
	startCell.SetId(m_pGameWorld->GetWorldWidth());
	startCell.m_fCostSoFarG = 0;
	startCell.m_fEstimatedCostToGoalH = startCell.ManhattanDistance(m_GoalCell);

	m_vOpenList.push_back(new base::SearchCell(startCell));
}

base::SearchCell *AStar::GetNextCell()
{
	float bestF = 99999.0f;
	int cellIndex = -1;
	base::SearchCell *nextCell = NULL;

	for(int i = 0; i < m_vOpenList.size(); i++)
	{
		if(m_vOpenList[i]->GetEstimatedTotalCostF() < bestF)
		{
			bestF = m_vOpenList[i]->GetEstimatedTotalCostF();
			cellIndex = i;
		}
	}

	if(cellIndex >= 0)
	{
		nextCell = m_vOpenList[cellIndex];
		m_vClosedList.push_back(nextCell);
		m_vOpenList.erase(m_vOpenList.begin() + cellIndex);
	}

	//printf("Called GetNextCell() : X:%d Z:%d\n", nextCell->m_iCoordinateX, nextCell->m_iCoordinateZ);

	return nextCell;
}


void AStar::Update()
{
	while (1)
	{

		for (int i = 0; i < 4; i++)
		{
			//printf("Called ContinuePath()\n");

			if (m_vOpenList.empty())
			{
				return;
			}

			base::SearchCell *currentCell = GetNextCell();

			// If we have reached the goal cell
			if (currentCell->m_iId == m_GoalCell->m_iId)
			{
				//printf("Goal cell reached\n");

				m_GoalCell->m_pParent = currentCell->m_pParent;

				base::SearchCell *getPath;

				for (getPath = m_GoalCell; getPath != NULL; getPath = getPath->m_pParent)
				{
					m_vPathToGoal.push_back(std::shared_ptr<base::Vector3>(new base::Vector3(getPath->m_iCoordinateX * CELL_SIZE, 0, getPath->m_iCoordinateZ * CELL_SIZE)));
				}

				m_eState = REQUEST_COMPLETE;

				endTime = std::chrono::high_resolution_clock::now();
				long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
				std::stringstream message;
				//message << "Time taken for pathfinding request to return in milliseconds: " << duration << std::endl;

				// CSV milliseconds, path size
				message << ms << "," << m_vPathToGoal.size() << std::endl;

				WorldManager::getInstance()->WriteToLogFile(message);

				return;
			}
			else
			{
				// topleft diagonal
				//ProcessCell(currentCell->m_iCoordinateX - 1, currentCell->m_iCoordinateZ + 1, currentCell->m_fCostSoFarG + 1.414f, currentCell);

				// topright diagonal
				//ProcessCell(currentCell->m_iCoordinateX + 1, currentCell->m_iCoordinateZ + 1, currentCell->m_fCostSoFarG + 1.414f, currentCell);

				// bottom left diagonal
				//ProcessCell(currentCell->m_iCoordinateX - 1, currentCell->m_iCoordinateZ - 1, currentCell->m_fCostSoFarG + 1.414f, currentCell);

				// bottom right
				//ProcessCell(currentCell->m_iCoordinateX + 1, currentCell->m_iCoordinateZ - 1, currentCell->m_fCostSoFarG + 1.414f, currentCell);

				// right side
				ProcessCell(currentCell->m_iCoordinateX + 1, currentCell->m_iCoordinateZ, currentCell->m_fCostSoFarG + 1, currentCell);

				// left side
				ProcessCell(currentCell->m_iCoordinateX - 1, currentCell->m_iCoordinateZ, currentCell->m_fCostSoFarG + 1, currentCell);

				// top cell
				ProcessCell(currentCell->m_iCoordinateX, currentCell->m_iCoordinateZ + 1, currentCell->m_fCostSoFarG + 1, currentCell);

				// bottom cell
				ProcessCell(currentCell->m_iCoordinateX, currentCell->m_iCoordinateZ - 1, currentCell->m_fCostSoFarG + 1, currentCell);



				for (int i = 0; i < m_vOpenList.size(); i++)
				{
					if (currentCell->m_iId == m_vOpenList[i]->m_iId)
					{
						m_vOpenList.erase(m_vOpenList.begin() + i);
					}
				}
			}
		}
	}
}

void AStar::ProcessCell(int x, int z, float newCost, base::SearchCell *parent)
{
	if (x < 0 || x > m_pGameWorld->GetWorldWidth() - 1 || z < 0 || z > m_pGameWorld->GetWorldHeight() - 1)
	{
		// Out of bounds
		return;
	}

	// Walls etc.
	if(m_pGameWorld->GetCellState(x, 0, z) == base::GameWorld::CELL_BLOCKED)
	{
		return;
	}

	int id = z * m_pGameWorld->GetWorldWidth() + x;

	for(int i = 0; i < m_vClosedList.size(); i++)
	{
		if(id == m_vClosedList[i]->m_iId)
		{
			return;
		}
	}

	base::SearchCell *newChild = new base::SearchCell(x, z, parent, m_pGameWorld->GetWorldWidth());
	newChild->m_fCostSoFarG = newCost;
	newChild->m_fEstimatedCostToGoalH = parent->ManhattanDistance(m_GoalCell);

	//printf("New child ID %d X%d Z:%d G:%f H:%f\n",newChild->m_iId, newChild->m_iCoordinateX, newChild->m_iCoordinateZ, newChild->G, newChild->H);

	for(int i = 0; i < m_vOpenList.size(); i++)
	{
		if(id == m_vOpenList[i]->m_iId)
		{
			float newF = newChild->m_fCostSoFarG + newCost + m_vOpenList[i]->m_fEstimatedCostToGoalH;

			// if newF is less than one in the list replace it
			if(m_vOpenList[i]->GetEstimatedTotalCostF() > newF)
			{
				m_vOpenList[i]->m_fCostSoFarG = newChild->m_fCostSoFarG + newCost;
				m_vOpenList[i]->m_pParent = newChild;
			}
			else // if the newF is not better
			{
				delete newChild;
				return;
			}
		}
	}

	m_vOpenList.push_back(newChild);
}


std::vector<std::shared_ptr<base::Vector3> > *AStar::GetPathToGoalRef()
{
	// Once the path to goal has been accessed the pathfinder
	// can be reset to IDLE in order to process more requests
	m_eState = IDLE;
	return &m_vPathToGoal;
}

std::vector<std::shared_ptr<base::Vector3> > AStar::CopyPathToGoal()
{
	// Return a copy of the vector
	m_eState = IDLE;
	ResetPath();
	return m_vPathToGoal;
}


void AStar::ResetPath()
{
	m_eState = IDLE;
	m_bInitializedStartGoal = false;
}

}
