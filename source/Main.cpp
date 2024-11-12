#include "plugin.h"
#include "PedEx.h"

using namespace plugin;

class PedCols {
public:
    PedCols() {
        Events::initGameEvent.AddBefore(CPedModelInfoEx::LoadPedColours);

        Events::pedSetModelEvent.before += [](CPed* ped, int modelIndex) { // find a random ped colour and randomly enable props here
            CPedModelInfo* mi = (CPedModelInfo*)CModelInfo::GetModelInfo(modelIndex);
            int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(mi->m_szName, false);

            CPedEx pedEx;
            if (extendedModelIndex >= 0) {
                CPedModelInfoEx* ex = &CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];

                ex->CPedModelInfoEx::FindEditableMaterialList(mi);
                ex->ChoosePedColour(pedEx.m_pedColour1, pedEx.m_pedColour2, pedEx.m_pedColour3, pedEx.m_pedColour4);
                ex->ChoosePedProps(pedEx.m_bProp1On, pedEx.m_bProp2On, pedEx.m_bProp3On, pedEx.m_bProp4On);
            }
            CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped), true);
            CPedEx::extendedPedInfo.push_back(pedEx); // add to CPedEx "pool" and keep in sync with CPed
        };

        Events::pedRenderEvent.before += [](CPed* ped) { // apply the ped colour and props here
            CPedModelInfo* mi = (CPedModelInfo*)CModelInfo::GetModelInfo(ped->m_nModelIndex);
            int extendedModelIndex = CPedModelInfoEx::GetExtendedModelIndex(mi->m_szName, false);

            if (extendedModelIndex >= 0) {
                CPedModelInfoEx* ex = &CPedModelInfoEx::extendedPedModelInfo[extendedModelIndex];
                CPedEx* pe = &CPedEx::extendedPedInfo[CPedEx::GetExtendedPedRef(CPools::GetPedRef(ped))];

                ex->SetPedColour(pe->m_pedColour1, pe->m_pedColour2, pe->m_pedColour3, pe->m_pedColour4);
                ex->SetPedProps(pe->m_bProp1On, pe->m_bProp2On, pe->m_bProp3On, pe->m_bProp4On);
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
