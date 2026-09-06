#ifndef UNCLASSIFIED_TU_80245DB4_H
#define UNCLASSIFIED_TU_80245DB4_H

struct TU80257D0CState
{
    void UnidentifiedRemoveStart()
    {
        if (mUnidentified08 != 0)
        {
            --mUnidentified08;
            ++mUnidentified04;
            if (mUnidentified04 % mUnidentified0C == 0)
            {
                mUnidentified04 = 0;
            }
        }
    }

    unsigned int* UnidentifiedAddEnd()
    {
        if (mUnidentified08 >= mUnidentified0C)
        {
            return 0;
        }
        return mUnidentified00 + (mUnidentified04 + mUnidentified08++) % mUnidentified0C;
    }

    /* 0x00 */ unsigned int* mUnidentified00;
    /* 0x04 */ unsigned int mUnidentified04;
    /* 0x08 */ int mUnidentified08;
    /* 0x0C */ unsigned int mUnidentified0C;
    /* 0x10 */ unsigned int mUnidentified10[3];
}; // size 0x1C

extern TU80257D0CState lbl_8057848C;

#endif // UNCLASSIFIED_TU_80245DB4_H
