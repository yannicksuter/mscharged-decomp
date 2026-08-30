#ifndef NL_GL_GLLOADMODEL_H
#define NL_GL_GLLOADMODEL_H

class GLInventory;
struct glModel;
class nlChunk;

struct UnidentifiedLoadContext
{
    unsigned char m_Unknown00[0x0C];
    GLInventory* m_pInventory;
};

class RLGReader
{
public:
    RLGReader() { }

    virtual void fn_802CA778(nlChunk* chunk);
    virtual void fn_802CAA00(void* data, unsigned long size);
    virtual void fn_80369E5C(void* data, unsigned long size) = 0;
    virtual void fn_80369EC8(void* data, unsigned long size) = 0;
    virtual void fn_802CAA60(void* data, unsigned long size);
    virtual void fn_802CAAC0(void* data, unsigned long size);
    virtual void fn_802CAB34(void* data, unsigned long size);
    virtual void fn_802CABBC(void* data, unsigned long size);
    virtual void fn_80369F34(nlChunk* chunk) = 0;
    virtual void fn_802CACBC(nlChunk* chunk);
    virtual void fn_8036A138() = 0;
    virtual void fn_802CAC1C();

    void fn_802CA870(nlChunk* chunk);
    void fn_802CADC4(void* data);

    /* 0x04 */ void* m_Unknown04;
    /* 0x08 */ unsigned long m_Unknown08;
    /* 0x0C */ glModel* m_pModels;
    /* 0x10 */ unsigned long m_nModels;
    /* 0x14 */ void* m_Unknown14;
    /* 0x18 */ unsigned long m_Unknown18;
    /* 0x1C */ void* m_Unknown1C;
    /* 0x20 */ void* m_Unknown20;
    /* 0x24 */ void* m_Unknown24;
    /* 0x28 */ void* m_Unknown28;
    /* 0x2C */ UnidentifiedLoadContext* m_pContext;
};

void glSetIgnoreDuplicateModels(bool ignore);

#endif // NL_GL_GLLOADMODEL_H
