#include "Game/FE/tlSlide.h"
#include "Game/FE/tlComponentInstance.h"

#include "NL/nlDLRing.h"
#include "NL/nlString.h"

extern const float lbl_806E66B8;

/**
 * Offset/Address/Size: 0x0 | 0x802FFAFC | size: 0x1D8
 */
void TLSlide::UpdateAsset(TLInstance* instance, float time)
{
    TLComponentInstance* var_r31;
    TLComponentInstance* var_r27;
    TLComponentInstance* var_r26;

    if (instance->pChildren == 0)
    {
        return;
    }

    var_r31 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(instance->pChildren);
    for (;;)
    {
        if (var_r31->m_type == TLAT_COMPONENT)
        {
            var_r31->Update(time);
        }

        if (var_r31->pChildren != 0)
        {
            TLComponentInstance* var_r28 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r31->pChildren);
            for (;;)
            {
                if (var_r28->m_type == TLAT_COMPONENT)
                {
                    var_r28->Update(time);
                }

                if (var_r28->pChildren != 0)
                {
                    var_r27 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r28->pChildren);
                    for (;;)
                    {
                        if (var_r27->m_type == TLAT_COMPONENT)
                        {
                            var_r27->Update(time);
                        }

                        if (var_r27->pChildren != 0)
                        {
                            var_r26 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(
                                var_r27->pChildren);
                            for (;;)
                            {
                                if (var_r26->m_type == TLAT_COMPONENT)
                                {
                                    var_r26->Update(time);
                                }

                                UpdateAsset(var_r26, time);
                                if (nlDLRingIsEnd<TLInstance>(var_r27->pChildren, var_r26) != 0)
                                {
                                    break;
                                }
                                var_r26 = (TLComponentInstance*)var_r26->m_next;
                            }
                        }

                        if (nlDLRingIsEnd<TLInstance>(var_r28->pChildren, var_r27) != 0)
                        {
                            break;
                        }
                        var_r27 = (TLComponentInstance*)var_r27->m_next;
                    }
                }

                if (nlDLRingIsEnd<TLInstance>(var_r31->pChildren, var_r28) != 0)
                {
                    break;
                }
                var_r28 = (TLComponentInstance*)var_r28->m_next;
            }
        }

        if (nlDLRingIsEnd<TLInstance>(instance->pChildren, var_r31) != 0)
        {
            break;
        }
        var_r31 = (TLComponentInstance*)var_r31->m_next;
    }
}

/**
 * Offset/Address/Size: 0x1D8 | 0x802FFCD4 | size: 0x308
 */
void TLSlide::Update(float time)
{
    if (field_0x44)
    {
        time = lbl_806E66B8;
    }

    float end = m_duration + m_start;
    m_time += time;
    if (m_time > end)
    {
        switch (m_uPlayMode)
        {
        case TLPM_LOOPING:
            m_time = m_time - end;
            break;
        case TLPM_STOP_AT_END:
            m_time = end;
            break;
        }
    }

    FEAnimation* anim = nlDLRingGetStart<FEAnimation>(this->m_animations);
    for (;;)
    {
        if (anim == 0)
        {
            break;
        }
        anim->Update(m_time);
        if (nlDLRingIsEnd<FEAnimation>(this->m_animations, anim) != 0)
        {
            break;
        }
        anim = anim->m_next;
    }

    TLComponentInstance* var_r31_2 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(this->m_instances);
    for (;;)
    {
        if (var_r31_2 == 0)
        {
            break;
        }
        if (var_r31_2->m_type == TLAT_COMPONENT)
        {
            var_r31_2->Update(time);
        }

        if (var_r31_2->pChildren != 0)
        {
            TLComponentInstance* var_r30 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r31_2->pChildren);
            for (;;)
            {
                if (var_r30->m_type == TLAT_COMPONENT)
                {
                    var_r30->Update(time);
                }

                if (var_r30->pChildren != 0)
                {
                    TLComponentInstance* var_r31 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r30->pChildren);
                    for (;;)
                    {
                        if (var_r31->m_type == TLAT_COMPONENT)
                        {
                            var_r31->Update(time);
                        }

                        if (var_r31->pChildren != 0)
                        {
                            TLComponentInstance* var_r26 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r31->pChildren);
                            for (;;)
                            {
                                if (var_r26->m_type == TLAT_COMPONENT)
                                {
                                    var_r26->Update(time);
                                }

                                if (var_r26->pChildren != 0)
                                {
                                    TLComponentInstance* var_r27 = (TLComponentInstance*)nlDLRingGetStart<TLInstance>(var_r26->pChildren);
                                    for (;;)
                                    {
                                        if (var_r27->m_type == TLAT_COMPONENT)
                                        {
                                            var_r27->Update(time);
                                        }

                                        UpdateAsset(var_r27, time);
                                        if (nlDLRingIsEnd<TLInstance>(var_r26->pChildren, var_r27) != 0)
                                        {
                                            break;
                                        }
                                        var_r27 = (TLComponentInstance*)var_r27->m_next;
                                    }
                                }

                                if (nlDLRingIsEnd<TLInstance>(var_r31->pChildren, var_r26) != 0)
                                {
                                    break;
                                }
                                var_r26 = (TLComponentInstance*)var_r26->m_next;
                            }
                        }

                        if (nlDLRingIsEnd<TLInstance>(var_r30->pChildren, var_r31) != 0)
                        {
                            break;
                        }
                        var_r31 = (TLComponentInstance*)var_r31->m_next;
                    }
                }

                if (nlDLRingIsEnd<TLInstance>(var_r31_2->pChildren, var_r30) != 0)
                {
                    break;
                }
                var_r30 = (TLComponentInstance*)var_r30->m_next;
            }
        }

        if (nlDLRingIsEnd<TLInstance>(this->m_instances, var_r31_2) != 0)
        {
            break;
        }
        var_r31_2 = (TLComponentInstance*)var_r31_2->m_next;
    }
}

/**
 * Offset/Address/Size: 0x4E0 | 0x802FFFDC | size: 0xA0
 */
TLSlide::TLSlide()
{
    m_animations = 0;
    m_time = lbl_806E66B8;
    m_uPlayMode = TLPM_LOOPING;
    field_0x44 = false;
    const char* name = "<undefined slide>";
    nlStrNCpy<char>(m_szName, name, sizeof(m_szName));
    m_hash = nlStringLowerHash(name);
}
