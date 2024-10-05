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

    int32_t CellXYToCellID(int32_t x, int32_t y)
    {
        return y * (GridSize / Radius) + x;
    }

    void ClearCells()
    {
        for (auto y = 0; y < GridSize / Radius; ++y)
        {
            for (auto y = 0; y < GridSize / Radius; ++y)
            {
                Cells[CellXYToCellID(x, y)]->NumInCell = 0;
            }
        }
    }
};