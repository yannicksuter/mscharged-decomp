#ifndef UNCLASSIFIED_TU_802492E4_H
#define UNCLASSIFIED_TU_802492E4_H

class TLComponentInstance;
struct TU8024A368Scene;

extern "C" void fn_802492E4(int mode, TLComponentInstance* breadcrumbs);
extern "C" void fn_8024960C(int mode, bool advance);
extern "C" void fn_80249800(int camera);
extern "C" void fn_80249A70(int camera);
extern "C" void fn_80249A94(int mode, bool advance);
extern "C" void fn_80249BF4(int mode);
extern "C" unsigned int fn_80249F50(int mode, int item);
extern "C" unsigned int fn_80249FF0(int mode, int item);
extern "C" int fn_8024A0E4();
extern "C" unsigned int fn_8024A0EC(unsigned int camera);
extern "C" void fn_8024A178();
extern "C" void fn_8024A290(float fDeltaT);
extern "C" void fn_8024A33C();
extern "C" bool fn_8024A34C();
extern "C" bool fn_8024A354();
extern "C" TU8024A368Scene* fn_8024A368(TU8024A368Scene* scene, int mode);
extern "C" TU8024A368Scene* fn_8024A6B0(TU8024A368Scene* scene, int deleteFlag);
extern "C" void fn_8024A7DC(TU8024A368Scene* scene);
extern "C" void fn_8024A990(TU8024A368Scene* scene, float fDeltaT);

#endif // UNCLASSIFIED_TU_802492E4_H
