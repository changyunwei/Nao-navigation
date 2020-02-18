#ifndef GRID_MAP_H
#define GRID_MAP_H

#include <QQueue>

#define ROOM_W             5870
#define ROOM_H             4050
#define CORRIDOR_W         2250
#define MAP_W              (ROOM_W*2+CORRIDOR_W)
#define MAP_H              (ROOM_H*4)
#define FACTOR             250
#define GRIDMAP_ROOM_W     (ROOM_W / FACTOR)
#define GRIDMAP_ROOM_H     (ROOM_H / FACTOR)
#define GRIDMAP_CORRIDOR_W (CORRIDOR_W / FACTOR)
#define GRIDMAP_W          (MAP_W / FACTOR)
#define GRIDMAP_H          (GRIDMAP_ROOM_H * 4)
#define GRIDCELL_SIZE      10

#define VALUE_EMPTY          0
#define VALUE_OBSTACLE      -1
#define VALUE_PATH          -2
//#define VALUE_REACHABLEAREA  3
//#define VALUE_BLOCKAREA      2
#define VALUE_LANDMARK_L    -10
#define VALUE_LANDMARK_R    -11

class GridCell
{
public:

	int gcx;
	int gcy;
};


class GridMap
{
public:
	GridMap(void);
	~GridMap(void);

    void buildEnvironment();

	// path planning
	void findNewPath();

	bool findPath(GridCell start, GridCell finish, QQueue<GridCell>& newPath);

public:
    // 2D Grid Map;
    int grid[GRIDMAP_W+1][GRIDMAP_H+1];

    // Index of grid map, rows and cols
    GridCell curPos;
    GridCell nextPos;
    // Current path, a group of grid cells
    QQueue<GridCell> path;

};

#endif