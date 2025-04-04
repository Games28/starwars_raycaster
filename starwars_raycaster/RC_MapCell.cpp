#include "RC_MapCell.h"


//////////////////////////////////  MAP CELL BLUEPRINTS  //////////////////////////////////////

// ==============================/  functions for MapCellBluePrint  /==============================

// The library of map cells is modeled as a std::map, for fast (O(n log n)) searching
std::map<char, MapCellBluePrint> mMapCellBluePrintLib;

// Convenience function to add one map cell configuration
void AddMapCellBluePrint(MapCellBluePrint& rMBP) {
    mMapCellBluePrintLib.insert(std::make_pair(rMBP.cID, rMBP));
}

// Uses the data from vInitMapCellBluePrints to populate the library of map cells (mMapCellBluePrintLib)
// This construction decouples the blue print definition from its use, and enables error checking
// on the blue print data
void InitMapCellBluePrints() {
    for (auto elt : vInitMapCellBluePrints) {
        AddMapCellBluePrint(elt);
    }
}

// return a reference to the block in the library having id cID
MapCellBluePrint& GetMapCellBluePrint(char cID) {
    std::map<char, MapCellBluePrint>::iterator itMapCellBP = mMapCellBluePrintLib.find(cID);
    if (itMapCellBP == mMapCellBluePrintLib.end()) {
        std::cout << "ERROR: GetMapCellBluePrint() --> can't find element with ID: " << cID << std::endl;
    }
    return (*itMapCellBP).second;
}

// ==============================/  class RC_MapCell  /==============================

RC_MapCell::RC_MapCell() {}
RC_MapCell::~RC_MapCell() {}

void RC_MapCell::Init(int px, int py, int l) {
    x = px;
    y = py;
    layer = l;
}

int RC_MapCell::GetX() { return x; }
int RC_MapCell::GetY() { return y; }
int RC_MapCell::GetLayer() { return layer; }

void RC_MapCell::SetX(int px) { x = px; }
void RC_MapCell::SetY(int py) { y = py; }
void RC_MapCell::SetLayer(int nLayer) { layer = nLayer; }

void RC_MapCell::Update(float fElapsedTime, bool& bPermFlag) {
    if (!bEmpty) {
        for (int i = 0; i < FACE_NR_OF; i++) {
            pFaces[i]->Update(fElapsedTime, bPermFlag);
        }
    }
}

// if this is an empty map cell sampling will return olc::BLANK
olc::Pixel RC_MapCell::Sample(int nFaceIx, float sX, float sY) {
    if (bEmpty) {
        return olc::BLANK;
    }
    else if (nFaceIx < 0 || nFaceIx >= FACE_NR_OF) {
        std::cout << "WARNING: RC_MapCell::Sample() --> face index out of range: " << nFaceIx << std::endl;
        return olc::MAGENTA;
    }

    return pFaces[nFaceIx]->Sample(sX, sY);
}

void RC_MapCell::SetTexturePixel(int nFaceIx, float sX, float sY,olc::Pixel p)
{
    pFaces[nFaceIx]->setPixel(sX, sY, p);
}

char RC_MapCell::GetID() { return id; }
void RC_MapCell::SetID(char cID) { id = cID; }

float RC_MapCell::GetHeight() { return height; }
void  RC_MapCell::SetHeight(float fH) { height = fH; }

bool RC_MapCell::IsEmpty() { return bEmpty; }
bool RC_MapCell::IsPermeable() { return bPermeable; }

void RC_MapCell::SetEmpty(bool bParam) { bEmpty = bParam; }
void RC_MapCell::SetPermeable(bool bParam) { bPermeable = bParam; }

void RC_MapCell::SetFacePtr(int nFaceIx, RC_Face* pFace) {
    if (nFaceIx < 0 || nFaceIx >= FACE_NR_OF) {
        std::cout << "WARNING: SetFacePtr() --> face index out of range: " << nFaceIx << std::endl;
    }
    else {
        pFaces[nFaceIx] = pFace;
    }
}

RC_Face* RC_MapCell::GetFacePtr(int nFaceIx) {
    RC_Face* result = nullptr;
    if (nFaceIx < 0 || nFaceIx >= FACE_NR_OF) {
        std::cout << "WARNING: GetFacePtr() --> face index out of range: " << nFaceIx << std::endl;
    }
    else {
        result = pFaces[nFaceIx];
        if (result == nullptr) {
            std::cout << "FATAL: GetFacePtr() --> nullptr result for face index: " << nFaceIx << std::endl;
        }
    }
    return result;
}

bool RC_MapCell::IsDynamic() { return false; }

// ==============================/  class RC_MapCellDynamic  /==============================


RC_MapCellDynamic::RC_MapCellDynamic() {}

// NOTE: contains hardcoded stuff currently!
void RC_MapCellDynamic::Init(int px, int py, int l) {
    x = px;
    y = py;
    layer = l;

    fTimer = 0.0f;   // local timer for this dynamic map cell
    fTickTime = 0.05f;  // tick every ... seconds
    nCounter = 0;      // keep track of nr of ticks
    nNrSteps = 101;      // cycle every ... ticks
}

// NOTE: contains hardcoded stuff currently!
void RC_MapCellDynamic::Update(float fElapsedTime, bool& bPermFlag) {
    // first update all the faces of this block
    if (!bEmpty) {
        for (int i = 0; i < FACE_NR_OF; i++) {
            pFaces[i]->Update(fElapsedTime, bPermFlag);
        }
    }
    // then update the block itself
    fTimer += fElapsedTime;
    if (fTimer >= fTickTime) {
        // if the threshold is small, 1 frame could exceed the threshold multiple times
        while (fTimer >= fTickTime) {
            fTimer -= fTickTime;
            // one tick gone by, advance counter
            nCounter += 1;
        }
        if (nCounter >= nNrSteps) {
            // animation sequence reverses
            nCounter -= nNrSteps;
            bUp = !bUp;
        }
        else {
            height = (bUp ? float(nCounter) / 100.0f : 1.0f - float(nCounter) / 100.0f);
        }
    }
}

bool RC_MapCellDynamic::IsEmpty() { return bEmpty; }

bool RC_MapCellDynamic::IsDynamic() { return true; }

void WallDismantle::prepboundry(olc::PixelGameEngine* pge, int wallheight)
{
    int tophalf = (pge->ScreenHeight() / 2) - (wallheight / 2);
    int bottomhalf = (pge->ScreenHeight() / 2) + (wallheight / 2);
    middleheight = (tophalf + bottomhalf) / 2;
    middlewidth = pge->ScreenWidth() / 2;
    

    top = middleheight - walldimension;
    bottom = middleheight + walldimension;
    left = middlewidth - walldimension;
    right = middlewidth + walldimension;

    maxcount = top + bottom + left + right;
}

bool WallDismantle::outsideboundry(int pixel_x, int pixel_y)
{
    if (pixel_x >= left && pixel_x <= right &&
        pixel_y >= top && pixel_y <= bottom)
    {
        return false;
    }
    else
    {

        return true;
    }

}

bool WallDismantle::withinboundry(int pixel_x, int pixel_y)
{
    if (pixel_x >= left && pixel_x <= right &&
        pixel_y >= top && pixel_y <= bottom)
    {
        return true;
    }
    else
    {

        return false;
    }
}

void WallDismantle::addChunkinfo(float sampleX, float sampleY, float mapx, float mapy, olc::Sprite* spr)
{
   
   
        if (chunkinfo.mapX == 0.0f) chunkinfo.mapX = mapx;
        if (chunkinfo.mapY == 0.0f) chunkinfo.mapY = mapy;
        if (chunkinfo.sprite == nullptr) chunkinfo.sprite = spr;
        olc::vf2d coords = { sampleX,sampleY };
        chunkinfo.samplecoords.push_back(coords);
    
    
}

void WallDismantle::ClearChunkinfo()
{
    chunkinfo.mapX = 0.0f;
    chunkinfo.mapY = 0.0f;
    chunkinfo.samplecoords.clear();
    chunkinfo.sprite = nullptr;
}





int WallDismantle::getboundrywidth()
{
    return right - left;
}

int WallDismantle::getboundryheight()
{
    return bottom - top;
}

void WallDismantle::abjustboundrysize(olc::PixelGameEngine* pge)
{
    if (pge->GetKey(olc::LEFT).bHeld) walldimension += 0.2f;
    if (pge->GetKey(olc::RIGHT).bHeld) walldimension -= 0.2f;
}