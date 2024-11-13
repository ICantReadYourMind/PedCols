#pragma once
#include "CPed.h"
#include "CPedModelInfo.h"
#include "CClumpModelInfo.h"
#include "CModelInfo.h"

class CPedModelInfoEx {
public:
    static std::vector<RwRGBA> ms_pedColourTable;
    static std::vector<CPedModelInfoEx> extendedPedModelInfo;

    std::vector<RpMaterial*> m_materials1;
    std::vector<RpMaterial*> m_materials2;
    std::vector<RpMaterial*> m_materials3;
    std::vector<RpMaterial*> m_materials4;

    std::vector<short> m_colours1;
    std::vector<short> m_colours2;
    std::vector<short> m_colours3;
    std::vector<short> m_colours4;

    unsigned short m_numColours;
    short m_lastColorVariation;

    short m_currentColour1;
    short m_currentColour2;
    short m_currentColour3;
    short m_currentColour4;

    struct editableMatCBData
    {
        CPedModelInfoEx* ex;
    };

    void ChoosePedColour(short& col1, short& col2, short& col3, short& col4);
    void ChoosePedProps(bool& prop1, bool& prop2, bool& prop3, bool& prop4);

    void SetPedColour(const short& c1, const short& c2, const short& c3, const short& c4);
    void SetPedProps(const bool& prop1, const bool& prop2, const bool& prop3, const bool& prop4);

    void FindEditableMaterialList(CPedModelInfo* mi);

    static int GetExtendedModelIndex(std::string modelName, bool setAsWell = false);

    static void LoadPedColours(void);
};

class CPedEx {
public:
    static std::vector<CPedEx> extendedPedInfo;

    short m_pedColour1;
    short m_pedColour2;
    short m_pedColour3;
    short m_pedColour4;

    bool m_bProp1On;
    bool m_bProp2On;
    bool m_bProp3On;
    bool m_bProp4On;

    static int GetExtendedPedRef(int pedRef, bool setAsWell = false, bool erase = false);
};