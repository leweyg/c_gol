#include <stdlib.h>
#include <stdio.h>

// GOL = Game of Life

typedef unsigned int uint;
typedef char GOLCell;
const GOLCell GOLCell_Empty  = '.';
const GOLCell GOLCell_Filled = 'X';

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
    }

    printf("Releasing...\n");
    GOLBoardRelease(front);
    GOLBoardRelease(back);

    printf("Done...\n");
    return 0;
}