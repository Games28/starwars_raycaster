#include "Player.h"

Player::Player()
{
	fPlayerX = 4.5f;
	fPlayerY = 4.5f;
    fOffsetX = 20.0f;
    fOffsetY = 20.0f;
	fPlayerH = 0.5f;
	fPlayerA_deg = 90.0f;
	fLookUp = 0.0f;
	fPlayerFoV_deg = 60.0f;
	fPlayerFoV_rad = 0.0f;
    fRadius = 0.4f;
    backtowall = false;
    rotate_speed = 60.0f;
    canrotate = true;
}

void Player::processInput(olc::PixelGameEngine* pge,RC_Map& cMap, float fSpeedUp,float fElapsedTime)
{
    
   

    // set test mode and test slice values
   
    
    // reset look up value and player height on pressing 'R'
    if (pge->GetKey(olc::R).bReleased) { fPlayerH = 0.5f; fLookUp = 0.0f; }

    float oldangle = fPlayerA_deg;
   
    
        // Rotate - collision detection not necessary. Keep fPlayerA_deg between 0 and 360 degrees
        if (pge->GetKey(olc::D).bHeld)
        {
           
                fPlayerA_deg += rotate_speed * fSpeedUp * fElapsedTime;
                if (fPlayerA_deg >= 360.0f) fPlayerA_deg -= 360.0f;
            
        }
        if (pge->GetKey(olc::A).bHeld) 
        {
            
                fPlayerA_deg -= rotate_speed * fSpeedUp * fElapsedTime;
                if (fPlayerA_deg < 0.0f) fPlayerA_deg += 360.0f;
            
        }
        // Rotate to discrete angle
        if (pge->GetKey(olc::NP6).bPressed) { fPlayerA_deg = 0.0f; }
        if (pge->GetKey(olc::NP3).bPressed) { fPlayerA_deg = 45.0f; }
        if (pge->GetKey(olc::NP2).bPressed) { fPlayerA_deg = 90.0f; }
        if (pge->GetKey(olc::NP1).bPressed) { fPlayerA_deg = 135.0f; }
        if (pge->GetKey(olc::NP4).bPressed) { fPlayerA_deg = 180.0f; }
        if (pge->GetKey(olc::NP7).bPressed) { fPlayerA_deg = 225.0f; }
        if (pge->GetKey(olc::NP8).bPressed) { fPlayerA_deg = 270.0f; }
        if (pge->GetKey(olc::NP9).bPressed) { fPlayerA_deg = 315.0f; }
    

    // variables used for collision detection - work out the new location in a separate coordinate pair, and only alter
    // the players coordinate if there's no collision
    float fNewX = fPlayerX;
    float fNewY = fPlayerY;
    float fOldX = fPlayerX;
    float fOldY = fPlayerY;

    
   

    // walking forward, backward and strafing left, right
    if (pge->GetKey(olc::W).bHeld) { fNewX += lu_cos(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; fNewY += lu_sin(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; }   // walk forward
    if (pge->GetKey(olc::S).bHeld) { fNewX -= lu_cos(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; fNewY -= lu_sin(fPlayerA_deg) * SPEED_MOVE * fSpeedUp * fElapsedTime; }   // walk backwards

    if (pge->GetKey(olc::Q).bHeld) { fNewX += lu_sin(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; fNewY -= lu_cos(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; }   // strafe left
    if (pge->GetKey(olc::E).bHeld) { fNewX -= lu_sin(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; fNewY += lu_cos(fPlayerA_deg) * SPEED_STRAFE * fSpeedUp * fElapsedTime; }   // strafe right
    // collision detection - only update position if no collision
    
    // looking up or down - collision detection not necessary
    // NOTE - there's no clamping to extreme values (yet)
    if (pge->GetKey(olc::UP).bHeld) { fLookUp += SPEED_LOOKUP * fSpeedUp * fElapsedTime; }
    if (pge->GetKey(olc::DOWN).bHeld) { fLookUp -= SPEED_LOOKUP * fSpeedUp * fElapsedTime; }

    // flying or crouching
     
    
    // NOTE - for multi layer rendering there's only clamping to keep fPlayerH > 0.0f, there's no upper limit.
    if (!cMap.Collides(fNewX , fNewY , fPlayerH, fRadius, 0.0f, 0.0f)) {
        backtowall = false;
        fPlayerX = fNewX;
        fPlayerY = fNewY;
    }
    else
    {
        backtowall = true;
       
    }

   
    
    // cache current height of horizon, so that you can compensate for changes in it via the look up value
    float fCacheHorHeight = float(pge->ScreenHeight() * fPlayerH) + fLookUp;
    if (MULTI_LAYERS) {
        // if the player height is adapted, keep horizon height stable by compensating with look up value
        if (pge->GetKey(olc::PGUP).bHeld) {
            float fNewHeight = fPlayerH + SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
            // do CD on the height map - player velocity is not relevant since movement is up/down
            if (!cMap.Collides(fPlayerX, fPlayerY, fNewHeight, 0.1f, 0.0f, 0.0f)) {
               fPlayerH = fNewHeight;
               fLookUp = fCacheHorHeight - float(pge->ScreenHeight() * fPlayerH);
            }
        }
        if (pge->GetKey(olc::PGDN).bHeld) {
            float fNewHeight = fPlayerH - SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
            // prevent negative height, and do CD on the height map - player velocity is not relevant since movement is up/down
            if (!cMap.Collides(fPlayerX, fPlayerY, fNewHeight, 0.1f, 0.0f, 0.0f)) {
                fPlayerH = fNewHeight;
                fLookUp = fCacheHorHeight - float(pge->ScreenHeight() * fPlayerH);
            }
        }
    }
    else {
        if (pge->GetKey(olc::PGUP).bHeld) {
            float fNewHeight = fPlayerH + SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
            if (fNewHeight < 1.0f) {
                fPlayerH = fNewHeight;
                // compensate look up value so that horizon remains stable
                fLookUp = fCacheHorHeight - float(pge->ScreenHeight() * fPlayerH);
            }
        }
        if (pge->GetKey(olc::PGDN).bHeld) {
            float fNewHeight = fPlayerH - SPEED_STRAFE_UP * fSpeedUp * fElapsedTime;
            if (fNewHeight > 0.0f) {
                fPlayerH = fNewHeight;
                // compensate look up value so that horizon remains stable
                fLookUp = fCacheHorHeight - float(pge->ScreenHeight() * fPlayerH);
            }
        }
    }

    setMoveDiff({ fOldX ,fOldY }, { fNewX,fNewY });
    setRotateDiff(oldangle, fPlayerA_deg);
}

float Player::getRotateDiff()
{
    return rotatedifference;
}

void Player::setRotateDiff(float oldR, float newR)
{
    rotatedifference = newR - oldR;

}

olc::vf2d Player::getMoveDiff()
{
    return movedifference;
}

void Player::setMoveDiff(olc::vf2d oldpos, olc::vf2d newpos)
{
    movedifference = newpos - oldpos;
}
