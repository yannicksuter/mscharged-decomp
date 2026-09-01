#include "Game/Task/TransitionTask.h"

#include "Game/BasicStadium.h"
#include "Game/Character.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Event.h"
#include "Game/NisPlayer.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "NL/nlTask.h"
#include "types.h"
#include "Game/Render/NPCManager.h"

// Charged keeps the predecessor's transition-manager surface but replaces most
// of its subsystem calls. Everything still address-named below lives in a
// translation unit that has not been reconstructed yet.

// The original type identity of this common weak static is not yet known.
struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

// The pending-callback list walked below belongs to the game object and is not
// reconstructed yet. Only the offsets and the flag bits the target reads are
// modelled here; these names are descriptive, not recovered.
struct Invokable
{
    virtual void Release(bool);
    virtual void Invoke();
};

struct PendingBinding
{
    ~PendingBinding()
    {
        if (this != 0)
        {
            if (mKind == 2 && mTarget != 0)
            {
                mTarget->Release(true);
            }
            mKind = 0;
        }
    }

    /* 0x00 */ int mKind;
    /* 0x04 */ Invokable* mTarget;
};

struct PendingEntry : public UnidentifiedConnection
{
    ~PendingEntry()
    {
        if (this != 0)
        {
        }
    }

    /* 0x0C */ PendingBinding mBinding;
};

struct PendingNode
{
    /* 0x00 */ PendingNode* next;
    /* 0x04 */ PendingNode* prev;
    /* 0x08 */ PendingEntry entry;
};

struct PendingIter
{
    PendingNode* cur;
    PendingNode* next;
};

struct Presentation
{
    char _000[0xC0];
    /* 0xC0 */ float mLetterBoxDuration;
    /* 0xC4 */ bool mLetterBoxEnabled;
    char _0C5[0x9F];
    /* 0x164 */ bool mUnknown164;
};

struct Game
{
    char _000[0x18];
    /* 0x18 */ int mState;
    char _01C[0x638];
    /* 0x654 */ PendingEntry* mCurrentEntry;
    char _658[0xC];
    /* 0x664 */ PendingNode* mFreeList;
    char _668[0x8];
    /* 0x670 */ PendingNode* mPendingList;
};

static PendingIter MakeIter(PendingNode* head);
static PendingIter MakeIterAt(PendingNode* head, PendingNode* node);

extern "C" {
void fn_80137CB8(int);
void fn_8027C86C();
void fn_8027D11C();
void fn_80278A00(void*, int, int);
Presentation* fn_80284A58();
void fn_80285714(Presentation*, u32, u32);
void* fn_80188C5C();
void fn_80189F0C(void*);
void fn_80188360();
void fn_801882B4();
void fn_800180AC(void*);
void fn_801E23A4(void*, u32, u32);
void fn_800A7998(void*);
void fn_80059940(Game*, int);
void fn_801745DC(void*);
void fn_800F23F4();
void fn_800F2404();
}

extern u8 lbl_806E13C2;
extern u8 lbl_806E181D;
extern void* g_pBall;
extern void* lbl_806E0DF8[];
extern void* lbl_806E1860;
extern void* lbl_806E12C8[2];
extern Game* lbl_806E0C94;
extern cCharacter* lbl_8056B800[10];

void TransitionTask::Initialize()
{
}

static PendingIter MakeIter(PendingNode* head)
{
    PendingIter it;

    it.next = (head == 0) ? 0 : head->next;
    it.cur = head;
    return it;
}

static PendingIter MakeIterAt(PendingNode* head, PendingNode* node)
{
    PendingIter it;

    it.next = node;
    it.cur = head;
    return it;
}

inline void ClearCharacterEffectsTexturing()
{
    PendingNode* next;
    PendingNode* cur;
    bool atEnd;
    Game* pGame;
    pGame = lbl_806E0C94;

    if (pGame != 0)
    {
        PendingIter begin = MakeIter(pGame->mPendingList);
        cur = begin.cur;
        next = begin.next;

        while (next != 0)
        {
            PendingEntry* entry = &next->entry;
            pGame->mCurrentEntry = entry;

            if ((next->entry.mFlags >> 31) != 0)
            {
                if (entry->mBinding.mKind == 1)
                {
                    ((void (*)())entry->mBinding.mTarget)();
                }
                else
                {
                    entry->mBinding.mTarget->Invoke();
                }
                cur = MakeIter(pGame->mPendingList).cur;
            }

            if (cur == 0)
            {
                atEnd = true;
            }
            else
            {
                atEnd = (cur == next);
            }
            if (atEnd || next == 0)
            {
                next = 0;
            }
            else
            {
                next = next->next;
            }

            if (((entry->mFlags >> 29) & 1) != 0)
            {
                PendingIter erase = MakeIterAt(pGame->mPendingList, (PendingNode*)((char*)entry - 8));
                PendingNode* dead = erase.next;

                if (dead->next == dead)
                {
                    pGame->mPendingList = 0;
                }
                else
                {
                    dead->prev->next = dead->next;
                    dead->next->prev = dead->prev;
                    if (pGame->mPendingList == dead)
                    {
                        pGame->mPendingList = dead->prev;
                    }
                }

                if (dead != 0)
                {
                    dead->entry.~PendingEntry();
                }

                dead->next = pGame->mFreeList;
                pGame->mFreeList = dead;
            }
        }

        pGame->mCurrentEntry = 0;
        DrawableCharacter::RenderAllCharacters();
    }

    fn_80059940(lbl_806E0C94, 0);
    lbl_806E1608->fn_801ABF8C();
    fn_801745DC(lbl_806E12C8[0]);
}

void TransitionTask::StateTransition(u32 from, u32 to)
{
    int i;

    nlTaskManager::m_pInstance->mLocked = true;

    bool bNISLighting;
    if (to & 0x10)
    {
        bNISLighting = true;
    }
    else if (to == 1 && (from & 0x10))
    {
        bNISLighting = true;
    }
    else
    {
        bNISLighting = false;
    }
    lbl_806E13C2 = bNISLighting;

    if (to == 4)
    {
        fn_80137CB8(1);
    }

    if (to == 0x10)
    {
        NisPlayer::Instance();
        fn_8027D11C();
    }

    if (BasicStadium::GetCurrentStadium() != 0)
    {
        if (to == 0x10 && fn_80284A58()->mUnknown164)
        {
            fn_80278A00(BasicStadium::GetCurrentStadium(), 0x37, 1);
        }
        else
        {
            fn_80278A00(BasicStadium::GetCurrentStadium(), 0x37, 0);
        }
    }

    fn_80285714(fn_80284A58(), from, to);

    if (to == 2)
    {
        fn_80137CB8(0);

        if (from != 1 && from != 0x20)
        {
            fn_80189F0C(fn_80188C5C());
        }

        if (!lbl_806E181D)
        {
            if ((from & 0x18) || (from == 1 && (nlTaskManager::m_pInstance->mPreviousState & 0x18)))
            {
                NisPlayer::Instance();
                fn_8027C86C();
                fn_80188360();
                fn_801882B4();
                WorldDarkening::Instance().fn_801AF550();
            }
        }
        else
        {
            lbl_806E181D = false;
        }
    }

    if ((from == 2 && to != 1) || (from == 1 && to != 2))
    {
        if (g_pBall != 0)
        {
            fn_800180AC(g_pBall);
        }
    }

    if (lbl_806E1860 != 0)
    {
        fn_801E23A4(lbl_806E1860, from, to);
    }

    if ((to & 0x18) || to == 0x20000)
    {
        if (from != 1 && to != 0x20000)
        {
            Presentation* presentation = fn_80284A58();
            presentation->mLetterBoxEnabled = true;

            for (i = 0; i < 2; i++)
            {
                fn_800A7998(lbl_806E0DF8[i]);
            }

            for (i = 0; i < 10; i++)
            {
                if (lbl_8056B800[i] != 0)
                {
                    lbl_8056B800[i]->ResetEffects();
                }
            }

            ClearCharacterEffectsTexturing();
        }
    }
    else if ((from & 0x18) || (from == 1 && (nlTaskManager::m_pInstance->mPreviousState & 0x18)))
    {
        if (to != 1 && to != 4)
        {
            Presentation* presentation = fn_80284A58();
            presentation->mLetterBoxEnabled = false;
            presentation->mLetterBoxDuration = 0.0f;

            for (i = 0; i < 10; i++)
            {
                if (lbl_8056B800[i] != 0)
                {
                    lbl_8056B800[i]->ResetEffects();
                }
            }

            ClearCharacterEffectsTexturing();
        }
        else if (to == 1)
        {
            if (lbl_806E0C94 != 0 && lbl_806E0C94->mState == 3)
            {
                Presentation* presentation = fn_80284A58();
                presentation->mLetterBoxEnabled = false;
                presentation->mLetterBoxDuration = 0.0f;
            }
        }
    }

    if (to == 8)
    {
        fn_800F23F4();
    }

    if (from == 8)
    {
        fn_800F2404();
    }

    nlTaskManager::m_pInstance->mLocked = false;
}

TransitionTask gTransitionTask;

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
