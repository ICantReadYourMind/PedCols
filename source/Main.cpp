#include "plugin.h"
#include "PedEx.h"

#ifdef GTA3
#include "ModuleList.hpp"
#endif

using namespace plugin;

void SetModelIndexEx(CPed* ped, int modelIndex)
{   // find random ped colours, variable textures and randomly enable props here
    auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(modelIndex);
    int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName, false);

    if (extendedModelIndex >= 0)
    {
        CPedEx pedEx;
        pedEx.pedRef = CPools::GetPedRef(ped);
        auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];

        pedModelInfoEx.FindEditableMaterialList(pedModelInfo);

        pedModelInfoEx.ChoosePedColoursAndProps(pedEx.m_pedColours, pedEx.m_bPropsOn);
        pedModelInfoEx.ChooseVariablePedTextures(pedEx.m_currentTextures);

        CPedEx::GetExtendedPedRef(pedEx.pedRef, true);
        CPedEx::extendedPedInfo.push_back(pedEx); // add to CPedEx "pool" and keep in sync with CPed
    }
}

class PedCols {
public:
    PedCols()
    {
#ifdef GTA3
        plugin::config_file settings(PLUGIN_PATH("PedColsIII.ini"));
#else
        plugin::config_file settings(PLUGIN_PATH("PedColsVC.ini"));
#endif
        std::string reloadAndSetFirstColourKeyCode;
        std::string reloadAndSetRandomColourKeyCode;
        int reloadAndSetFirstColourKey;
        int reloadAndSetRandomColourKey;

        reloadAndSetFirstColourKeyCode = settings["ReloadAndSetFirstColourKey"].asString("0x00");
        reloadAndSetRandomColourKeyCode = settings["ReloadAndSetRandomColourKey"].asString("0x00");

        sscanf(reloadAndSetFirstColourKeyCode.c_str(), "%x", &reloadAndSetFirstColourKey);
        sscanf(reloadAndSetRandomColourKeyCode.c_str(), "%x", &reloadAndSetRandomColourKey);

        Events::initGameEvent.after += []()
        {
            CPedModelInfoEx::LoadPedColours();
        };

#ifdef GTA3
        ModuleList modList;
        HMODULE iii_anim = modList.Get(L"iii_anim");

        if (iii_anim != 0) { // compatibility with skins and bones
            ThiscallEvent <AddressList<0x4C52A0, H_JUMP>, PRIORITY_BEFORE, ArgPick2N<CPed*, 0, int, 1>, void(CPed*, int)> xboxSetModelEvent;
            xboxSetModelEvent.AddAfter(SetModelIndexEx);
        }
        else
#endif
            Events::pedSetModelEvent.AddAfter(SetModelIndexEx);

        Events::pedRenderEvent.before += [](CPed* ped)
        { // apply the ped colour and props here
            auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(ped->m_nModelIndex);
            int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName, false);

            if (extendedModelIndex >= 0) {
                auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];
                auto& pedEx = CPedEx::extendedPedInfo[CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped))];

                pedModelInfoEx.SetVariablePedTextures(pedEx.m_currentTextures, pedModelInfo->m_nTxdIndex);
                pedModelInfoEx.SetPedColoursAndProps(pedEx.m_pedColours, pedEx.m_bPropsOn);
            }
        };

        Events::gameProcessEvent += [reloadAndSetFirstColourKey, reloadAndSetRandomColourKey](void)
        { // reload colours, textures and props on keypress here
            if (GetAsyncKeyState(reloadAndSetFirstColourKey) || GetAsyncKeyState(reloadAndSetRandomColourKey))
            {
                CPedModelInfoEx::LoadPedColours(true);

                for (auto& pedEx : CPedEx::extendedPedInfo)
                {
                    auto* ped = CPools::GetPed(pedEx.pedRef);
                    auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(ped->m_nModelIndex);
                    auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName)];

                    if (GetAsyncKeyState(reloadAndSetFirstColourKey))
                    {
                        pedModelInfoEx.ChoosePedColoursAndProps(pedEx.m_pedColours, pedEx.m_bPropsOn);
                        pedModelInfoEx.ChooseVariablePedTextures(pedEx.m_currentTextures);
                    }
                    else if (GetAsyncKeyState(reloadAndSetRandomColourKey))
                    {
                        pedModelInfoEx.ChoosePedColoursAndProps(pedEx.m_pedColours, pedEx.m_bPropsOn, true);
                        pedModelInfoEx.ChooseVariablePedTextures(pedEx.m_currentTextures, true);
                    }
                }
            }
        };

        Events::pedDtorEvent.before += [](CPed* ped)
        { // get rid of CPedEx objects here
            int x = CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped), false, true);
            if (x >= 0) {
                auto i = CPedEx::extendedPedInfo.begin() + x;

                if (i != CPedEx::extendedPedInfo.end())
                    CPedEx::extendedPedInfo.erase(i);
            }
        };
	};
} PedColsPlugin;
