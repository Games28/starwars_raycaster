#include "RC_RayCast.h"

int Raycast::clamp(int value, int min, int max)
{
    if (value < min) return min;
    else if (value > max) return max;
    else return value;
}

bool Raycast::CastRayPerLevelAndAngle(RC_Map & map, Player& player, float fPx, float fPy, int layer, float fRayAngle_deg, std::vector<IntersectInfo>&vHitList)
{
    // counter for nr of hit points found
    int nHitPointsFound = 0;

    // The player's position is the "from point"
    float fFromX = fPx;
    float fFromY = fPy;
    // Calculate the "to point" using the player's angle and fMaxDistance
    float fToX = fPx + map.fMaxDistance * lu_cos(fRayAngle_deg);
    float fToY = fPy + map.fMaxDistance * lu_sin(fRayAngle_deg);
    // work out normalized direction vector (fDX, fDY)
    float fDX = fToX - fFromX;
    float fDY = fToY - fFromY;
    float fRayLen = sqrt(fDX * fDX + fDY * fDY);
    fDX /= fRayLen;
    fDY /= fRayLen;
    // calculate the scaling factors for the ray increments per unit in x resp y direction
    // this calculation takes division by 0.0f into account
    float fSX = (fDX == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDY / fDX) * (fDY / fDX));
    float fSY = (fDY == 0.0f) ? FLT_MAX : sqrt(1.0f + (fDX / fDY) * (fDX / fDY));
    // work out if line is going right or left resp. down or up
    int nGridStepX = (fDX > 0.0f) ? +1 : -1;
    int nGridStepY = (fDY > 0.0f) ? +1 : -1;

    // init loop variables
    float fLengthPartialRayX = 0.0f;
    float fLengthPartialRayY = 0.0f;

    int nCurX = int(fFromX);
    int nCurY = int(fFromY);

    // work out the first intersections with the grid
    if (nGridStepX < 0) { // ray is going left - get scaled difference between start point and left cell border
        fLengthPartialRayX = (fFromX - float(nCurX)) * fSX;
    }
    else {              // ray is going right - get scaled difference between right cell border and start point
        fLengthPartialRayX = (float(nCurX + 1.0f) - fFromX) * fSX;
    }
    if (nGridStepY < 0) { // ray is going up - get scaled difference between start point and top cell border
        fLengthPartialRayY = (fFromY - float(nCurY)) * fSY;
    }
    else {              // ray is going down - get scaled difference between bottom cell border and start point
        fLengthPartialRayY = (float(nCurY + 1.0f) - fFromY) * fSY;
    }

    // check whether analysis got out of map boundaries
    bool bOutOfBounds = !map.IsInBounds(nCurX, nCurY);
    // did analysis reach the destination cell?
    bool bDestCellReached = (nCurX == int(fToX) && nCurY == int(fToY));
    // to keep track of what direction you are searching
    bool bCheckHor;

    // lambda to return index value of face that was hit
    auto get_face_hit = [=](bool bHorGridLine) {
        int nFaceValue = FACE_UNKNOWN;
        if (bHorGridLine) {
            nFaceValue = (nGridStepY < 0 ? FACE_SOUTH : FACE_NORTH);
        }
        else {
            nFaceValue = (nGridStepX < 0 ? FACE_EAST : FACE_WEST);
        }
        return nFaceValue;
        };

    // convenience lambda to add hit point with one call
    auto add_hit_point = [&](std::vector<IntersectInfo>& vHList, float fDst, float fStrtX, float fStrtY, float fDeltaX, float fDeltaY, int nTileX, int nTileY, float fHght, int nLayer, bool bHorGrid) {
        IntersectInfo sInfo;
        sInfo.fDistFrnt = fDst;
        sInfo.fHitX = fStrtX + fDst * fDeltaX;
        sInfo.fHitY = fStrtY + fDst * fDeltaY;
        sInfo.nHitX = nTileX;
        sInfo.nHitY = nTileY;
        sInfo.fHeight = fHght;
        sInfo.nLayer = nLayer;
        sInfo.nFaceHit = get_face_hit(bHorGrid);
        sInfo.bHorizHit = bHorGrid;

        vHList.push_back(sInfo);
        };

    float fDistIfFound = 0.0f;  // accumulates distance of analysed piece of ray
    float fCurHeight = 0.0f;  // to check on differences in height

    bool bPrevWasTransparent = false;
    bool blockatplayer = (map.CellHeightAt(nCurX, nCurY, layer) != fCurHeight);
  
   if (blockatplayer)
   {
       
       if (fLengthPartialRayX < fLengthPartialRayY) {
           // continue analysis in x direction

          

           fDistIfFound = 0.5f;
         
           bCheckHor = false;

       }
       else {
           // continue analysis in y direction
          

           fDistIfFound = 0.5f;
         
           bCheckHor = true;

       }
      fCurHeight = map.CellHeightAt(nCurX, nCurY, layer);
      
      // NEW CODE HERE:
      // check if this is a transparent map cell
      RC_MapCell* auxMapCellPtr = map.MapCellPtrAt(nCurX, nCurY, layer);
      bool bTrnspMapCell;
      if (auxMapCellPtr->IsEmpty()) {
          bTrnspMapCell = false;
      }
      else {
          RC_Face* auxFacePtr = auxMapCellPtr->GetFacePtr(get_face_hit(bCheckHor));
          bTrnspMapCell = auxFacePtr->IsTransparent();
      }
     
      // check if destination cell is found already (for loop control)
      bDestCellReached = (nCurX == int(fToX) && nCurY == int(fToY));
     
      if (player.fPlayerH != fCurHeight || bPrevWasTransparent) {
          bPrevWasTransparent = bTrnspMapCell;
          nHitPointsFound += 1;
          // set current height to new value
          
          // put the collision info in a new IntersectInfo node and push it up the hit list
          add_hit_point(vHitList, fDistIfFound, fFromX, fFromY, fDX, fDY, nCurX, nCurY, fCurHeight, layer, bCheckHor);
      }
      else if (bTrnspMapCell) {
          bPrevWasTransparent = true;
          nHitPointsFound += 1;
          // put the collision info in a new IntersectInfo node and push it up the hit list
          add_hit_point(vHitList, fDistIfFound, fFromX, fFromY, fDX, fDY, nCurX, nCurY, fCurHeight, layer, bCheckHor);
      }
      
   }

    

    // terminate the loop / algorithm if out of bounds or destination found or maxdistance exceeded
    while (!bOutOfBounds && !bDestCellReached && fDistIfFound < map.fMaxDistance) {


      
        ///////////////////////////////////////////////////////////////////////////////////////////
        // advance to next map cell, depending on length of partial ray's
        if (fLengthPartialRayX < fLengthPartialRayY) {
            // continue analysis in x direction
           
                nCurX += nGridStepX;
           
            fDistIfFound = fLengthPartialRayX;
            fLengthPartialRayX += fSX;
            bCheckHor = false;
          
        }
        else {
            // continue analysis in y direction
             nCurY += nGridStepY;
      
            fDistIfFound = fLengthPartialRayY;
            fLengthPartialRayY += fSY;
            bCheckHor = true;
           
        }

        bOutOfBounds = !map.IsInBounds(nCurX , nCurY);
        if (bOutOfBounds) {
            bDestCellReached = false;

            // If out of bounds, finalize the list with one additional intersection with the map boundary and height 0.
            // (only if the list is not empty!) This additional intersection record is necessary for proper rendering at map boundaries.
            if (fCurHeight != 0.0f && nHitPointsFound > 0) {

                fCurHeight = 0.0f;  // since we're out of bounds
                // put the collision info in a new IntersectInfo node and push it up the hit list
                add_hit_point(vHitList, fDistIfFound, fFromX, fFromY, fDX, fDY, nCurX, nCurY, fCurHeight, layer, bCheckHor);
            }
        }
        else {
            // check if there's a difference in height found
            bool bHitFound = (map.CellHeightAt(nCurX, nCurY, layer) != fCurHeight);
            // NEW CODE HERE:
            // check if this is a transparent map cell
            RC_MapCell* auxMapCellPtr = map.MapCellPtrAt(nCurX, nCurY, layer);
            bool bTrnspMapCell;
            if (auxMapCellPtr->IsEmpty()) {
                bTrnspMapCell = false;
            }
            else {
                RC_Face* auxFacePtr = auxMapCellPtr->GetFacePtr(get_face_hit(bCheckHor));
                bTrnspMapCell = auxFacePtr->IsTransparent();
            }

            // check if destination cell is found already (for loop control)
            bDestCellReached = (nCurX == int(fToX) && nCurY == int(fToY));

            if (bHitFound || bPrevWasTransparent) {
                bPrevWasTransparent = bTrnspMapCell;
                nHitPointsFound += 1;
                // set current height to new value
                fCurHeight = map.CellHeightAt(nCurX, nCurY, layer);
                // put the collision info in a new IntersectInfo node and push it up the hit list
                add_hit_point(vHitList, fDistIfFound, fFromX, fFromY, fDX, fDY, nCurX, nCurY, fCurHeight, layer, bCheckHor);
            }
            else if (bTrnspMapCell) {
                bPrevWasTransparent = true;
                nHitPointsFound += 1;
                // put the collision info in a new IntersectInfo node and push it up the hit list
                add_hit_point(vHitList, fDistIfFound, fFromX, fFromY, fDX, fDY, nCurX, nCurY, fCurHeight, layer, bCheckHor);
            }
            
        }
    }
    // return whether any hitpoints were found on this layer
    return (nHitPointsFound > 0);
}

void  Raycast::CalculateBlockProjections(Player& player,RC_Map* map, float fCorrectedDistToWall, int nHorHeight, int nLayerHeight, float fWallHeight, int& nOspTop, int& nOspBottom)
{
    int nSliceHeight = int((1.0f / fCorrectedDistToWall) * map->fDistToProjPlane);
    nOspTop = nHorHeight - (nSliceHeight * (1.0f - player.fPlayerH)) - (nLayerHeight + fWallHeight - 1.0f) * nSliceHeight;
    nOspBottom = nOspTop + nSliceHeight * fWallHeight;
}

void Raycast::raycaster(olc::PixelGameEngine* pge, RC_DepthDrawer& ddraw, Player& player, RC_Map* map)
{
    int nHorizonHeight = pge->ScreenHeight() * player.fPlayerH + (int)player.fLookUp;
    float fAngleStep_deg = player.fPlayerFoV_deg / float(pge->ScreenWidth());

    // having set the horizon height, determine the cos of all the angles through each of the pixels in this slice
    std::vector<float> fHeightAngleCos(pge->ScreenHeight());
    for (int y = 0; y < pge->ScreenHeight(); y++) {
        fHeightAngleCos[y] = std::abs(lu_cos((y - nHorizonHeight) * fAnglePerPixel_deg));
    }

    // clear depth buffer
    ddraw.Reset();

    typedef struct sDelayedPixel {
        int x, y;       // screen coordinates
        float depth;    // for depth drawing
        olc::Pixel p;
    } DelayedPixel;
    std::vector<DelayedPixel> vRenderLater;

   

    // iterate over all screen slices, processing the screen in columns
    for (int x = 0; x < pge->ScreenWidth(); x++) {
        float fViewAngle_deg = float(x - (pge->ScreenWidth() / 2)) * fAngleStep_deg;
        float fCurAngle_deg = player.fPlayerA_deg + fViewAngle_deg;

        float fX_hit, fY_hit;        // to hold exact (float) hit location (world space)
        int   nX_hit, nY_hit;        // to hold coords of tile that was hit (tile space)

        int   nWallTop, nWallTop2;   // to store the top and bottom y coord of the wall per column (screen space)
        int   nWallBot, nWallBot2;   // the ...2 variant represents the back of the current map cell

        // This lambda performs much of the sampling proces of horizontal surfaces. It can be used for floors, roofs and ceilings etc.
        // fProjDistance is the distance from the player to the hit point on the surface.
        auto generic_sampling = [=](float fProjDistance, olc::Sprite* cTexturePtr) -> olc::Pixel {
            // calculate the world coordinates from the distance and the view angle + player angle
            float fProjX = player.fPlayerX + fProjDistance * lu_cos(fCurAngle_deg);
            float fProjY = player.fPlayerY + fProjDistance * lu_sin(fCurAngle_deg);
            // calculate the sample coordinates for that world coordinate, by subtracting the
            // integer part and only keeping the fractional part. Wrap around if the result < 0 or > 1
            float fSampleX = fProjX - int(fProjX); if (fSampleX < 0.0f) fSampleX += 1.0f; if (fSampleX >= 1.0f) fSampleX -= 1.0f;
            float fSampleY = fProjY - int(fProjY); if (fSampleY < 0.0f) fSampleY += 1.0f; if (fSampleY >= 1.0f) fSampleY -= 1.0f;
            // having both sample coordinates, use the texture pointer to get the sample, and shade and return it
            return ShadePixel(cTexturePtr->Sample(fSampleX, fSampleY), fProjDistance);
            };

        // This lambda performs much of the sampling proces of horizontal surfaces. It can be used for floors, roofs and ceilings etc.
        // fProjDistance is the distance from the player to the hit point on the surface.
        auto generic_sampling_new = [=](float fProjDistance, int nLayer, int nFaceID) -> olc::Pixel {
            // calculate the world coordinates from the distance and the view angle + player angle
            float fProjX = player.fPlayerX + fProjDistance * lu_cos(fCurAngle_deg);
            float fProjY = player.fPlayerY + fProjDistance * lu_sin(fCurAngle_deg);
            // calculate the sample coordinates for that world coordinate, by subtracting the
            // integer part and only keeping the fractional part. Wrap around if the result < 0 or > 1
            float fSampleX = fProjX - int(fProjX); if (fSampleX < 0.0f) fSampleX += 1.0f; if (fSampleX >= 1.0f) fSampleX -= 1.0f;
            float fSampleY = fProjY - int(fProjY); if (fSampleY < 0.0f) fSampleY += 1.0f; if (fSampleY >= 1.0f) fSampleY -= 1.0f;

            // select the sprite to render the ceiling depending on the map cell that was hit
            int nTileX = clamp(int(fProjX), 0, map->Width() - 1);
            int nTileY = clamp(int(fProjY), 0, map->Hight() - 1);
            // obtain a pointer to the map cell that was hit
            RC_MapCell* auxMapCellPtr = map->MapCellPtrAt(nTileX, nTileY, nLayer);
            // sample that map cell passing the face that was hit and the sample coordinates
            olc::Pixel sampledPixel = (auxMapCellPtr == nullptr) ? olc::MAGENTA : auxMapCellPtr->Sample(nFaceID, fSampleX, fSampleY);
            // shade and return the pixel
            return ShadePixel(sampledPixel, fProjDistance);
            };

        // this lambda returns a sample of the floor through the pixel at screen coord (px, py)
        auto get_floor_sample = [=](int px, int py) -> olc::Pixel {
            // work out the distance to the location on the floor you are looking at through this pixel
            float fFloorProjDistance = ((player.fPlayerH / float(py - nHorizonHeight)) * map->fDistToProjPlane) / lu_cos(fViewAngle_deg);
            // call the generic sampler to work out the rest
            return generic_sampling(fFloorProjDistance, vFlorSprites[0]);
            };

        // this lambda returns a sample of the roof through the pixel at screen coord (px, py)
        // NOTE: fHeightWithinLayer denotes the height of the hit point on the roof. This is typically the height of the map cell + its layer
        auto get_roof_sample = [=](int px, int py, int nLayer, float fHeightWithinLayer, float& fRoofProjDistance) -> olc::Pixel {
            // work out the distance to the location on the roof you are looking at through this pixel
            fRoofProjDistance = (((player.fPlayerH - (float(nLayer) + fHeightWithinLayer)) / float(py - nHorizonHeight)) * map->fDistToProjPlane) / lu_cos(fViewAngle_deg);
            // call the generic sampler to work out the rest
            return generic_sampling_new(fRoofProjDistance, nLayer, FACE_TOP);
            };

        // this lambda returns a sample of the ceiling through the pixel at screen coord (px, py)
        // NOTE: fHeightWithinLayer denotes the height of the hit point on the ceiling. This is typically the layer of the map cell, WITHOUT its height!
        auto get_ceil_sample = [=](int px, int py, int nLayer, float fHeightWithinLayer, float& fCeilProjDistance) -> olc::Pixel {
            // work out the distance to the location on the ceiling you are looking at through this pixel
            fCeilProjDistance = (((float(nLayer) - player.fPlayerH) / float(nHorizonHeight - py)) * map->fDistToProjPlane) / lu_cos(fViewAngle_deg);
            // call the generic sampler to work out the rest
            return generic_sampling_new(fCeilProjDistance, nLayer, FACE_BOTTOM);
            };

        // prepare the rendering for this slice by calculating the list of intersections in this ray's direction
        // for each layer, get the list of hitpoints in that layer, work out front and back distances and projections
        // on screen, and add to the global vHitPointList
        std::vector<IntersectInfo> vHitPointList;
        for (int k = 0; k < map->NrOfLayers(); k++) {

            std::vector<IntersectInfo> vCurLevelList;
            CastRayPerLevelAndAngle(*map,player,player.fPlayerX, player.fPlayerY, k, fCurAngle_deg, vCurLevelList);

            for (int i = 0; i < (int)vCurLevelList.size(); i++) {
                // make correction for the fish eye effect
                vCurLevelList[i].fDistFrnt *= lu_cos(fViewAngle_deg);
                // calculate values for the on screen projections osp_top_frnt and top_bottom
                CalculateBlockProjections(
                    player,
                    map,
                    vCurLevelList[i].fDistFrnt,
                    nHorizonHeight,
                    vCurLevelList[i].nLayer,
                    vCurLevelList[i].fHeight,
                    vCurLevelList[i].osp_top_frnt,
                    vCurLevelList[i].osp_bot_frnt
                );
            }
            // Extend the hit list with projected ceiling info for the back of the wall
            for (int i = 0; i < (int)vCurLevelList.size(); i++) {
                if (i == (int)vCurLevelList.size() - 1) {
                    // last element, has no successor
                    vCurLevelList[i].fDistBack = vCurLevelList[i].fDistFrnt;
                    vCurLevelList[i].osp_top_back = vCurLevelList[i].osp_top_frnt;
                    vCurLevelList[i].osp_bot_back = vCurLevelList[i].osp_bot_frnt;
                }
                else {
                    // calculate values for the on screen projections osp_top_frnt and top_bottom
                    vCurLevelList[i].fDistBack = vCurLevelList[i + 1].fDistFrnt;
                    CalculateBlockProjections(
                        player,
                        map,
                        vCurLevelList[i].fDistBack,
                        nHorizonHeight,
                        vCurLevelList[i].nLayer,
                        vCurLevelList[i].fHeight,
                        vCurLevelList[i].osp_top_back,
                        vCurLevelList[i].osp_bot_back
                    );
                }
            }

            // populate ray list for rendering mini map
           // if (bMinimap && !vCurLevelList.empty()) {
           //     RayType curHitPoint = { {player.fPlayerX,player.fPlayerY }, { vCurLevelList[0].fHitX, vCurLevelList[0].fHitY }, vCurLevelList[0].nLayer };
           //     vRayList.push_back(curHitPoint);
           // }

            // add the hit points for this layer list to the combined hit point list
            vHitPointList.insert(vHitPointList.end(), vCurLevelList.begin(), vCurLevelList.end());
        }

        // remove all hit points with height 0.0f - they were necessary for calculating the back face projection
        // of map cells, but that part is done now
        vHitPointList.erase(
            std::remove_if(
                vHitPointList.begin(),
                vHitPointList.end(),
                [](IntersectInfo& a) {
                    return a.fHeight == 0.0f;
                }
            ),
            vHitPointList.end()
        );

        // sort hit points from far away to close by (painters algorithm)
        std::sort(
            vHitPointList.begin(),
            vHitPointList.end(),
            [](IntersectInfo& a, IntersectInfo& b) {
                return (a.fDistFrnt > b.fDistFrnt) ||
                    (a.fDistFrnt == b.fDistFrnt && a.nLayer < b.nLayer);
            }
        );

       

        // start rendering this slice by putting sky and floor in it
        float fWellAway = map->fMaxDistance + 100.0f;
        for (int y = pge->ScreenHeight() - 1; y >= 0; y--) {

            // draw floor and horizon
            if (y < nHorizonHeight) {
                olc::Pixel skySample = olc::CYAN;
                ddraw.Draw(fWellAway, x, y, skySample);
            }
            else {
                olc::Pixel floorSample = get_floor_sample(x, y);   // shading is done in get_floor_sample()
                ddraw.Draw(fWellAway, x, y, floorSample);
            }
        }

        // now render all hit points back to front
        for (int i = 0; i < vHitPointList.size(); i++ ) {

            IntersectInfo& hitRec = vHitPointList[i];

            float fMapCellElevation = 1.0f;
            int   nMapCellLevel = 0;
            float fFrntDistance = 0.0f;     // distance var also used for wall shading
            int   nFaceHit = FACE_UNKNOWN;

            // For the distance calculations we needed also points where the height returns to 0.0f (the
            // back faces of the map cell). For the rendering we must skip these "hit points"
            if (hitRec.fHeight > 0.0f) {
                // load the info from next hit point
                fX_hit = hitRec.fHitX;
                fY_hit = hitRec.fHitY;
                nX_hit = hitRec.nHitX;
                nY_hit = hitRec.nHitY;
                fMapCellElevation = hitRec.fHeight;
                nMapCellLevel = hitRec.nLayer;
                fFrntDistance = hitRec.fDistFrnt;
                nFaceHit = hitRec.nFaceHit;
                // make sure the screen y coordinate is within screen boundaries
                nWallTop = clamp(hitRec.osp_top_frnt, 0, pge->ScreenHeight() - 1);
                nWallTop2 = clamp(hitRec.osp_top_back, 0, pge->ScreenHeight() - 1);
                nWallBot = clamp(hitRec.osp_bot_frnt, 0, pge->ScreenHeight() - 1);
                nWallBot2 = clamp(hitRec.osp_bot_back, 0, pge->ScreenHeight() - 1);

              
                
                // render roof segment if it's visible
                for (int y = nWallTop2; y < nWallTop; y++) {
                    // the distance to this point is calculated and passed from get_roof_sample
                    float fRenderDistance;
                    olc::Pixel roofSample = get_roof_sample(x, y, nMapCellLevel, fMapCellElevation, fRenderDistance);   // shading is done in get_roof_sample()
                    ddraw.Draw(fRenderDistance / fHeightAngleCos[y], x, y, roofSample);
                }

                if (i == (vHitPointList.size() - 1) && nMapCellLevel == 0)
                {
                    map->walldismantle.prepboundry(pge->ScreenWidth(), ( pge->ScreenHeight() / 2 ) - nWallTop, (pge->ScreenHeight() / 2) + nWallBot);
                   
                }
                // render wall segment
                float fSampleX = -1.0f;
                for (int y = nWallTop; y <= nWallBot; y++) {

                    // first get x sample coordinate from face hit info
                    if (fSampleX == -1.0f) {
                        switch (nFaceHit) {
                        case FACE_SOUTH:
                        case FACE_NORTH: fSampleX = fX_hit - (float)nX_hit; break;
                        case FACE_EAST:
                        case FACE_WEST: fSampleX = fY_hit - (float)nY_hit; break;
                        default: std::cout << "ERROR: OnUserUpdate() --> invalid face value: " << nFaceHit << std::endl;
                        }
                    }

                    // the y sample coordinate depends only on the pixel y coord on the screen in relation to the vertical space the wall is taking up
                    float fSampleY = fMapCellElevation * float(y - hitRec.osp_top_frnt) / float(hitRec.osp_bot_frnt - hitRec.osp_top_frnt);

                    // get a pointer to the map cell that was hit
                    RC_MapCell* auxMapCellPtr = map->MapCellPtrAt(nX_hit, nY_hit, nMapCellLevel);
                    if (auxMapCellPtr == nullptr) {
                        std::cout << "FATAL ERROR - situation should not occur!!! nullptr map cell ptr detected at (" << nX_hit << ", " << nY_hit << ") layer " << nMapCellLevel << std::endl;
                    }
                    // sample that map cell passing the face that was hit and the sample coordinates
                    olc::Pixel sampledPixel = (auxMapCellPtr == nullptr) ? olc::MAGENTA : auxMapCellPtr->Sample(nFaceHit, fSampleX, fSampleY);
                    // shade the pixel
                    olc::Pixel wallSample = ShadePixel(sampledPixel, fFrntDistance);

                    // render or store for later rendering, depending on the map cell type
                    RC_Face* auxFacePtr = auxMapCellPtr->GetFacePtr(nFaceHit);

                   

                    if (auxFacePtr->IsTransparent()) {
                      
                        if (map->walldismantle.withinboundry(x, y))
                        {
                            //if (wallSample != olc::BLANK)
                            {
                                DelayedPixel aux = { x, y, fFrntDistance / fHeightAngleCos[y], olc::BLANK };
                                vRenderLater.push_back(aux);

                            }
                        }
                        else
                        {
                       
                            DelayedPixel aux = { x, y, fFrntDistance / fHeightAngleCos[y], wallSample };
                            vRenderLater.push_back(aux);
                        }
                    }
                    else 
                    {
                      
                      
                            ddraw.Draw(fFrntDistance / fHeightAngleCos[y], x, y, wallSample);
                          
                    }
                  
                    
                    if (map->walldismantle.withinboundry(x, y))
                    {
                        wallSample.a = 150;
                        ddraw.Draw(fFrntDistance / fHeightAngleCos[y], x, y, wallSample);
                    }

                  

                }
               
               
                // render ceiling segment if it's visible
                for (int y = nWallBot + 1; y <= nWallBot2; y++) {
                    float fRenderDistance;
                    olc::Pixel ceilSample = get_ceil_sample(x, y, nMapCellLevel, fMapCellElevation, fRenderDistance);   // shading is done in get_ceil_sample()
                    ddraw.Draw(fRenderDistance / fHeightAngleCos[y], x, y, ceilSample);
                }
            }
        }
        // if test mode triggered, print the hit list along the test slice
       
    }

    // DELAYED WALL RENDERING (with masking of blank pixels)
    for (auto& elt : vRenderLater) {

        
        if (elt.p != olc::BLANK) 
        {
           // if (map->walldismantle.withinboundry(elt.x, elt.y))
           // {
           //     elt.p.a = 150;
           //     ddraw.Draw(elt.depth, elt.x, elt.y, elt.p);
           // }
           // else
            {
                ddraw.Draw(elt.depth, elt.x, elt.y, elt.p);
            }
            
        }
    }
}


olc::Pixel  Raycast::ShadePixel(const olc::Pixel& p, float fDistance)
{
    if (RENDER_SHADED) {
        float fShadeFactor = std::max(SHADE_FACTOR_MIN, std::min(SHADE_FACTOR_MAX, fObjectIntensity * (fIntensityMultiplier / fDistance)));
        return p * fShadeFactor;
    }
    else
        return p;
}

