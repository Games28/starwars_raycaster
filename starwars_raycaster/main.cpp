#include <cfloat>       // needed for constant FLT_MAX in the DDA function

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// ==============================/  specific include files   /==============================
#include "Defs.h"
#include "RC_Misc.h"
#include "RC_Face.h"
#include "RC_MapCell.h"
#include "RC_Map.h"
#include "RC_DepthDrawer.h"
#include "RC_Objects.h"
#include "Player.h"
#include "RC_RayCast.h"
#include "RC_ObjectManager.h"
// Screen and pixel constants - keep the screen sizes constant and vary the resolution by adapting the pixel size
// to prevent accidentally defining too large a window


// ==============================/  map definition here   /==============================

#include "Mapeditor.h"

// ==============================/  PGE derived ray caster engine   /==============================


class MyRayCaster : public olc::PixelGameEngine {

public:
    MyRayCaster() {    // display the screen and pixel dimensions in the window caption
        sAppName = "MyRayCaster - Permadi tutorial elaborations - S:(" + std::to_string(SCREEN_X / PIXEL_SIZE) + ", " + std::to_string(SCREEN_Y / PIXEL_SIZE) + ")" +
            ", P:(" + std::to_string(PIXEL_SIZE) + ", " + std::to_string(PIXEL_SIZE) + ")";
    }

private:
    // definition of the map object
    RC_Map cMap;
    //float fMaxDistance;           // max visible distance - use length of map diagonal to overlook whole map
    Player player;
    RC_ObjectManager OM;

   

    //float fAnglePerPixel_deg;
    //float fDistToProjPlane;         // constant distance to projection plane - is calculated in OnUserCreate()

    // all sprites for texturing the scene and the objects, grouped in categories
    //std::vector<olc::Sprite*> vWallSprites;
    //std::vector<olc::Sprite*> vCeilSprites;
    //std::vector<olc::Sprite*> vRoofSprites;
    //std::vector<olc::Sprite*> vFlorSprites;
    //std::vector<olc::Sprite*> vObjtSprites;

    // var's and initial values for shading - trigger keys INS and DEL
    //float fObjectIntensity = MULTI_LAYERS ? OBJECT_INTENSITY : 0.2f;
   // float fIntensityMultiplier = MULTI_LAYERS ? MULTIPLIER_INTENSITY : 10.0f;

    // toggles for rendering
    bool bMinimap = false;    // toggle on mini map rendering (trigger key P)
    bool bMapRays = false;    //           rays in mini map   (trigger key O)
    bool bPlayerInfo = false;    //           player info hud    (trigger key I)
    bool bProcessInfo = false;    //           process info hud   (trigger key U)
    bool bTestSlice = false;    //           visible test slice (trigger key G)
    bool bTestGrid = false;    //           visible test grid  (trigger key H)

    typedef struct sRayStruct {
        olc::vf2d pointA, pointB;
        int layer;
    } RayType;
    std::vector<RayType> vRayList;    // needed for ray rendering in minimap

    std::list<RC_Object> vListObjects;     // list of all objects in the game

    // which of the slices to output in test mode
    float fTestSlice;

    RC_DepthDrawer cDDrawer;              // depth drawing object
    Raycast ray;
public:

    bool OnUserCreate() override {

        bool bSuccess = true;

        srand(time(0));

        // initialize sine and cosine lookup arrays - these are meant for performance improvement
        init_lu_sin_array();
        init_lu_cos_array();
        
        // Work out distance to projection plane. This is a constant float value, depending on the width of the projection plane and the field of view.
        cMap.fDistToProjPlane = ((ScreenWidth() / 2.0f) / lu_sin(player.fPlayerFoV_deg / 2.0f)) * lu_cos(player.fPlayerFoV_deg / 2.0f);

        // lambda expression for loading sprite files with error checking
        auto load_sprite_file = [=](const std::string& sFileName) {
            olc::Sprite* tmp = new olc::Sprite(sFileName);
            if (tmp->width == 0 || tmp->height == 0) {
                std::cout << "ERROR: OnUserCreate() --> can't load file: " << sFileName << std::endl;
                delete tmp;
                tmp = nullptr;
            }
            return tmp;
            };
        // lambda expression for loading all sprites for one category (walls, ceilings, roofs, floors or objects) into the
        // associated container
        auto load_sprites_from_files = [=](std::vector<std::string>& vFileNames, std::vector<olc::Sprite*>& vSpritePtrs) {
            bool bNoErrors = true;
            for (auto& sf : vFileNames) {
                olc::Sprite* tmpPtr = load_sprite_file(sf);
                bNoErrors &= (tmpPtr != nullptr);
                vSpritePtrs.push_back(tmpPtr);
            }
            return bNoErrors;
            };
        // load all sprites into their associated container
        bSuccess &= load_sprites_from_files(vWallSpriteFiles, ray.vWallSprites); std::cout << "Loaded: " << (int)vWallSpriteFiles.size() << " files into " << (int)ray.vWallSprites.size() << " wall sprites." << std::endl;
        bSuccess &= load_sprites_from_files(vCeilSpriteFiles, ray.vCeilSprites); std::cout << "Loaded: " << (int)vCeilSpriteFiles.size() << " files into " << (int)ray.vCeilSprites.size() << " ceiling sprites." << std::endl;
        bSuccess &= load_sprites_from_files(vRoofSpriteFiles, ray.vRoofSprites); std::cout << "Loaded: " << (int)vRoofSpriteFiles.size() << " files into " << (int)ray.vRoofSprites.size() << " roof sprites." << std::endl;
        bSuccess &= load_sprites_from_files(vFlorSpriteFiles, ray.vFlorSprites); std::cout << "Loaded: " << (int)vFlorSpriteFiles.size() << " files into " << (int)ray.vFlorSprites.size() << " floor sprites." << std::endl;
        bSuccess &= load_sprites_from_files(vObjtSpriteFiles, ray.vObjtSprites); std::cout << "Loaded: " << (int)vObjtSpriteFiles.size() << " files into " << (int)ray.vObjtSprites.size() << " object sprites." << std::endl << std::endl;

        // fill the library of face blueprints
        InitFaceBluePrints();
        // fill the library of map cell blueprints
        InitMapCellBluePrints();

        // create and fill the map - the map itself is defined in a separate file
        // NOTES: 1) string arguments in AddLayer() must match x and y dimensions in InitMap()!
        //        2) the parameters vWallSprites, vCeilSprites and vRoofSprites must be initialised
        cMap.InitMap(glbMapX, glbMapY);
        for (int i = 0; i < (int)vMap_layer.size(); i++) {
            cMap.AddLayer(vMap_layer[i], ray.vWallSprites, ray.vCeilSprites, ray.vRoofSprites);
        }

        // max ray length for DDA is diagonal length of the map
        cMap.fMaxDistance = cMap.DiagonalLength();

        // aux map to keep track of placed objects
        // count nr of occupied cells at the same time
        std::string sObjMap;
        int nTilesOccupied = 0;
        for (int y = 0; y < cMap.Hight(); y++) {
            for (int x = 0; x < cMap.Width(); x++) {
                sObjMap.append(" ");
                if (cMap.CellHeight(x, y) != 0.0f) {
                    nTilesOccupied += 1;
                }
            }
        }
       

        // set initial test slice value at middle of screen
        fTestSlice = ScreenWidth() / 2.0f;
        // determine how much degrees one pixel shift represents.
        ray.fAnglePerPixel_deg = player.fPlayerFoV_deg / ScreenWidth();
        // determine the player field of view in radians as well
        player.fPlayerFoV_rad = deg2rad(player.fPlayerFoV_deg);
        // initialise the depth drawer object
        cDDrawer.Init(this);

        OM.initailize(cMap);

        return bSuccess;
    }

    // Holds intersection point in float (world) coordinates and in int (tile) coordinates,
    // the distance to the intersection point and the height of the map at these tile coordinates
   
    void PrintHitPoint(IntersectInfo& p, bool bVerbose) {
        std::cout << "hit (world): ( " << p.fHitX << ", " << p.fHitY << " ) ";
        std::cout << "hit (tile): ( " << p.nHitX << ", " << p.nHitY << " ) ";
        std::cout << "dist.: " << p.fDistFrnt << " ";
        std::cout << "lvl: " << p.nLayer << " hght: " << p.fHeight << " ";
        if (bVerbose) {
            std::cout << "bot frnt: " << p.osp_bot_frnt << " bot back: " << p.osp_bot_back << " ";
            std::cout << "top frnt: " << p.osp_top_frnt << " top back: " << p.osp_top_back << " ";
            switch (p.nFaceHit) {
            case FACE_EAST: std::cout << "EAST";    break;
            case FACE_NORTH: std::cout << "NORTH";   break;
            case FACE_WEST: std::cout << "WEST";    break;
            case FACE_SOUTH: std::cout << "SOUTH";   break;
            case FACE_TOP: std::cout << "TOP";     break;
            case FACE_BOTTOM: std::cout << "BOTTOM";  break;
            case FACE_UNKNOWN: std::cout << "UNKNOWN"; break;
            default: std::cout << "ERROR: " << p.nFaceHit;
            }
        }
        std::cout << std::endl;
    }

    void PrintHitList(std::vector<IntersectInfo>& vHitList, bool bVerbose = false) {
        for (int i = 0; i < (int)vHitList.size(); i++) {
            std::cout << "Elt: " << i << " = ";
            PrintHitPoint(vHitList[i], bVerbose);
        }
        std::cout << std::endl;
    }

   

    // ==============================/   Mini map rendering prototypes   /==============================

        // function to render the mini map on the screen. If nRenderLevel == -1, all layers are taken into account.
        // Otherwise only the specified level is renderd in the minimap
    void RenderMap(int nRenderLevel = -1);
    void RenderMapPlayer();      // function to render the player in the mini map on the screen
    void RenderMapRays(int nPlayerLevel);        // function to render the rays in the mini map on the screen
    void RenderMapObjects();     // function to render all the objects in the mini map on the screen
    void RenderPlayerInfo();      // function to render player info in a separate hud on the screen
    void RenderProcessInfo();     // function to render process info in a separate hud on the screen

   // Shade the pixel p using fDistance as a factor in the shade formula

    int nTestAnimState = ANIM_STATE_CLOSED;

    bool OnUserUpdate(float fElapsedTime) override {

        // step 1 - user input
        // ===================
        Clear(olc::BLACK);
        // For all movements and rotation you can speed up by keeping SHIFT pressed
        // or speed down by keeping CTRL pressed. This also affects shading/lighting
        float fSpeedUp = 1.0f;
       

       
        player.processInput(this, cMap, fSpeedUp, fElapsedTime);
        // looking up or down - collision detection not necessary
        // NOTE - there's no clamping to extreme values (yet)
       
        // alter object intensity and multiplier
        if (GetKey(olc::INS).bHeld) ray.fObjectIntensity += INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(olc::DEL).bHeld) ray.fObjectIntensity -= INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(olc::HOME).bHeld) ray.fIntensityMultiplier += INTENSITY_SPEED * fSpeedUp * fElapsedTime;
        if (GetKey(olc::END).bHeld) ray.fIntensityMultiplier -= INTENSITY_SPEED * fSpeedUp * fElapsedTime;


        // step 2 - game logic
        // ===================

        bool bStateChanged = false;
        // directly setting to opened or closed is not useful. State can only become Opening if it was closed, and vice versa
        if (GetKey(olc::F6).bPressed) { bStateChanged = true; nTestAnimState = ANIM_STATE_CLOSING; }
        if (GetKey(olc::F5).bPressed) { bStateChanged = true; nTestAnimState = ANIM_STATE_OPENING; }

        // little lambda returns whether distance between b and c is <= a (note - sqrt not needed here)
        auto within_distance = [=](int a, int b, int c) {
            return (b * b + c * c) <= (a * a);
            };
        // iterate over all the map cells in the map
        for (int h = 0; h < cMap.NrOfLayers(); h++) {
            for (int y = 0; y < cMap.Hight(); y++) {
                for (int x = 0; x < cMap.Width(); x++) {

                    // grab a pointer to the current map cell
                    RC_MapCell* pMapCell = cMap.MapCellPtrAt(x, y, h);
                    if (!pMapCell->IsEmpty()) {
                        // update this map cell (this will update all it's faces)
                        bool bTmp = pMapCell->IsPermeable();
                        pMapCell->Update(fElapsedTime, bTmp);
                        pMapCell->SetPermeable(bTmp);

                        // test code for manually changing state of animated faces
                        for (int i = 0; i < FACE_NR_OF; i++) {
                            RC_Face* facePtr = pMapCell->GetFacePtr(i);
                            if (facePtr->IsAnimated()) {
                                // only trigger gate if close enough
                                if (bStateChanged &&
                                    within_distance(SENSE_RADIUS, x + 0.5f - player.fPlayerX, y + 0.5f - player.fPlayerY)) {
                                    // You must cast to RC_FaceAnimated * to get the function working properly...
                                    ((RC_FaceAnimated*)facePtr)->SetState(nTestAnimState);
                                }
                            }
                        }
                    } // else - map cell is empty, skip it
                }
            }
        }

        //OM.Update(this,player,cMap, cDDrawer,fElapsedTime);

        // step 3 - render
        // ===============
        ray.raycaster(this, cDDrawer, player, &cMap);

        

        // OBJECT RENDERING
        // ================

        // display all objects after the background rendering and before displaying the minimap or debugging output
        // split the rendering into two phase so that it can be sorted on distance (painters algo) before rendering
       
        OM.Update(this, player, cMap, cDDrawer, fElapsedTime);
        // TEST STUFF RENDERING
        // ====================
        DrawString(100, 30, "playerAngle: " + std::to_string(player.fPlayerA_deg), olc::BLUE);
        DrawString(100, 40, "rotatespeed: " + std::to_string(player.rotate_speed), olc::BLUE);
        // to aim the slice that is output on testmode
        if (bTestSlice) {
            DrawLine(int(fTestSlice), 0, int(fTestSlice), ScreenHeight() - 1, olc::MAGENTA);
        }

        // horizontal grid lines for testing
        if (bTestGrid) {
            for (int i = 0; i < ScreenHeight(); i += 100) {
                for (int j = 0; j < 100; j += 10) {
                    DrawLine(0, i + j, ScreenWidth() - 1, i + j, olc::BLACK);
                }
                DrawLine(0, i, ScreenWidth() - 1, i, olc::DARK_GREY);
                DrawString(0, i - 5, std::to_string(i), olc::WHITE);
            }
        }

        // MINIMAP & HUD RENDERING
        // =======================

        if (bMinimap) {
            RenderMap(0);
            if (bMapRays) {
                RenderMapRays(int(player.fPlayerH));
            }
            RenderMapPlayer();
            RenderMapObjects();

            vRayList.clear();
        }

        if (bPlayerInfo) {
            RenderPlayerInfo();
        }

       //if (bProcessInfo) {
       //    RenderProcessInfo();
       //}

        return true;
    }

    bool OnUserDestroy() {

        cMap.FinalizeMap();

        return true;
    }
};

int main()
{
    MyRayCaster demo;
    if (demo.Construct(SCREEN_X / PIXEL_SIZE, SCREEN_Y / PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE))
        demo.Start();

    return 0;
}


//////////////////////////////////   put bloat behind main()  /////////////////////////////////


// ==============================/   Mini map rendering stuff   /==============================

// function to render the mini map on the screen
void MyRayCaster::RenderMap(int nRenderLevel) {

    auto local_map_cell_height = [=](int nLayer, int x, int y) {
        if (nLayer < 0) return cMap.CellHeight(x, y);
        if (nLayer > cMap.NrOfLayers()) return 0.0f;
        return cMap.CellHeightAt(x, y, nRenderLevel);
        };
    // fill background for minimap
    float fMMFactor = MINIMAP_SCALE_FACTOR * MINIMAP_TILE_SIZE;
    FillRect(0, 0, cMap.Width() * fMMFactor, cMap.Hight() * fMMFactor, COL_HUD_BG);
    // draw each tile
    for (int y = 0; y < cMap.Hight(); y++) {
        for (int x = 0; x < cMap.Width(); x++) {
            // colour different for different heights
            olc::Pixel p;
            bool bBorderFlag = true;
            if (local_map_cell_height(nRenderLevel, x, y) == 0.0f) {
                p = COL_HUD_BG;   // don't visibly render
                bBorderFlag = false;
            }
            else if (local_map_cell_height(nRenderLevel, x, y) < 1.0f) {
                p = olc::PixelF(cMap.CellHeight(x, y), 0.0f, 0.0f);    // height < 1.0f = shades of red
            }
            else {
                float fColFactor = std::min(cMap.CellHeight(x, y) / 4.0f + 0.5f, 1.0f);    // heights > 1.0f = shades of blue
                p = olc::PixelF(0.0f, 0.0f, fColFactor);
            }
            // render this tile
            FillRect(x * fMMFactor + 1, y * fMMFactor + 1, fMMFactor - 1, fMMFactor - 1, p);
            if (bBorderFlag) {
                p = olc::WHITE;
                DrawRect(x * fMMFactor, y * fMMFactor, fMMFactor, fMMFactor, p);
            }
        }
    }
}

// function to render the player in the mini map on the screen
void MyRayCaster::RenderMapPlayer() {
    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    olc::Pixel p = olc::YELLOW;
    float px = player.fPlayerX * fMMFactor;
    float py = player.fPlayerY * fMMFactor;
    float pr = 0.6f * fMMFactor;

    // draw sense radius around player - let it blend to get it semi-transparent
    SetPixelBlend(SENSE_BLENDF);
    SetPixelMode(olc::Pixel::ALPHA);
    FillCircle(px, py, SENSE_RADIUS * fMMFactor, olc::DARK_GREY);
    SetPixelMode(olc::Pixel::NORMAL);

    // Draw player object itself
    FillCircle(px, py, pr, p);
    // Draw player direction pointer
    float dx = lu_cos(player.fPlayerA_deg);
    float dy = lu_sin(player.fPlayerA_deg);
    float pdx = dx * 2.0f * fMMFactor;
    float pdy = dy * 2.0f * fMMFactor;
    DrawLine(px, py, px + pdx, py + pdy, p);
}

// function to render the rays in the mini map on the screen
void MyRayCaster::RenderMapRays(int nPlayerLevel) {
    // choose different colour for each layer
    auto get_layer_col = [=](int nLvl) {
        olc::Pixel result = olc::WHITE;
        switch (nLvl) {
        case 0: result = olc::GREEN;   break;
        case 1: result = olc::RED;     break;
        case 2: result = olc::BLUE;    break;
        case 3: result = olc::GREY;    break;
        case 4: result = olc::MAGENTA; break;
        default: result = olc::YELLOW;  break;
        }
        return result;
        };

    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    // draw an outline of the visible part of the world
    // use a different colour per layer
    olc::Pixel layerCol = get_layer_col(nPlayerLevel);
    for (auto& elt : vRayList) {
        if (elt.layer == nPlayerLevel) {
            DrawLine(
                elt.pointA.x * fMMFactor,
                elt.pointA.y * fMMFactor,
                elt.pointB.x * fMMFactor,
                elt.pointB.y * fMMFactor,
                layerCol
            );
        }
    }
}

// function to render all the objects in the mini map on the screen
void MyRayCaster::RenderMapObjects() {
    float fMMFactor = MINIMAP_TILE_SIZE * MINIMAP_SCALE_FACTOR;
    for (auto& elt : vListObjects) {

        olc::Pixel p = (elt.getStationary() ? olc::RED : olc::MAGENTA);

        float px = elt.getPos().x * fMMFactor;
        float py = elt.getPos().y * fMMFactor;
        float pr = 0.4f * fMMFactor;
        FillCircle(px, py, pr, p);

        if (!elt.getStationary()) {
            float dx = lu_cos(rad2deg(elt.GetAngle()));
            float dy = lu_sin(rad2deg(elt.GetAngle()));
            float pdx = dx * 0.3f * elt.GetSpeed() * fMMFactor;
            float pdy = dy * 0.3f * elt.GetSpeed() * fMMFactor;
            DrawLine(px, py, px + pdx, py + pdy, p);
        }
    }
}

// function to render player info in a separate hud on the screen
void MyRayCaster::RenderPlayerInfo() {
    int nStartX = ScreenWidth() - 175;
    int nStartY = 10;
    // render background pane for debug info
    FillRect(nStartX, nStartY, 155, 65, COL_HUD_BG);
    // output player and rendering values for debugging
    DrawString(nStartX + 5, nStartY + 5, "X      = " + std::to_string(player.fPlayerX), COL_HUD_TXT);
    DrawString(nStartX + 5, nStartY + 15, "Y      = " + std::to_string(player.fPlayerY), COL_HUD_TXT);
    DrawString(nStartX + 5, nStartY + 25, "H      = " + std::to_string(player.fPlayerH), COL_HUD_TXT);
    DrawString(nStartX + 5, nStartY + 35, "Angle  = " + std::to_string(player.fPlayerA_deg), COL_HUD_TXT);
    DrawString(nStartX + 5, nStartY + 55, "LookUp = " + std::to_string(player.fLookUp), COL_HUD_TXT);
}

// function to render performance info in a separate hud on the screen
void MyRayCaster::RenderProcessInfo() {
  // int nStartX = ScreenWidth() - 200;
  // int nStartY = ScreenHeight() - 100;
  // // render background pane for debug info
  // FillRect(nStartX, nStartY, 195, 85, COL_HUD_BG);
  // // output player and rendering values for debugging
  // DrawString(nStartX + 5, nStartY + 5, "Intensity  = " + std::to_string(fObjectIntensity), COL_HUD_TXT);
  // DrawString(nStartX + 5, nStartY + 15, "Multiplier = " + std::to_string(fIntensityMultiplier), COL_HUD_TXT);
  // DrawString(nStartX + 5, nStartY + 25, "# Objects  = " + std::to_string((int)vListObjects.size()), COL_HUD_TXT);
  //
  // DrawString(nStartX + 5, nStartY + 45, "Map dim. X = " + std::to_string(cMap.Width()), COL_HUD_TXT);
  // DrawString(nStartX + 5, nStartY + 55, "Map dim. Y = " + std::to_string(cMap.Hight()), COL_HUD_TXT);
  // DrawString(nStartX + 5, nStartY + 65, "Map dim. Z = " + std::to_string(cMap.NrOfLayers()), COL_HUD_TXT);

}

