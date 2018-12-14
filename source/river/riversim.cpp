#include "riversim.hpp"
namespace River
{
    namespace SimpleGeo{

        void AddBranches(Geometry &riverGeom, unsigned int id, double angle = 0.01, int size = 20, double dl = 0.02)
        {

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{dl, angle, id, 1}, true/*relative angle*/);

            if(size == 2)
                return;
            auto[idLeft, idRight] = riverGeom.AddBiffurcation(id, dl);
            AddBranches(riverGeom, idLeft, angle, size - 1);
            AddBranches(riverGeom, idRight, angle, size - 1);

        }

        Geometry CustomRiverTree(double dl, double eps)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(eps);
            riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);

            auto rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            AddBranches(riverGeom, rootBranchID, 0.001, 4);
            return riverGeom;
        }

        Geometry Box()
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(0.001);
            riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);

            return riverGeom;
        }
        
        Geometry SingleTip()
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(0.001);
            riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);

            auto rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            for (int i = 0; i < 10; ++i)
                riverGeom.addPolar(GeomPolar{0.02, 0, rootBranchID, 1}, true/*relative angle*/);

            return riverGeom;
        }
    }

}