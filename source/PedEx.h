#pragma once
#include "CPed.h"
#include "CPedModelInfo.h"
#include "CClumpModelInfo.h"
#include "CModelInfo.h"

class CPedModelInfoEx {
public:
    static std::vector<RwRGBA> ms_pedColourTable;
    static std::vector<RwRGBA> ms_editableMaterialColours;
    static std::vector<short> ms_pedPropChances;
    static std::vector<CPedModelInfoEx> extendedPedModelInfo;

    std::vector<std::vector<RpMaterial*>> m_materials;
    std::vector<std::vector<short>> m_colours;

    std::vector<std::vector<RpMaterial*>> m_remapMaterials;
    std::vector<std::vector<std::string>> m_remapTextures;

    std::vector<std::vector<RpMaterial*>> m_skinRemapMaterials;
    std::vector<std::vector<std::string>> m_skinRemapTextures;

    size_t m_lastColorVariation;

    struct editableMatCBData
    {
        CPedModelInfoEx* ex;
    };

    void FindEditableMaterialList(CPedModelInfo* pedModelInfo);

    void ChoosePedColoursAndProps(std::vector<short>& colours, std::vector<bool>& props, bool useFirstColour = false);
    void ChooseVariablePedTextures(std::vector<std::string>& textures, size_t& skinColourIndex, bool useFirstTexture = false);

    void SetVariablePedTextures(const std::vector<std::string>& textures, const size_t& skinColourIndex, const int& txdIndex);
    void SetPedColoursAndProps(const std::vector<short>& colours, const std::vector<bool>& props);

    static int GetExtendedModelIndex(const std::string& modelName, bool setAsWell = false);
    static void LoadPedColours(bool reload = false);
};

class CPedEx {
public:
    static std::vector<CPedEx> extendedPedInfo;

    int pedRef;
    size_t m_currentSkinColourIndex;

    std::vector<short> m_pedColours;
    std::vector<bool> m_bPropsOn;
    std::vector<std::string> m_currentTextures;

    static int GetExtendedPedRef(const int& pedRef, bool setAsWell = false, bool erase = false);
};