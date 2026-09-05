#ifndef NL_GL_UNIDENTIFIEDCHUNKLOADER_802D0458_H
#define NL_GL_UNIDENTIFIEDCHUNKLOADER_802D0458_H

class glModel;
class nlChunk;
class ResourceInterface_802CC094;

class UnidentifiedChunkLoader_802D0458
{
public:
    UnidentifiedChunkLoader_802D0458(
        ResourceInterface_802CC094* pResource, int nParam);
    bool fn_802D046C(nlChunk* pChunk);

    /* 0x00 */ glModel* m_pModels;
    /* 0x04 */ ResourceInterface_802CC094* m_pResource;
    /* 0x08 */ int m_nUnidentified008;
};

#endif // NL_GL_UNIDENTIFIEDCHUNKLOADER_802D0458_H
