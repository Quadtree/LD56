#pragma once

#define MAX_PER_CELL 20

template <class T>
class GridCell
{
public:
    T *List[MAX_PER_CELL];
    int NumInCell;
};

template <int Radius, int GridSize, class T>
class SpatialIndex
{
public:
    GridCell<T> Cells[(GridSize / Radius) * (GridSize / Radius)];

    int32_t CellXYToCellID(int32_t x, int32_t y) const
    {
        if (x < 0 || x >= GridSize / Radius || y < 0 || y >= GridSize / Radius)
            return -1;

        return y * (GridSize / Radius) + x;
    }

    int32_t Vector2ToCellID(const Vector2 &v2) const
    {
        return CellXYToCellID(
            (int32_t)((v2.X / Radius) + 0.5f) + (GridSize / Radius / 2),
            (int32_t)((v2.Y / Radius) + 0.5f) + (GridSize / Radius / 2));
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
        const Vector2 OFFSETS[] = {
            Vector2(0, 0),

            Vector2(-1, 0),
            Vector2(1, 0),
            Vector2(0, 1),
            Vector2(0, -1),

            Vector2(-1, -1),
            Vector2(1, 1),
            Vector2(-1, 1),
            Vector2(-1, -1),
        };

        for (auto &it : OFFSETS)
        {
            auto cellId = Vector2ToCellID(bacteria->Position + it * Radius);

            if (cellId == -1)
                return;

            if (Cells[cellId].NumInCell < MAX_PER_CELL)
            {
                Cells[cellId].List[Cells[cellId].NumInCell] = bacteria;
                Cells[cellId].NumInCell++;
            }
        }
    }
};