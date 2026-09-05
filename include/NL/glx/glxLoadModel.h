#ifndef NL_GLX_GLXLOADMODEL_H
#define NL_GLX_GLXLOADMODEL_H

class cSHierarchy;
class GLSkinMesh;
struct glModel;
class nlChunk;

glModel* glplatEndLoadModel(
    void* data, unsigned long size, unsigned long* pNumModels, void* context);
bool glplatBeginLoadModel(
    const char* filename, void (*callback)(void*, unsigned long, void*),
    void* userData);
glModel* glplatLoadModel(
    const char* filename, unsigned long* pNumModels, void* context);
GLSkinMesh* glx_MakeSkinMesh(
    nlChunk* outerChunk, glModel* models, cSHierarchy* hierarchy);

void* fn_802CB7D0(unsigned long hash);

#endif // NL_GLX_GLXLOADMODEL_H
