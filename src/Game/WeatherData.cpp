#include "Game/WeatherData.h"
#include "decomp.h"

// Chain lightning paths for the left half of the field.
static nlVector3 sLeftChainPath0[] = {
    { -3.51f, 3.5f, 0.0f },
    { -4.64f, 4.61f, 0.0f },
    { -4.64f, 7.64f, 0.0f },
    { -6.3f, 9.31f, 0.0f },
    { -19.12f, 9.31f, 0.0f },
};

static nlVector3 sLeftChainPath1[] = {
    { -4.95f, -0.03f, 0.0f },
    { -7.28f, -0.03f, 0.0f },
    { -7.28f, 4.84f, 0.0f },
    { -11.58f, 4.84f, 0.0f },
    { -12.1f, 6.15f, 0.0f },
    { -19.67f, 6.15f, 0.0f },
};

static nlVector3 sLeftChainPath2[] = {
    { -4.95f, -0.03f, 0.0f },
    { -9.15f, -0.03f, 0.0f },
    { -11.22f, -2.03f, 0.0f },
    { -13.56f, -2.03f, 0.0f },
    { -13.56f, 4.63f, 0.0f },
    { -19.12f, 4.63f, 0.0f },
};

static nlVector3 sLeftChainPath3[] = {
    { -4.95f, -0.3f, 0.0f },
    { -9.15f, -0.3f, 0.0f },
    { -11.22f, -2.03f, 0.0f },
    { -13.56f, -2.03f, 0.0f },
    { -13.56f, -4.65f, 0.0f },
    { -19.04f, -4.65f, 0.0f },
};

static nlVector3 sLeftChainPath4[] = {
    { -3.52f, -3.51f, 0.0f },
    { -5.36f, -5.33f, 0.0f },
    { -9.18f, -5.33f, 0.0f },
    { -9.18f, -8.9f, 0.0f },
    { -15.13f, -8.9f, 0.0f },
    { -16.84f, -7.22f, 0.0f },
    { -19.82f, -7.22f, 0.0f },
};

static nlVector3 sLeftChainPath5[] = {
    { 0.0f, -4.99f, 0.0f },
    { 0.0f, -8.51f, 0.0f },
    { -1.01f, -9.21f, 0.0f },
    { -1.01f, -11.08f, 0.0f },
};

// Chain lightning paths for the right half of the field.
static nlVector3 sRightChainPath0[] = {
    { 0.02f, 4.93f, 0.0f },
    { 0.02f, 8.53f, 0.0f },
    { 1.01f, 9.22f, 0.0f },
    { 1.01f, 10.96f, 0.0f },
};

static nlVector3 sRightChainPath1[] = {
    { 3.58f, 3.57f, 0.0f },
    { 5.56f, 5.57f, 0.0f },
    { 6.88f, 5.57f, 0.0f },
    { 6.85f, 11.67f, 0.0f },
};

static nlVector3 sRightChainPath2[] = {
    { 5.02f, 0.02f, 0.0f },
    { 7.68f, 0.02f, 0.0f },
    { 10.45f, 2.36f, 0.0f },
    { 10.45f, 7.87f, 0.0f },
    { 19.84f, 7.87f, 0.0f },
};

static nlVector3 sRightChainPath3[] = {
    { 3.58f, -3.57f, 0.0f },
    { 5.49f, -5.51f, 0.0f },
    { 9.06f, -5.51f, 0.0f },
    { 11.09f, -3.46f, 0.0f },
    { 13.6f, -3.46f, 0.0f },
    { 13.6f, 4.63f, 0.0f },
    { 19.13f, 4.63f, 0.0f },
};

static nlVector3 sRightChainPath4[] = {
    { 3.58f, -3.57f, 0.0f },
    { 5.49f, -5.51f, 0.0f },
    { 9.06f, -5.51f, 0.0f },
    { 11.09f, -3.46f, 0.0f },
    { 13.6f, -3.46f, 0.0f },
    { 13.6f, -4.64f, 0.0f },
    { 19.1f, -4.64f, 0.0f },
};

static nlVector3 sRightChainPath5[] = {
    { 3.58f, -3.57f, 0.0f },
    { 5.49f, -5.51f, 0.0f },
    { 7.32f, -5.51f, 0.0f },
    { 7.32f, -8.39f, 0.0f },
    { 10.62f, -8.39f, 0.0f },
    { 11.15f, -9.68f, 0.0f },
    { 19.15f, -9.68f, 0.0f },
};

// Sand patch centres (x, y, z) and radii (w) on the left half of the field.
static nlVector4 sSandPatches[] = {
    { -19.53f, -17.05f, 0.0f, 12.04f },
    { -9.09f, -13.63f, 0.0f, 5.88f },
    { 5.27f, -20.02f, 0.0f, 12.54f },
    { 21.79f, -19.57f, 0.0f, 14.0f },
};

static nlVector3* sRightChainPaths[6];
static int sRightChainPathLengths[6];
static nlVector3* sLeftChainPaths[6];
static int sLeftChainPathLengths[6];

void InitChainLightningPaths()
{
    sRightChainPaths[0] = sRightChainPath0;
    sRightChainPathLengths[0] = ARRAY_COUNT(sRightChainPath0);
    sRightChainPaths[1] = sRightChainPath1;
    sRightChainPathLengths[1] = ARRAY_COUNT(sRightChainPath1);
    sRightChainPaths[2] = sRightChainPath2;
    sRightChainPathLengths[2] = ARRAY_COUNT(sRightChainPath2);
    sRightChainPaths[3] = sRightChainPath3;
    sRightChainPathLengths[3] = ARRAY_COUNT(sRightChainPath3);
    sRightChainPaths[4] = sRightChainPath4;
    sRightChainPathLengths[4] = ARRAY_COUNT(sRightChainPath4);
    sRightChainPaths[5] = sRightChainPath5;
    sRightChainPathLengths[5] = ARRAY_COUNT(sRightChainPath5);

    sLeftChainPaths[0] = sLeftChainPath0;
    sLeftChainPathLengths[0] = ARRAY_COUNT(sLeftChainPath0);
    sLeftChainPaths[1] = sLeftChainPath1;
    sLeftChainPathLengths[1] = ARRAY_COUNT(sLeftChainPath1);
    sLeftChainPaths[2] = sLeftChainPath2;
    sLeftChainPathLengths[2] = ARRAY_COUNT(sLeftChainPath2);
    sLeftChainPaths[3] = sLeftChainPath3;
    sLeftChainPathLengths[3] = ARRAY_COUNT(sLeftChainPath3);
    sLeftChainPaths[4] = sLeftChainPath4;
    sLeftChainPathLengths[4] = ARRAY_COUNT(sLeftChainPath4);
    sLeftChainPaths[5] = sLeftChainPath5;
    sLeftChainPathLengths[5] = ARRAY_COUNT(sLeftChainPath5);
}

int GetNumChainLightningPaths()
{
    return ARRAY_COUNT(sLeftChainPaths);
}

nlVector3* GetLeftChainLightningPath(int index)
{
    return sLeftChainPaths[index];
}

int GetLeftChainLightningPathLength(int index)
{
    return sLeftChainPathLengths[index];
}

nlVector3* GetRightChainLightningPath(int index)
{
    return sRightChainPaths[index];
}

int GetRightChainLightningPathLength(int index)
{
    return sRightChainPathLengths[index];
}

int GetNumSandPatches(SandTombWeather*)
{
    return ARRAY_COUNT(sSandPatches);
}

nlVector4 GetSandPatch(SandTombWeather*, int index, bool side)
{
    nlVector4 patch = sSandPatches[index];
    if (side == true)
    {
        patch.x *= -1.0f;
        patch.y *= -1.0f;
    }
    return patch;
}
