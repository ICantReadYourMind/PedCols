#include "plugin.h"
#include "PedEx.h"
#include "ModuleList.hpp"

using namespace plugin;

void SetModelIndexEx(CPed* ped, int modelIndex) {
    // find a random ped colour and randomly enable props here
    auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(modelIndex);
    int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName, false);

    if (extendedModelIndex >= 0) {
        CPedEx pedEx;
        auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];

        pedModelInfoEx.FindEditableMaterialList(pedModelInfo);
        pedModelInfoEx.ChoosePedColour(pedEx.m_pedColour1, pedEx.m_pedColour2, pedEx.m_pedColour3, pedEx.m_pedColour4);
        pedModelInfoEx.ChoosePedProps(pedEx.m_bProp1On, pedEx.m_bProp2On, pedEx.m_bProp3On, pedEx.m_bProp4On);

        CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped), true);
        CPedEx::extendedPedInfo.push_back(pedEx); // add to CPedEx "pool" and keep in sync with CPed
    }
}

class PedCols {
public:
    PedCols() {

        Events::initGameEvent.before += []() {
            CPedModelInfoEx::LoadPedColours();
        };

        ModuleList modList;
        HMODULE iii_anim = modList.Get(L"iii_anim");

        if (iii_anim != 0) { // compatibility with skins and bones
            plugin::ThiscallEvent <plugin::AddressList<0x4C52A0, plugin::H_JUMP>, plugin::PRIORITY_BEFORE, plugin::ArgPick2N<CPed*, 0, int, 1>, void(CPed*, int)> xboxSetModelEvent;
            xboxSetModelEvent.AddAfter(SetModelIndexEx);
        }
        else {
            Events::pedSetModelEvent.AddAfter(SetModelIndexEx);
        }

        Events::pedRenderEvent.before += [](CPed* ped) { // apply the ped colour and props here
            auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(ped->m_nModelIndex);
            int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName, false);

            if (extendedModelIndex >= 0) {
                auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];
                auto& pedEx = CPedEx::extendedPedInfo[CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped))];

                pedModelInfoEx.SetPedColour(pedEx.m_pedColour1, pedEx.m_pedColour2, pedEx.m_pedColour3, pedEx.m_pedColour4);
                pedModelInfoEx.SetPedProps(pedEx.m_bProp1On, pedEx.m_bProp2On, pedEx.m_bProp3On, pedEx.m_bProp4On);
            }
        };

        Events::pedDtorEvent.before += [](CPed* ped) { // get rid of CPedEx objects here
            int x = CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped), false, true);
            if (x >= 0) {
                auto i = CPedEx::extendedPedInfo.begin() + x;

                if (i != CPedEx::extendedPedInfo.end())
                    CPedEx::extendedPedInfo.erase(i);
            }
        };
	};
} PedColsPlugin;
