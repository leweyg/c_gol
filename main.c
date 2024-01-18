#include <stdlib.h>
#include <stdio.h>

// GOL = Game of Life

typedef unsigned int uint;
typedef char GOLCell;
const GOLCell GOLCell_Empty  = '.';
const GOLCell GOLCell_Filled = 'X';
#define GOLCellAlive(v) ((v)==GOLCell_Filled)

typedef struct
{
    uint Width;
    uint Height;
    uint CellCount;
    GOLCell* Cells;
} GOLBoard;


void GOLBoardFill(GOLBoard* pBoard, GOLCell val)
{
    int n = pBoard->CellCount;
    for (int i=0; i<n; i++) {
        pBoard->Cells[i] = val;
    }
}

uint GOLBoardIndexSafe(GOLBoard* pBoard, uint x, uint y)
{
    uint sx = (x % pBoard->Width);
    uint sy = (y % pBoard->Height);
    uint index = (sx + (sy * pBoard->Width));
    return index;
}

GOLCell GOLBoardReadSafe(GOLBoard* pBoard, uint x, uint y)
{
    uint index = GOLBoardIndexSafe(pBoard, x, y);
    return pBoard->Cells[index];
}

void GOLBoardWriteSafe(GOLBoard* pBoard, uint x, uint y, GOLCell val)
{
    uint index = GOLBoardIndexSafe(pBoard, x, y);
    pBoard->Cells[index] = val;
}

GOLBoard* GOLBoardCreate(uint w, uint h)
{
    GOLBoard* pBoard = (GOLBoard*)malloc(sizeof(GOLBoard));
    pBoard->Width = w;
    pBoard->Height = h;
    pBoard->CellCount = w * h;
    pBoard->Cells = (GOLCell*)malloc(sizeof(GOLCell) * pBoard->CellCount);
    GOLBoardFill(pBoard, GOLCell_Empty);
    return pBoard;
}

void GOLBoardRelease(GOLBoard* pBoard)
{
    free(pBoard->Cells);
    free(pBoard);
}

uint GOLBoardAliveNeighborCount(GOLBoard* pBoard, uint x, uint y)
{
    uint count = 0;
    const uint negX = pBoard->Width - 1; // same as -1 in wrapped space
    const uint negY = pBoard->Height - 1;

    for (uint dy=0; dy<=2; dy++) {
        uint ny = y + negY + dy;

        for (uint dx=0; dx<=2; dx++) {
            uint nx = x + negX + dx;

            // exclude the center point:
            if ((dx == 1) && (dy == 1)) continue;

            GOLCell val = GOLBoardReadSafe(pBoard, nx, ny);
            if (GOLCellAlive(val)) {
                count++;
            }
        }
    }
    return count;
}

void GOLBoardStep(GOLBoard* pFrom, GOLBoard* pTo)
{
    uint w = pFrom->Width;
    uint h = pTo->Width;
    if (pTo->Width != pFrom->Width || pTo->Height != pFrom->Height) {
        printf("ERROR: Mismatched boards during iteration!\n");
        return;
    }
    for (uint y=0; y<h; y++) {
        for (uint x=0; x<w; x++) {
            const uint index = GOLBoardIndexSafe(pFrom, x, y);

            // read current:
            const GOLCell center = pFrom->Cells[index];
            const uint count = GOLBoardAliveNeighborCount(pFrom, x, y);

            // growth rule:
            GOLCell nextValue = GOLCell_Empty;
            if (GOLCellAlive(center)) {
                if ((count == 2) || (count == 3)) {
                    nextValue = GOLCell_Filled;
                }
            } else {
                if (count == 3) {
                    nextValue = GOLCell_Filled;
                }
            }

            // write result:
            pTo->Cells[index] = nextValue;
        }
    }
}

void GOLBoardPrint(GOLBoard* pBoard)
{
    for (int y=0; y<pBoard->Height; y++) {
        for (int x=0; x<pBoard->Width; x++) {
            GOLCell cell = GOLBoardReadSafe(pBoard, x, y);
            printf("%c", cell);
        }
        printf("\n");
    }
    printf("\n");
}

void GOLBoardDrawBlinker(GOLBoard* pBoard) {
    GOLBoardWriteSafe(pBoard, 4, 3, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 4, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 5, GOLCell_Filled);
}


int main(int argc, char* argv[])
{
    printf("Starting...\n");
    uint boardSize = 8;
    GOLBoard* front = GOLBoardCreate(boardSize, boardSize);
    GOLBoard* back = GOLBoardCreate(boardSize, boardSize);

    printf("Seeding...\n");
    GOLBoardDrawBlinker(front);

    printf("Generations...\n");
    uint genMax = 4;
    for (uint genIndex=0; genIndex<genMax; genIndex++)
    {
        GOLBoardPrint(front);

        // next step:
        GOLBoardStep(front, back);
        // swap front and back:
        GOLBoard* t = front;
        front = back;
        back = t;
    }

    printf("Releasing...\n");
    GOLBoardRelease(front);
    GOLBoardRelease(back);

    printf("Done...\n");
    return 0;
}