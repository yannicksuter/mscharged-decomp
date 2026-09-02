#ifndef TU_80177498_H
#define TU_80177498_H

#include "Game/Physics/PhysicsBox.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

class cFielder;
class EmissionController;

class WaluigiWallGeometry_80177498
{
public:
    virtual ~WaluigiWallGeometry_80177498();
};

class PhysicsBox_80177498 : public PhysicsBox
{
public:
    PhysicsBox_80177498(cFielder* pParam, float fParam1, float fParam2);
    virtual ~PhysicsBox_80177498();

    virtual int GetObjectType() const { return 0x1D; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PreCollide();
    virtual ContactType Contact(PhysicsObject*, dContact*, int);

    void fn_80177E44(const nlVector3& v3Param);
    void fn_80178170(float fParam);
    ContactType fn_8017819C(cFielder* pParam);

    static void* operator new(unsigned long)
    {
        PhysicsBox_80177498* pObject = 0;
        pool.Allocate(pObject);
        return pObject;
    }

    static void operator delete(void* pObject)
    {
        pool.Free((PhysicsBox_80177498*)pObject);
    }

    static SlotPool<PhysicsBox_80177498> pool;

    /* 0x38 */ nlVector3 mUnidentified038;
    /* 0x44 */ nlVector3 mUnidentified044;
    /* 0x50 */ cFielder* mUnidentified050;
    /* 0x54 */ EmissionController* mUnidentified054;
    /* 0x58 */ float mUnidentified058;
    /* 0x5C */ float mUnidentified05C;
    /* 0x60 */ unsigned int mUnidentified060;
    /* 0x64 */ bool mUnidentified064;
    /* 0x65 */ unsigned char mPadding065[3];
    /* 0x68 */ WaluigiWallGeometry_80177498* mUnidentified068;
}; // size: 0x6C

class WaluigiWallManager_80178400
{
public:
    WaluigiWallManager_80178400();
    ~WaluigiWallManager_80178400();

    PhysicsBox_80177498* fn_80178968(
        cFielder* pParam, float fParam1, float fParam2);
    void fn_80178D0C();
    void fn_80178DBC(float fDeltaT);
    PhysicsBox_80177498* fn_801792C4(int nIndex);
    PhysicsBox_80177498* fn_801792D0(unsigned int uParam);

    /* 0x00 */ PhysicsBox_80177498* mUnidentified000[20];
    /* 0x50 */ PhysicsBox_80177498* mUnidentified050;
    /* 0x54 */ PhysicsBox_80177498* mUnidentified054;
    /* 0x58 */ cFielder* mUnidentified058;
    /* 0x5C */ bool mUnidentified05C;
    /* 0x5D */ unsigned char mPadding05D[3];
    /* 0x60 */ float mUnidentified060;
    /* 0x64 */ void* mUnidentified064;
    /* 0x68 */ void* mUnidentified068;
    /* 0x6C */ void* mUnidentified06C;
    /* 0x70 */ void* mUnidentified070;
}; // size: 0x74

#endif // TU_80177498_H
