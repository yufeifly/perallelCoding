#include <iostream>
#include <stdio.h>
#include <map>
#include <stack>
#include <cassert>
using namespace std;
const int TEMPLATE_SIZE = 9;
const int SUB_SIZE = 3;
bool bSolved = false;
int gCount = 0;
class Grid
{
public:
	Grid(){}
	int val;
	int nRemainCount;
	map<int, int> valMap;
	bool Conflict(int val)
	{
		return valMap.find(val) != valMap.end();
	}
	void IncCount(int _val)
	{
		valMap[_val]++;
		nRemainCount = 9 - valMap.size();
	}
	void DecCount(int _val)
	{
		valMap[_val]--;
		if (valMap[_val] == 0)
		{
			valMap.erase(_val);
		}
		nRemainCount = 9 - valMap.size();
	}
};
int board[TEMPLATE_SIZE][TEMPLATE_SIZE] = {
	{2, 5, 0, 0, 0, 9, 0, 4, 0},
	{4, 0, 7, 1, 0, 3, 0, 0, 6},
	{8, 0, 3, 4, 0, 7, 5, 9, 0},
	{3, 0, 8, 0, 7, 0, 0, 6, 9},
	{0, 1, 0, 3, 0, 2, 4, 0, 0},
	{5, 0, 4, 9, 0, 6, 0, 8, 3},
	{9, 0, 6, 0, 3, 0, 7, 0, 8},
	{0, 3, 0, 6, 0, 8, 0, 1, 0},
	{1, 0, 2, 0, 9, 0, 6, 0, 4}
};
class SUDOKUState
{
public:

	bool CanPlace(int val)
	{
		return !m_grids[m_curY][m_curX].Conflict(val);
	}
	bool IsFinal()
	{
		return m_nRemained == 0;
	}

	bool IsDead()
	{
		return m_curY == -1 && m_curY == -1;
	}
	void RemoveNumber(int val)
	{
		assert(!posTrace.empty());
		pair<int, int> prePos = posTrace.top();

		m_curX = prePos.first;
		m_curY = prePos.second;
		m_grids[m_curY][m_curX].val = 0;
		m_grids[m_curY][m_curX].DecCount(val);
		for (int r = 0;r < TEMPLATE_SIZE;r++)
		{
			if (r != m_curY)
			{
				m_grids[r][m_curX].DecCount(val);
			}
		}
		for (int c = 0 ; c < TEMPLATE_SIZE; c++)
		{
			if (c != m_curX)
			{
				m_grids[m_curY][c].DecCount(val);
			}
		}

		for (int r = (m_curY / 3) * 3; r < (m_curY / 3) * 3 + 3; r ++)
		{
			for (int c = (m_curX / 3) * 3; c < (m_curX / 3) * 3 + 3; c++)
			{
				if (r == m_curY || c == m_curX)
				{
					continue;
				}
				m_grids[r][c].DecCount(val);
			}
		}
		posTrace.pop();
		m_nRemained ++;
	}
	void PlaceNumber(int val)
	{
		m_grids[m_curY][m_curX].val = val;
		m_grids[m_curY][m_curX].IncCount(val);
		for (int r = 0;r < TEMPLATE_SIZE;r++)
		{
			if (r != m_curY)
			{
				m_grids[r][m_curX].IncCount(val);
			}
		}
		for (int c = 0 ; c < TEMPLATE_SIZE; c++)
		{
			if (c != m_curX)
			{
				m_grids[m_curY][c].IncCount(val);
			}
		}

		for (int r = (m_curY / 3) * 3; r < (m_curY / 3) * 3 + 3; r ++)
		{
			for (int c = (m_curX / 3) * 3; c < (m_curX / 3) * 3 + 3; c++)
			{
				if (r == m_curY || c == m_curX)
				{
					continue;
				}
				m_grids[r][c].IncCount(val);
			}
		}
		m_nRemained --;
		posTrace.push(make_pair<int,int>(m_curX, m_curY));
		DecideNextPlace();
	}

	SUDOKUState(int a[TEMPLATE_SIZE][TEMPLATE_SIZE])
	{
		m_nRemained = TEMPLATE_SIZE * TEMPLATE_SIZE;
		for (int i = 0; i <TEMPLATE_SIZE;i++)
		{
			for (int j = 0; j< TEMPLATE_SIZE;j++)
			{
				m_grids[i][j].nRemainCount = TEMPLATE_SIZE;
				m_grids[i][j].val = a[i][j];
				if (a[i][j] != 0)
				{
					m_curX = j;
					m_curY = i;
					PlaceNumber(a[i][j]);
				}
			}
		}
		DecideNextPlace();
	}
	//计算下一步放数字的格子，贪心
	void DecideNextPlace()
	{
		if (m_nRemained == 0)
		{
			return;
		}
		int minv = 10000;
		for (int r = 0; r < TEMPLATE_SIZE; r++)
		{
			for (int c = 0; c < TEMPLATE_SIZE; c++)
			{
				if (m_grids[r][c].val == 0 && m_grids[r][c].nRemainCount < minv)
				{
					minv = m_grids[r][c].nRemainCount;
					m_curX = c; 
					m_curY = r;
				}
			}
		}

		//有一个空格子已经没有数可以选择了
		if (minv == 0)
		{
			m_curX = -1;
			m_curY = -1;
		}
	}

	void PrintBoard()
	{
		for (int i = 0; i <TEMPLATE_SIZE;i++)
		{
			for (int j = 0; j< TEMPLATE_SIZE;j++)
			{
				cout << m_grids[i][j].val << " ";
			}
			cout << endl;
		}
	}
	Grid m_grids[TEMPLATE_SIZE][TEMPLATE_SIZE];
	std::stack<pair<int,int> > posTrace;  
	int m_nRemained;
	int m_curX;
	int m_curY;	
};

void Solve(SUDOKUState& state)
{
	if (bSolved)
	{
		return;
	}
	gCount++;
	if (state.IsFinal())
	{
		state.PrintBoard();
		bSolved = true;
		return;
	}
	if (state.IsDead())
	{
		return;
	}

	for (int i = 1; i < 10; i++)
	{
		if (!state.CanPlace(i))
		{
			continue;
		}
		state.PlaceNumber(i);
		Solve(state);
		state.RemoveNumber(i);
	}
}
int main(int argc, char* argv[])
{
	SUDOKUState state(board);	
	state.PrintBoard();
	cout << endl;
	Solve(state);
	cout << gCount << " times called" << endl;
	return 0;
}