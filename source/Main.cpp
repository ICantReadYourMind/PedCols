#include "plugin.h"
#include "PedEx.h"

using namespace plugin;

class PedCols {
public:
    PedCols() {
        Events::initGameEvent.AddBefore(CPedModelInfoEx::LoadPedColours);

        Events::pedSetModelEvent.before += [](CPed* ped, int modelIndex) { // find a random ped colour and randomly enable props here
            auto* pedModelInfo = (CPedModelInfo*)CModelInfo::GetModelInfo(modelIndex);
            int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(pedModelInfo->m_szName, false);

            CPedEx pedEx;
            if (extendedModelIndex >= 0) {
                auto& pedModelInfoEx = CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];

                pedModelInfoEx.CPedModelInfoEx::FindEditableMaterialList(pedModelInfo);
                pedModelInfoEx.ChoosePedColour(pedEx.m_pedColour1, pedEx.m_pedColour2, pedEx.m_pedColour3, pedEx.m_pedColour4);
                pedModelInfoEx.ChoosePedProps(pedEx.m_bProp1On, pedEx.m_bProp2On, pedEx.m_bProp3On, pedEx.m_bProp4On);
            }
            CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped), true);
            CPedEx::extendedPedInfo.push_back(pedEx); // add to CPedEx "pool" and keep in sync with CPed
        };

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
            auto i = CPedEx::extendedPedInfo.begin() + x;

            if (i != CPedEx::extendedPedInfo.end())
                CPedEx::extendedPedInfo.erase(i);
        };
	};
} PedColsPlugin;
