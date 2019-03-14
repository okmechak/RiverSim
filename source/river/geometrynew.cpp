#include "geometrynew.hpp"

namespace River
{
    BranchNew& BranchNew::Shrink(double lenght)
    {
        while(lenght > 0)
        {
            auto dl = TipVector().norm();
            if(lenght >= dl)
                RemoveTipPoint();
            else 
            {
                auto k = lenght/dl;
                auto new_tip = TipVector()*k;
                RemoveTipPoint();
                AddPoint(new_tip);
                lenght = 0;
            }
            lenght -= dl;
        }
        

        return *this;
    }

}