#pragma once

#define MAX_PER_CELL 20

class GridCell
{
public:
    Bacteria List[MAX_PER_CELL];
    int NumInCell;
};

template <int Radius, int GridSize>
class SpatialIndex
{
public:
    GridCell Cells[(GridSize / Radius) * (GridSize / Radius)];
};