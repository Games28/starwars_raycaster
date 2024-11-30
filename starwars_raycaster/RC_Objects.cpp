#include "RC_Objects.h"

RC_Object::RC_Object() {}

RC_Object::RC_Object(float fX, float fY, float fS, float fD, float fA, olc::Sprite* pS) {
    position = { fX,fY };
    scale = fS;
    fDistToPlayer = fD;
    fAngleToPlayer = fA;
    sprite = pS;
    velocity = { 0.0f,0.0f };
    acceleration = { 0.0f,0.0f };
    UpdateObjAngle();
    UpdateObjSpeed();
}

RC_Object::~RC_Object()
{
   // sprite = nullptr;
   // decal = nullptr;
}

olc::vf2d RC_Object::getPos()
{
    return position;
}

void RC_Object::setPos(olc::vf2d pos)
{
    this->position = pos;
}

void RC_Object::setX(float x)
{
    position.x = x;
}

void RC_Object::SetY(float y)
{
    position.y = y;
}

void RC_Object::SetVX(float vx)
{
    velocity.x = vx;
}

void RC_Object::SetVY(float vy)
{
    velocity.y = vy;
}

void RC_Object::setPosOffset(olc::vf2d pos)
{
    this->position += pos;
}

void RC_Object::setRadius(float r)
{
    fradius = r;
}

float RC_Object::getRadius()
{
    return fradius;
}

olc::vf2d RC_Object::getVel()
{
    return velocity;
}

void RC_Object::Setvel(olc::vf2d vel)
{
    velocity = vel;
}

void RC_Object::IsStationary(bool stationary)
{
    isStationary = stationary;
}

bool RC_Object::getStationary()
{
    return isStationary;
}

void RC_Object::IsAnimated(bool animate)
{
    isAnimated = animate;
}

void RC_Object::IsBillboarded(bool board)
{
    billboarding = board;
}





void RC_Object::SetScale(float fS) { scale = fS; }
void RC_Object::SetDistToPlayer(float fD) { fDistToPlayer = fD; }
void RC_Object::SetAngleToPlayer(float fA) { fAngleToPlayer = fA; }

float RC_Object::GetScale() { return scale; }
float RC_Object::GetDistToPlayer() { return fDistToPlayer; }
float RC_Object::GetAngleToPlayer() { return fAngleToPlayer; }

void RC_Object::setispickedup(bool picked)
{
   ispickedup = picked;
}

bool RC_Object::getispickedup()
{
    return ispickedup;
}

void RC_Object::SetSprite(olc::Sprite* pS) { sprite = pS; }
void RC_Object::SetDecal()
{
    decal = new olc::Decal(sprite);
}
olc::Sprite* RC_Object::GetSprite() { return sprite; }

float RC_Object::GetAngle() { return fObjAngle_rad; }

float RC_Object::GetSpeed()
{
    return fObjSpeed;
}

void RC_Object::UpdateObjAngle()
{
    fObjAngle_rad = mod2pi(atan2f(velocity.y, velocity.x));
}

void RC_Object::UpdateObjAngle(float vx, float vy)
{
}

void RC_Object::UpdateObjSpeed()
{
    fObjSpeed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
}



void RC_Object::Print() {
  
}

// work out distance and angle between object and player, and
// store it in the object itself
void RC_Object::PrepareRender(float fPx, float fPy, float fPa_deg) {
    // can object be seen?
    float fVecX = getPos().x - fPx;
    float fVecY = getPos().y - fPy;
    SetDistToPlayer(sqrtf(fVecX * fVecX + fVecY * fVecY));
    // calculate angle between vector from player to object, and players looking direction
    // to determine if object is in players field of view
    float fEyeX = lu_cos(fPa_deg);
    float fEyeY = lu_sin(fPa_deg);
    float fObjA_rad = atan2f(fVecY, fVecX) - atan2f(fEyeY, fEyeX);
    // "bodge" angle into range [ -PI, PI ]
    fObjA_rad = mod2pi(fObjA_rad, -PI);
    SetAngleToPlayer(fObjA_rad);
}

void RC_Object::Render(olc::PixelGameEngine *pge,RC_DepthDrawer & ddrwr, float fPh, float fFOV_rad, float fMaxDist, int nHorHeight) {
    // determine whether object is in field of view (a bit larger to prevent objects being not rendered at
    // screen boundaries)
    float fObjDist = GetDistToPlayer();
    float fObjA_rad = GetAngleToPlayer();
    bool bInFOV = fabs(fObjA_rad) < fFOV_rad / 1.2f;

    // render object only when within Field of View, and within visible distance.
    // the check on proximity is to prevent asymptotic errors when distance to player becomes very small
    if (bInFOV && fObjDist >= 0.3f && fObjDist < fMaxDist) {

        // determine the difference between standard player height (i.e. 0.5f = standing on the floor)
        // and current player height
        float fCompensatePlayerHeight = fPh - 0.5f;
        // get the projected (halve) slice height of this object
        float fObjHalveSliceHeight = float(ddrwr.ScreenHeight() / fObjDist);
        float fObjHalveSliceHeightScld = float((ddrwr.ScreenHeight()) / fObjDist) * GetScale();

        // work out where objects floor and ceiling are (in screen space)
        // due to scaling factor, differentiated a normalized (scale = 1.0f) ceiling and a scaled variant
        float fObjCeilingNormalized = float(nHorHeight) - fObjHalveSliceHeight;
        float fObjCeilingScaled = float(nHorHeight) - fObjHalveSliceHeightScld;
        // and adapt all the scaling into the ceiling value
        float fScalingDifference = fObjCeilingNormalized - fObjCeilingScaled;
        float fObjCeiling = fObjCeilingNormalized - 2 * fScalingDifference;
        float fObjFloor = float(nHorHeight) + fObjHalveSliceHeight;

        // compensate object projection heights for elevation of the player
        fObjCeiling += fCompensatePlayerHeight * fObjHalveSliceHeight * 2.0f;
        fObjFloor += fCompensatePlayerHeight * fObjHalveSliceHeight * 2.0f;

        // get height, aspect ratio and width
        float fObjHeight = fObjFloor - fObjCeiling;
        float fObjAR = float(GetSprite()->height) / float(GetSprite()->width);
        float fObjWidth = fObjHeight / fObjAR;
        // work out where the object is across the screen width
        float fMidOfObj = (0.5f * (fObjA_rad / (fFOV_rad / 2.0f)) + 0.5f) * float(ddrwr.ScreenWidth());


        std::array<olc::vf2d, 4> points;

        std::array<olc::vf2d, 4> offsets;



        points[3] = { (fMidOfObj + 0.0f - (fObjWidth / 2.0f))           ,(fObjCeiling + 0)              };  //topleft
        points[0] = { (fMidOfObj + fObjWidth - (fObjWidth / 2.0f))      ,(fObjCeiling + 0)              }; //topright
        points[2] = { (fMidOfObj + 0.0f - (fObjWidth / 2.0f))           ,(fObjCeiling + fObjHeight)     }; //bottom left
        points[1] = { (fMidOfObj + fObjWidth - (fObjWidth / 2.0f))      ,(fObjCeiling + fObjHeight)     }; // bottom right

        
        ddrwr.DrawWarpedSprite(pge, GetSprite(), points,fObjDist);
        //pge->DrawWarpedDecal(decal, points);

       // render the sprite
       //for (float fx = 0.0f; fx < fObjWidth; fx++) {
       //    // get distance across the screen to render
       //    int nObjColumn = int(fMidOfObj + fx - (fObjWidth / 2.0f));
       //    // only render this column if it's on the screen
       //    if (nObjColumn >= 0 && nObjColumn < ddrwr.ScreenWidth()) {
       //        for (float fy = 0.0f; fy < fObjHeight; fy++) {
       //            // calculate sample coordinates as a percentage of object width and height
       //            float fSampleX = fx / fObjWidth;
       //            float fSampleY = fy / fObjHeight;
       //            // sample the pixel and draw it
//     //                  olc::Pixel objSample = ShadePixel( GetSprite()->Sample( fSampleX, fSampleY ), fObjDist );
       //            olc::Pixel objSample = GetSprite()->Sample(fSampleX, fSampleY);
       //            if (objSample != olc::MAGENTA) {
       //                ddrwr.Draw(fObjDist, nObjColumn, fObjCeiling + fy, objSample);
       //            }
       //        }
       //    }
       //}
    }
}


