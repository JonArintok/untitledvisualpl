
#define fr(i, bound) for (int i = 0; i < (bound); i++)
bool allEq(const float *l, const float *r, int c);

extern float halfVideoSize_gu2[2];

void mapTexRectToVerts(
  float      *destVertData,
  const float destRect_gu[4], // grid units
  const float srcRect_nt[4]   // normalized texture coordinates
);
void printVerts(const float *vertData, int vertCount);
void setRectElems(uint32_t *elems, const uint32_t elemsSize);

void setUiVertAttribs(void);

typedef void (*cursEventHandler)(void *data);
extern void doNothing(void *data);
extern cursEventHandler onDrag;
extern cursEventHandler onClickUp;

typedef struct {
  cursEventHandler onClickDn;
  cursEventHandler onDrag;
  cursEventHandler onClickUp;
} uiElement;

extern GLuint uiShader;
extern GLint  unif_scroll;
extern GLint  unif_unitScale;
extern GLuint uiTex;