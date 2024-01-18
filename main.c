#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

// GOL = Game of Life

typedef unsigned int uint;
typedef char GOLCell;
const GOLCell GOLCell_Empty  = '.';
const GOLCell GOLCell_Filled = 'X';
#define GOLCellAlive(v) ((v)==GOLCell_Filled)

// GOLBoardCreate - basic Game of Life board mechansim.
//  Lifetime: GOLBoardCreate, GOLBoardRelease
//  Read/Write: GOLBoardReadSafe, GOLBoardWriteSafe
//  Print/Draw: GOLBoardPrint, GOLBoardDrawNamed
//  Step: GOLBoardStep
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

void GOLBoardStepCell(GOLBoard* pFrom, GOLBoard* pTo, uint x, uint y)
{
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
            // NOTE: this could be safely parallelized:
            GOLBoardStepCell(pFrom, pTo, x, y);
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

// GOLBoard filling/drawing:

void GOLBoardDrawBlinker(GOLBoard* pBoard) {
    GOLBoardWriteSafe(pBoard, 4, 3, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 4, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 5, GOLCell_Filled);
}

void GOLBoardDrawToad(GOLBoard* pBoard) {
    GOLBoardWriteSafe(pBoard, 3, 3, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 3, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 5, 3, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 2, 4, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 3, 4, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, 4, 4, GOLCell_Filled);
}

void GOLBoardDrawCube(GOLBoard* pBoard, uint x, uint y)
{
    GOLBoardWriteSafe(pBoard, x+0, y+0, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, x+1, y+0, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, x+0, y+1, GOLCell_Filled);
    GOLBoardWriteSafe(pBoard, x+1, y+1, GOLCell_Filled);
}

void GOLBoardDrawBeacon(GOLBoard* pBoard) {
    GOLBoardDrawCube(pBoard, 5, 1);
    GOLBoardDrawCube(pBoard, 3, 3);
}

void GOLBoardDrawRandom(GOLBoard* pBoard) {
    uint n = pBoard->CellCount;
    for (uint index=0; index<n; index++) {
        if ((rand()%2) == 0) {
            pBoard->Cells[index] = GOLCell_Filled;
        }
    }
}

static int custom_charicmp(char a, char b)
{
    int la = tolower(a);
    int lb = tolower(b);
    return (la - lb);
}

static int custom_stricmp(const char* cstrA, const char* cstrB)
{
    if ((!cstrA) && (!cstrB)) {
        return 0;
    }
    if ((!cstrA) || (!cstrB)) {
        return (cstrA ? 1 : -1);
    }
    const char* a = cstrA;
    const char* b = cstrB;
    int res = custom_charicmp(*a, *b);
    while ((res==0) && *a) {
        a++;
        b++;
        res = custom_charicmp(*a, *b);
    }
    return res;
}

void GOLBoardDrawNamed(GOLBoard* pBoard, const char* name) {
    const char* defaultName = "RANDOM";
    if (!name) {
        name = defaultName;
    }
    if (custom_stricmp(name, "Blinker")==0) {
        GOLBoardDrawBlinker(pBoard);
    } else if (custom_stricmp(name, "Toad")==0) {
        GOLBoardDrawToad(pBoard);
    } else if (custom_stricmp(name, "Beacon")==0) {
        GOLBoardDrawBeacon(pBoard);
    } else {
        // default or RANDOM:
        name = defaultName;
        GOLBoardDrawRandom(pBoard);
    }
    printf("%s\n", name);
}

int main(int argc, char* argv[])
{
    //printf("Starting...\n");
    uint boardSize = 8;
    GOLBoard* front = GOLBoardCreate(boardSize, boardSize);
    GOLBoard* back = GOLBoardCreate(boardSize, boardSize);

    //printf("Seeding...\n");
    GOLBoardDrawNamed(front, (argc > 1) ? argv[1] : NULL);

    //printf("Generations...\n");
    uint genMax = 4;
    for (uint genIndex=0; genIndex<genMax; genIndex++)
    {
        GOLBoardPrint(front);

        // next step:
        GOLBoardStep(front, back);
        // swap front and back buffer:
        GOLBoard* t = front;
        front = back;
        back = t;
    }

    //printf("Releasing...\n");
    GOLBoardRelease(front);
    GOLBoardRelease(back);

    //printf("Done...\n");
    return 0;
}