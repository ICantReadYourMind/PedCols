#include "PedEx.h"
#include "CFileMgr.h"
#include <sstream>

std::vector<RwRGBA> CPedModelInfoEx::ms_pedColourTable;

std::vector<CPedModelInfoEx> CPedModelInfoEx::extendedPedModelInfo;
std::vector<CPedEx> CPedEx::extendedPedInfo;

// A lot of code here is based off of re3 code for vehicle colour loading.

int CPedEx::GetExtendedPedRef(int pedRef, bool setAsWell, bool erase) {
    static std::vector<int> pedRefs{};

    if (setAsWell) {
        pedRefs.push_back(pedRef);
        return 0;
    }

    auto find = std::find(pedRefs.begin(), pedRefs.end(), pedRef);

    if (find != pedRefs.end()) {
        int extendedPedRef = find - pedRefs.begin();
        if (erase) {
            pedRefs.erase(find);
        }
        return extendedPedRef;
    }
}

int CPedModelInfoEx::GetExtendedModelIndex(std::string modelName, bool setAsWell)
{
    static std::vector<std::string> names{};

    auto find = std::find(names.begin(), names.end(), modelName);

    if (find != names.end()) { // aka something found
        return find - names.begin();
    }
    else {
        if (setAsWell)
        {
            names.push_back(modelName);
            return 0;
        }
        else {
            return -999; // nothing found
        }
    }
}

void CPedModelInfoEx::LoadPedColours(void)
{
    int fd;
    int start, end;
    int section = 0;
    int r, g, b;

    char line[1024];

    enum
    {
        NONE,
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
        }
        else if (line[start] == 'e' && line[start + 1] == 'n' && line[start + 2] == 'd')
        {
            section = NONE;
        }
        else if (section == COLOURS)
        {
            sscanf(&line[start], "%d %d %d", &r, &g, &b);

            RwRGBA temp;
            temp.red = r;
            temp.green = g;
            temp.blue = b;
            temp.alpha = 0xFF;
            ms_pedColourTable.push_back(temp);
        }
        else if (section == PEDS)
        {   // using stringstream here to read the line for "infinite" amount of color sets per ped
            std::string strLine(line);
            std::string name;
            std::stringstream ss(strLine);

            ss >> name;

            CPedModelInfoEx pedEx;

            int num;
            while (ss >> num) {
                static UINT8 colourNumber = 1;

                if (!ss.fail()) {
                    switch (colourNumber)
                    {
                    case 1:
                        pedEx.m_colours1.push_back(num);
                        break;
                    case 2:
                        pedEx.m_colours2.push_back(num);
                        break;
                    case 3:
                        pedEx.m_colours3.push_back(num);
                        break;
                    case 4:
                        pedEx.m_colours4.push_back(num);
                        break;
                    }
                }

                if (colourNumber >= 4) {
                    colourNumber = 1;
                }
                else
                    ++colourNumber;
            }

            pedEx.m_numColours = pedEx.m_colours4.size();
            extendedPedModelInfo.push_back(pedEx);
            GetExtendedModelIndex(name, true);
        }
    }
    CFileMgr::CloseFile(fd);
    return;
}

void CPedModelInfoEx::FindEditableMaterialList(CPedModelInfo* mi)
{
    editableMatCBData cbdata;

    cbdata.ex = this;
    RpClumpForAllAtomics(mi->m_pClump, [](RpAtomic* atomic, void* data) {
        RpGeometryForAllMaterials(atomic->geometry, [](RpMaterial* material, void* data) {
            RwRGBA white = { 255, 255, 255, 255 };
            const RwRGBA* col = RpMaterialGetColor(material);
            editableMatCBData* cbdata = (editableMatCBData*)data;

            if (col->red == 0x00 && col->green == 0xFF && col->blue == 0x3C)
            {
                cbdata->ex->m_materials1.push_back(material);
                RpMaterialSetColor(material, &white);
            }
            else if (col->red == 0xFF && col->green == 0 && col->blue == 0xAF)
            {
                cbdata->ex->m_materials2.push_back(material);
                RpMaterialSetColor(material, &white);
            }
            else if (col->red == 0x3C && col->green == 0 && col->blue == 0xFF)
            {
                cbdata->ex->m_materials3.push_back(material);
                RpMaterialSetColor(material, &white);
            }
            else if (col->red == 0xFF && col->green == 0 && col->blue == 0x3C)
            {
                cbdata->ex->m_materials4.push_back(material);
                RpMaterialSetColor(material, &white);
            }
            return material;
            }, data);
        return atomic;
        }, &cbdata);

    m_currentColour1 = -1;
    m_currentColour2 = -1;
    m_currentColour3 = -1;
    m_currentColour4 = -1;
}

void CPedModelInfoEx::SetPedColour(const short& c1, const short& c2, const short& c3, const short& c4)
{
    RwRGBA desiredColour, *materialColour;
    
    if (c1 != m_currentColour1 && c1 != 0)
    {
        desiredColour = ms_pedColourTable[abs(c1)];
        for (const auto& material : m_materials1) {
            materialColour = (RwRGBA*)RpMaterialGetColor(material);
            *materialColour = desiredColour;
        }
        m_currentColour1 = c1;
    }

    if (c2 != m_currentColour2 && c2 != 0)
    {
        desiredColour = ms_pedColourTable[abs(c2)];
        for (const auto& material : m_materials2) {
            materialColour = (RwRGBA*)RpMaterialGetColor(material);
            *materialColour = desiredColour;
        }
        m_currentColour2 = c2;
    }

    if (c3 != m_currentColour3 && c3 != 0)
    {
        desiredColour = ms_pedColourTable[abs(c3)];
        for (const auto& material : m_materials3) {
            materialColour = (RwRGBA*)RpMaterialGetColor(material);
            *materialColour = desiredColour;
        }
        m_currentColour3 = c3;
    }

    if (c4 != m_currentColour4 && c4 != 0)
    {
        desiredColour = ms_pedColourTable[abs(c4)];
        for (const auto& material : m_materials4) {
            materialColour = (RwRGBA*)RpMaterialGetColor(material);
            *materialColour = desiredColour;
        }
        m_currentColour4 = c4;
    }
}

void CPedModelInfoEx::SetPedProps(const bool& prop1, const bool& prop2, const bool& prop3, const bool& prop4)
{
    RwRGBA *colp;

    for (const auto& material : m_materials1) {
        colp = (RwRGBA*)RpMaterialGetColor(material);
        colp->alpha = prop1 ? 255 : 0;
    }

    for (const auto& material : m_materials2) {
        colp = (RwRGBA*)RpMaterialGetColor(material);
        colp->alpha = prop2 ? 255 : 0;
    }

    for (const auto& material : m_materials3) {
        colp = (RwRGBA*)RpMaterialGetColor(material);
        colp->alpha = prop3 ? 255 : 0;
    }

    for (const auto& material : m_materials4) {
        colp = (RwRGBA*)RpMaterialGetColor(material);
        colp->alpha = prop4 ? 255 : 0;
    }
}

void CPedModelInfoEx::ChoosePedColour(short& col1, short& col2, short& col3, short& col4)
{
    if (m_numColours < 1)
        return;
    m_lastColorVariation = rand() % m_numColours;
    col1 = m_colours1[m_lastColorVariation];
    col2 = m_colours2[m_lastColorVariation];
    col3 = m_colours3[m_lastColorVariation];
    col4 = m_colours4[m_lastColorVariation];
}

void CPedModelInfoEx::ChoosePedProps(bool& prop1, bool& prop2, bool& prop3, bool& prop4) {
    if (m_colours1[m_lastColorVariation] < 0)
        prop1 = rand() % 2;
    else
        prop1 = true;
    if (m_colours2[m_lastColorVariation] < 0)
        prop2 = rand() % 2;
    else
        prop2 = true;
    if (m_colours3[m_lastColorVariation] < 0)
        prop3 = rand() % 2;
    else
        prop3 = true;
    if (m_colours4[m_lastColorVariation] < 0)
        prop4 = rand() % 2;
    else
        prop4 = true;
}