
#include <GL/freeglut.h>
#include <glm/glm.hpp>
using glm::vec2;

float bigger(const float a, const float b) {return a > b ? a : b;}
float ternaryReduc(const float in) {return in ? (in > 0 ? 1 : -1) : 0;}
bool passedZero(const float prev, const float cur) {
  return (prev > 0 && cur <= 0) || (prev < 0 && cur >= 0);
}

class scrollable {
  float accel;
  float bumper;
  vec2  size;
  vec2  boundary;
  vec2  vel;
  vec2  pos;
  vec2  pPos;
  vec2  overBounds;
  vec2  pOverBounds;
  vec2  winSize;
  vec2  posBR();
  vec2  pPosBR();
  int   lastAdvanced;//milliseconds
public:
  scrollable();
  scrollable(const float accelIn, const vec2 sizeIn, const vec2 winSizeIn);
  void advance(float cursPress, float pCursPress, vec2 cursPos,vec2 pCursPos);
  vec2 getPos();
  bool hasMoved();
};

vec2 scrollable::getPos()   {return pos;}
vec2 scrollable::posBR()    {return pos  + boundary;}
vec2 scrollable::pPosBR()   {return pPos + boundary;}
bool scrollable::hasMoved() {return pos != pPos;}

scrollable::scrollable() {}

scrollable::scrollable(
  const float accelIn, const vec2 sizeIn, const vec2 winSizeIn
) : accel(accelIn), size(sizeIn), winSize(winSizeIn)
{
  for (int i = 0; i < 2; i++) {boundary[i] = bigger(size[i], winSize[i]);}
  bumper = winSize.y/8;
}

void scrollable::advance(
  float cursPress,
  float pCursPress,
  vec2  cursPos,
  vec2  pCursPos
) {
  const int elapsedTime = glutGet(GLUT_ELAPSED_TIME);//milliseconds
  //const int deltaT = elapsedTime - lastAdvanced;
  for (int i = 0; i < 2; i++) {
    pOverBounds[i] = overBounds[i];
    overBounds[i] = pos[i] > 0 ?
      pos[i] :
      (posBR()[i] < winSize[i] ? posBR()[i] - winSize[i] : 0)
    ;
    pPos[i] = pos[i];
    pos[i] = passedZero(pOverBounds[i], overBounds[i]) ?
      (pOverBounds[i] > 0 ? 0 : winSize[i] - boundary[i]) :
      (pos[i] > winSize[i]-bumper ?
        winSize[i] - bumper :
        (posBR()[i] < bumper ?
          bumper - boundary[i] :
          pos[i] + vel[i]
        )
      )
    ;
    vel[i] =
      pos[i] > winSize[i]-bumper ||
      posBR()[i] < bumper ||
      passedZero(pOverBounds[i], overBounds[i]) ?
      0 :
      (cursPress ?
        (pCursPress ? cursPos[i] - pCursPos[i] : 0) :
        vel[i] + accel*ternaryReduc(overBounds[i])*-1
      )
    ;
  }
  lastAdvanced = elapsedTime;
}
