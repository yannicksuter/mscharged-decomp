#ifndef NL_GL_CHUNK_LOADER_H
#define NL_GL_CHUNK_LOADER_H

class glModel;
class nlChunk;
class GLResourcePool;

class GLResourceChunkLoader
{
public:
    GLResourceChunkLoader(
        GLResourcePool* pResource, int textureBundleMode);
    bool LoadChunk(nlChunk* pChunk);

    /* 0x00 */ glModel* m_pModels;
    /* 0x04 */ GLResourcePool* m_pResource;
    /* 0x08 */ int mTextureBundleMode;
};

#endif // NL_GL_CHUNK_LOADER_H
