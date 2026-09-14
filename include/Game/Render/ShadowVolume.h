#ifndef GAME_RENDER_SHADOW_VOLUME_H
#define GAME_RENDER_SHADOW_VOLUME_H

class GLView;
struct glModel;

void CreateShadowVolumeTarget();
void SetShadowVolumeTarget(GLView* view, GLView* farView);
void AttachShadowVolumeModels(glModel* firstModel, glModel* secondModel, GLView* view, GLView* farView);
void RenderShadowVolumeBlend(GLView* view);

#endif // GAME_RENDER_SHADOW_VOLUME_H
