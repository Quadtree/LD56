#pragma once

#define MAX_PER_CELL 20

template <class T>
class GridCell
{
public:
    T List[MAX_PER_CELL];
    int NumInCell;
};

template <int Radius, int GridSize, class T>
class SpatialIndex
{
public:
    GridCell<T> Cells[(GridSize / Radius) * (GridSize / Radius)];

    int32_t CellXYToCellID(int32_t x, int32_t y)
    {
        return y * (GridSize / Radius) + x;
    }

    int32_t Vector2ToCellID(const Vector2 &v2)
    {
        return CellXYToCellID(
            (int32_t)((v2.X / Radius) + 0.5f),
            (int32_t)((v2.Y / Radius) + 0.5f));
    }

    void ClearCells()
    {
        for (auto y = 0; y < GridSize / Radius; ++y)
        {
            for (auto x = 0; x < GridSize / Radius; ++x)
            {
                Cells[CellXYToCellID(x, y)].NumInCell = 0;
            }
        }
    }

    void AddToIndex(T *bacteria)
    {
        auto cellId = Vector2ToCellID(bacteria->Position);

        if (Cells[cellId].NumInCell < MAX_PER_CELL)
        {
            Cells[cellId].List[Cells[cellId].NumInCell] = bacteria;
            Cells[cellId].NumInCell++;
        }
    }
};