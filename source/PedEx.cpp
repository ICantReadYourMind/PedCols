#include "PedEx.h"
#include "CTxdStore.h"
#include "CFileMgr.h"
#include <sstream>

std::vector<RwRGBA> CPedModelInfoEx::ms_pedColourTable;
std::vector<RwRGBA> CPedModelInfoEx::ms_editableMaterialColours;
std::vector<short> CPedModelInfoEx::ms_pedPropChances;

std::vector<CPedModelInfoEx> CPedModelInfoEx::extendedPedModelInfo;
std::vector<CPedEx> CPedEx::extendedPedInfo;

// A lot of code here is based off of re3 code for vehicle colour loading.

int CPedEx::GetExtendedPedRef(const int& pedRef, bool setAsWell, bool erase) {
    static std::vector<int> pedRefs{};

    if (setAsWell)
    {
        pedRefs.push_back(pedRef);
        return 0;
    }

    auto find = std::find(pedRefs.begin(), pedRefs.end(), pedRef);

    if (find != pedRefs.end())
    { // aka something found
        int extendedPedRef = find - pedRefs.begin();
        if (erase) {
            pedRefs.erase(find);
        }
        return extendedPedRef;
    }

    return -999; // nothing found
}

int CPedModelInfoEx::GetExtendedModelIndex(const std::string& modelName, bool setAsWell)
{
    static std::vector<std::string> names{};

    if (setAsWell)
    {
        names.push_back(modelName);
        return 0;
    }

    auto find = std::find(names.begin(), names.end(), modelName);

    if (find != names.end())
    { // aka something found
        return find - names.begin();
    }

    return -999; // nothing found
}

void CPedModelInfoEx::LoadPedColours(bool reload)
{
    static bool firstInit = true;

    if (!firstInit && !reload)
    {
        for (auto& pedModelInfoEx : CPedModelInfoEx::extendedPedModelInfo)
        {
            for (auto& materials : pedModelInfoEx.m_materials)
            {
                materials.clear(); // should remove all editable material pointers when restarting game
            }
            for (auto& remapMaterials : pedModelInfoEx.m_varMaterials)
            {
                remapMaterials.clear();
            }
            for (auto& skinRemapMaterials : pedModelInfoEx.m_skinVarMaterials)
            {
                skinRemapMaterials.clear();
            }
        }
        return;
    }

    int fd;
    int start, end;
    int section = 0;
    int r, g, b;
    int chance = 2;
    size_t count = 0;

    char line[1024];

    enum
    {
        NONE,
        MATERIALS,
        COLOURS,
        PEDS
    };

    CFileMgr::ChangeDir("\\DATA\\");
    fd = CFileMgr::OpenFile("PEDCOLS.DAT", "r");
    CFileMgr::ChangeDir("\\");

    while (CFileMgr::ReadLine(fd, line, sizeof(line)))
    {
        // find first valid character in line
        for (start = 0;; start++)
            if (line[start] > ' ' || line[start] == '\0' || line[start] == '\n')
                break;
        // find end of line
        for (end = start;; end++)
        {
            if (line[end] == '\0' || line[end] == '\n')
                break;
            if (line[end] == ',' || line[end] == '\r')
                line[end] = ' ';
        }

        line[end] = '\0';

        // empty line
        if (line[start] == '#' || line[start] == '\0')
            continue;

        if (section == NONE)
        {
            if (line[start] == 'c' && line[start + 1] == 'o' && line[start + 2] == 'l')
                section = COLOURS;
            if (line[start] == 'p' && line[start + 1] == 'e' && line[start + 2] == 'd')
                section = PEDS;
            if (line[start] == 'm' && line[start + 1] == 'a' && line[start + 2] == 't' && !reload)
                section = MATERIALS;
        }
        else if (line[start] == 'e' && line[start + 1] == 'n' && line[start + 2] == 'd')
        {
            section = NONE;
        }
        else if (section == MATERIALS)
        {
            sscanf(&line[start], "%d %d %d", &r, &g, &b);

            RwRGBA temp;
            temp.red = r;
            temp.green = g;
            temp.blue = b;
            temp.alpha = 0xFF;
            ms_editableMaterialColours.push_back(temp);
        }
        else if (section == COLOURS)
        {
            int found = sscanf(&line[start], "%d %d %d %d", &r, &g, &b, &chance);

            RwRGBA temp;
            temp.red = r;
            temp.green = g;
            temp.blue = b;
            temp.alpha = 0xFF;
            if (count < ms_pedColourTable.size() && reload)
            {
                ms_pedPropChances[count] = chance;
                ms_pedColourTable[count] = temp;
                ++count;
            }
            else {
                ms_pedPropChances.push_back(chance);
                ms_pedColourTable.push_back(temp);
            }
        }
        else if (section == PEDS)
        {   // using stringstream here to read the line for "infinite" amount of color sets per ped
            std::string strLine(line);
            std::string pedName;
            std::stringstream ss(strLine);

            int num;
            CPedModelInfoEx pedModelInfoEx;

            ss >> pedName;

            if (ms_editableMaterialColours.empty())
            { // use default material colours from VCS if there are no material colours set in pedcols.dat
                ms_editableMaterialColours.push_back({ 0, 255, 60 });
                ms_editableMaterialColours.push_back({ 255, 0, 175 });
                ms_editableMaterialColours.push_back({ 60, 0, 255 });
                ms_editableMaterialColours.push_back({ 255, 0, 60 });
            }

            pedModelInfoEx.m_colours.resize(ms_editableMaterialColours.size());
            pedModelInfoEx.m_materials.resize(ms_editableMaterialColours.size());

            while (ss >> num)
            {
                static UINT8 colourNumber = 0;

                if (!ss.fail())
                    pedModelInfoEx.m_colours[colourNumber].push_back(num);

                if (colourNumber >= ms_editableMaterialColours.size() - 1)
                    colourNumber = 0;
                else
                    ++colourNumber;
            }

            if (reload)
            {
                int extendedModelIndex = GetExtendedModelIndex(pedName);
                if (extendedModelIndex < 0)
                    GetExtendedModelIndex(pedName, true);

                extendedPedModelInfo[extendedModelIndex].m_colours = pedModelInfoEx.m_colours;
            }
            else
            {
                extendedPedModelInfo.push_back(pedModelInfoEx);
                GetExtendedModelIndex(pedName, true);
            }
        }
    }
    firstInit = false;
    CFileMgr::CloseFile(fd);
    return;
}

void CPedModelInfoEx::FindEditableMaterialList(CPedModelInfo* pedModelInfo)
{
    editableMatCBData cbdata;

    cbdata.ex = this;
    RpClumpForAllAtomics(pedModelInfo->m_pClump, [](RpAtomic* atomic, void* data)
    {
        RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial* material, void* data)
        {
            RwRGBA white = { 255, 255, 255, 255 };
            const RwRGBA* materialColour = RpMaterialGetColor(material);
            editableMatCBData* cbdata = (editableMatCBData*)data;

            for (size_t i = 0; i < ms_editableMaterialColours.size(); ++i)
            {
                if (materialColour->red == ms_editableMaterialColours[i].red &&
                    materialColour->green == ms_editableMaterialColours[i].green &&
                    materialColour->blue == ms_editableMaterialColours[i].blue)
                {
                    cbdata->ex->m_materials[i].push_back(material);
                    RpMaterialSetColor(material, &white);
                }
            }
            if (RpMaterialGetTexture(material)) // check if material has texture that can be varied
            {
                editableMatCBData* cbdata = (editableMatCBData*)data;
                int x, y;

                int index = sscanf(RwTextureGetName(RpMaterialGetTexture(material)), "var%d_%d", &x, &y);

                if (index > 0)
                    cbdata->ex->m_varMaterials[x].push_back(material);

                int skinIndex = sscanf(RwTextureGetName(RpMaterialGetTexture(material)), "skin%d_%d", &x, &y);

                if (skinIndex > 0)
                    cbdata->ex->m_skinVarMaterials[y].push_back(material);
            }
            return material;
        }, data);
        return atomic;
    }, &cbdata);
}

void CPedModelInfoEx::FindPedVariations(const int& txdIndex)
{
    CTxdStore::SetCurrentTxd(txdIndex);
    RwTexDictionary* currentDict = RwTexDictionaryGetCurrent();

    if (currentDict) // find remappable textures here
    {
        if (!rwLinkListEmpty(&currentDict->texturesInDict))
        {
            RwLLLink* current = rwLinkListGetFirstLLLink(&currentDict->texturesInDict);
            RwLLLink* end = rwLinkListGetTerminator(&currentDict->texturesInDict);

            while (current != end) {
                const RwTexture* texture = rwLLLinkGetData(current, RwTexture, name);
                const char* textureName = texture->name + 8; // have to get pointer to name offset by 8, for some reason 
                // above statement returns some wrong values for first 8 elements of the name array
                size_t materialIndex;
                size_t textureIndex;

                size_t skinColourIndex;
                size_t skinTextureIndex;

                bool found = sscanf(textureName, "var%d_%d", &materialIndex, &textureIndex);
                bool skinFound = sscanf(textureName, "skin%d_%d", &skinColourIndex, &skinTextureIndex);

                if (found)
                {
                    if (materialIndex >= m_varTextures.size()) {
                        m_varTextures.resize(materialIndex + 1);
                        m_varMaterials.resize(materialIndex + 1);
                    }

                    if (textureIndex >= m_varTextures[materialIndex].size())
                        m_varTextures[materialIndex].resize(textureIndex + 1);

                    m_varTextures[materialIndex][textureIndex] = textureName;
                }

                if (skinFound)
                {
                    if (skinColourIndex >= m_skinVarTextures.size()) {
                        m_skinVarTextures.resize(skinColourIndex + 1);
                    }

                    if (skinTextureIndex >= m_skinVarTextures[skinColourIndex].size())
                    {
                        m_skinVarTextures[skinColourIndex].resize(skinTextureIndex + 1);
                        m_skinVarMaterials.resize(skinTextureIndex + 1);
                    }

                    m_skinVarTextures[skinColourIndex][skinTextureIndex] = textureName;
                }
                current = rwLLLinkGetNext(current);
            }
        }
    }
}

void CPedModelInfoEx::SetPedColoursAndProps(const std::vector<short>& colours, const std::vector<bool>& props)
{
    RwRGBA desiredColour;
    
    for (size_t i = 0; i < m_materials.size(); ++i)
    {
        if (colours[i] != 0)
        {
            desiredColour = ms_pedColourTable[abs(colours[i])];
            desiredColour.alpha = props[i] ? 255 : 0;
    
            for (auto& material : m_materials[i])
                RpMaterialSetColor(material, &desiredColour);
        }
    }
}

void CPedModelInfoEx::ChoosePedColoursAndProps(std::vector<short>& colours, std::vector<bool>& props, bool useFirstColour)
{
    if (m_colours.back().empty())
        return;

    m_lastColorVariation = rand() % m_colours.back().size();

    colours.resize(m_colours.size());
    props.resize(m_colours.size());

    for (size_t i = 0; i < m_colours.size(); ++i)
    {
        short colour;

        if (useFirstColour)
            colour = m_colours[i][0];
        else
            colour = m_colours[i][m_lastColorVariation];

        short pedPropChance = ms_pedPropChances[abs(colour)];
        colours[i] = colour;

        if (colour < 0)
        {
            if (pedPropChance > 0)
                props[i] = rand() % abs(pedPropChance);
            else
                props[i] = !(rand() % abs(pedPropChance));
        }
        else
            props[i] = true;
    }
}

void CPedModelInfoEx::ChooseVariablePedTextures(std::vector<RwTexture*>& textures, std::vector<RwTexture*>& skinTextures, const int& txdIndex, bool useFirstTexture)
{
    CTxdStore::SetCurrentTxd(txdIndex);
    textures.resize(m_varMaterials.size());
    skinTextures.resize(m_skinVarMaterials.size());

    for (size_t i = 0; i < textures.size(); ++i)
    {
        if (!m_varTextures[i].empty())
        {
            if (useFirstTexture)
                textures[i] = RwTextureRead(m_varTextures[i][0].c_str(), 0);
            else
                textures[i] = RwTextureRead(m_varTextures[i][rand() % m_varTextures[i].size()].c_str(), 0);
        }
    }

    if (!m_skinVarTextures.empty())
    {
        size_t skinColourIndex;
        if (useFirstTexture)
            skinColourIndex = 0;
        else
            skinColourIndex = rand() % m_skinVarTextures.size();
        if (!m_skinVarTextures[skinColourIndex].empty())
        {
            for (size_t i = 0; i < m_skinVarMaterials.size(); ++i)
            {
                skinTextures[i] = RwTextureRead(m_skinVarTextures[skinColourIndex][i].c_str(), 0);
            }
        }
    }
}

void CPedModelInfoEx::SetVariablePedTextures(const std::vector<RwTexture*>& textures, const std::vector<RwTexture*> skinTextures)
{
    for (size_t i = 0; i < m_varMaterials.size(); ++i)
    {
        for (auto& varMaterial : m_varMaterials[i])
        {
            RpMaterialSetTexture(varMaterial, textures[i]);
        }
    }

    for (size_t i = 0; i < m_skinVarMaterials.size(); ++i)
    {
        for (auto& skinVarMaterial : m_skinVarMaterials[i])
        {
            RpMaterialSetTexture(skinVarMaterial, skinTextures[i]);
        }
    }
}