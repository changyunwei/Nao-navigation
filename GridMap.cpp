#include "stdafx.h"
#include "GridMap.h"
#include <QDebug>

GridMap::GridMap(void)
{
    // Initial GridMap
    for( int i=0; i < GRIDMAP_W; i++ )
    {
        for( int j=0; j<GRIDMAP_H; j++ )
        {
            grid[i][j] = VALUE_EMPTY;                        ///grid[][] ==0 indicates, current grid is empty;
        }
    }

    buildEnvironment();
}

GridMap::~GridMap(void)
{
}

void GridMap::buildEnvironment()
{
	// Define horizontal wall
	for (int i=0; i < GRIDMAP_W+1; i++)
	{
		// 1st
		grid[i][0] = VALUE_OBSTACLE;
		grid[i][GRIDMAP_ROOM_H-1] = VALUE_OBSTACLE;
		// 2nd
		grid[i][GRIDMAP_ROOM_H] = VALUE_OBSTACLE;
		grid[i][2*GRIDMAP_ROOM_H-1] = VALUE_OBSTACLE;
		// 3rd
		grid[i][2*GRIDMAP_ROOM_H] = VALUE_OBSTACLE;
		grid[i][3*GRIDMAP_ROOM_H-1] = VALUE_OBSTACLE;
		// 4th
		grid[i][3*GRIDMAP_ROOM_H] = VALUE_OBSTACLE;
		grid[i][GRIDMAP_H-1] = VALUE_OBSTACLE;
	}
	// Define vertical wall
	for (int i=0; i < GRIDMAP_H; i++)
	{
		grid[0][i] = VALUE_OBSTACLE;
		grid[GRIDMAP_ROOM_W][i] = VALUE_OBSTACLE;
		grid[GRIDMAP_ROOM_W + GRIDMAP_CORRIDOR_W+1][i] = VALUE_OBSTACLE;
		grid[GRIDMAP_W][i] = VALUE_OBSTACLE;
	}
	// Open the door, door width 4
	for (int i=0; i < 4; i++)
	{
		grid[GRIDMAP_ROOM_W][GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W][3*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W][5*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W][7*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W + GRIDMAP_CORRIDOR_W+1][GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W + GRIDMAP_CORRIDOR_W+1][3*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W + GRIDMAP_CORRIDOR_W+1][5*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W + GRIDMAP_CORRIDOR_W+1][7*GRIDMAP_ROOM_H/2-2+i] = VALUE_EMPTY;
	}
	// Open the corridor
	for (int i=0; i < GRIDMAP_CORRIDOR_W; i++)
	{
		grid[GRIDMAP_ROOM_W +1 + i][GRIDMAP_ROOM_H] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W +1 + i][GRIDMAP_ROOM_H-1] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W +1 + i][2*GRIDMAP_ROOM_H] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W +1 + i][2*GRIDMAP_ROOM_H-1] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W +1 + i][3*GRIDMAP_ROOM_H] = VALUE_EMPTY;
		grid[GRIDMAP_ROOM_W +1 + i][3*GRIDMAP_ROOM_H-1] = VALUE_EMPTY;
	}


}

void GridMap::findNewPath(  )
{
    /**
    * set a timer count for every 2s updating the path, in urbiscript the fps of inertial is 4, so here
    *  we set the timeCount is 8 to make sure every 2s updating the path
    */
    // First clear the old path 
    if ( !path.isEmpty() )
    {
        /**
        for (int i =0; i < path.size(); i++)
        {
        grid[path[i].row][path[i].col] = 0;
        }
        */
        for(int i=0;i<GRIDMAP_W+1;i++)
        {
            for(int j=0;j<GRIDMAP_H+1;j++)
            {
                if(grid[i][j]!=VALUE_OBSTACLE &&grid[i][j]!=VALUE_LANDMARK_L&& grid[i][j]!=VALUE_LANDMARK_R)
                    grid[i][j]=0;
            }
        }

        path.clear();
    }

    // Then calculate the new path		
    bool flag = findPath( curPos, nextPos, path );

    // If successfully find the path
    if ( flag )
    {
        // Mark the new path and copy to "oldPath"
        //oldPath = path;

        for ( int i=0; i < path.size(); i++ )
        {
            // Indicate it is the path
            grid[path[i].gcx][path[i].gcy] = VALUE_PATH;
        }
    }
}

bool GridMap::findPath(GridCell start, GridCell finish, QQueue<GridCell> &newPath)
{
    if (( start.gcx == finish.gcx ) && ( start.gcy == finish.gcy ))
    { 
        newPath.clear(); 
        return true; 
    }

    //GridCell direction[8];
	GridCell direction[4];
    /*
    offset[0].row = -1; offset[1].col = 0; // up
    offset[1].row = -1; offset[5].col = -1; // Left up
    offset[2].row = 0; offset[2].col = -1; // left
    offset[3].row = 1; offset[6].col = -1; // left down
    offset[4].row = 1; offset[3].col = 0; // down
    offset[5].row = 1; offset[7].col = 1; //  right down 
    offset[6].row = 0; offset[0].col = 1; // right
    offset[7].row = -1; offset[4].col = 1; // right up
    */

	/*
    direction[0].gcx = 0; direction[0].gcy = 1; // right
    direction[1].gcx = -1; direction[1].gcy = 0; // up
    direction[2].gcx = 0; direction[2].gcy = -1; // left
    direction[3].gcx = 1; direction[3].gcy = 0; // down
    direction[4].gcx = -1; direction[4].gcy = 1; // right up
    direction[5].gcx = -1; direction[5].gcy = -1; // Left up
    direction[6].gcx = 1; direction[6].gcy = -1; // left down
    direction[7].gcx = 1; direction[7].gcy = 1; //  right down 

	*/
	direction[0].gcx = -1; direction[0].gcy = 0; // up
	direction[1].gcx = 0; direction[1].gcy = 1; // right
	direction[2].gcx = 0; direction[2].gcy = -1; // left
	direction[3].gcx = 1; direction[3].gcy = 0; // down
	//direction[4].gcx = -1; direction[4].gcy = 1; // right up
	//direction[5].gcx = -1; direction[5].gcy = -1; // Left up
	//direction[6].gcx = 1; direction[6].gcy = -1; // left down
	//direction[7].gcx = 1; direction[7].gcy = 1; //  right down 




    // Eight directions
    //int NumOfNbrs = 8;
	int NumOfNbrs = 4;


    GridCell here, nbr/*nbrMore*/;

    here.gcx = start.gcx;
    here.gcy = start.gcy;

    // ·âËø???
    grid[start.gcx][start.gcy] = 2; 

    //////////////////////////////////////////////////////////////////////////
    // Label the area can be reached
    QQueue<GridCell> Q;

    do
    { 
        for ( int i = 0; i < NumOfNbrs; i++ ) 
        {
            // Next grid if go "offset[i]" direction
            nbr.gcx = here.gcx + direction[i].gcx;
            nbr.gcy = here.gcy + direction[i].gcy;

            // Move more
            //nbrMore.row = nbr.row + offset[i].row;
            //nbrMore.col = nbr.col + offset[i].col;

            // If the grid is unoccupied
            if ( grid[nbr.gcx][nbr.gcy] == 0 )
            {
                // Unlabeled nbr, label it
                grid[nbr.gcx][nbr.gcy] = grid[here.gcx][here.gcy] + 1;
                // grid[nbr.row][nbr.col] = VALUE_REACHABLEAREA;
                //here = nbr;

                // If reach the destination!
                if ( ( nbr.gcx == finish.gcx ) && ( nbr.gcy == finish.gcy ) )
                    break;

                // Put the "grid" in the path
                Q.enqueue(nbr);


            } // if ( grid[nbr.row][nbr.col] == 0 )

        } // for ( int i = 0; i < NumOfNbrs; i++ )

        if ( ( nbr.gcx == finish.gcx ) && ( nbr.gcy == finish.gcy ) ) 
            break; 

        // No path available!
        if ( Q.isEmpty() )
        {
            qDebug() << "No path available!";
            return false;
        }

        // Q.delete(here);
        here = Q.dequeue(); 

    } while(true);
    //////////////////////////////////////////////////////////////////////////

    // Delete old path
    newPath.clear();

    //newPath = Q;

    int mapSize = grid[finish.gcx][finish.gcy] - 2;
    for (int i=0; i<mapSize; i++)
    {
        newPath.enqueue(here);
    }

    here = finish;

    for (int j = mapSize-1; j >= 0; j--) 
    {
        newPath[j] = here;

        for ( int i = 0; i < NumOfNbrs; i++ )
        {
            nbr.gcx = here.gcx + direction[i].gcx;
            nbr.gcy = here.gcy + direction[i].gcy;

            if ( grid[nbr.gcx][nbr.gcy] == j+2 )
                break;
        }

        // move to predecessor
        here = nbr;
    }

    return true;
}