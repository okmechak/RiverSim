#include "riversim.hpp"
namespace River
{
    namespace SimpleGeo{

        void AddBranches(Geometry &riverGeom, unsigned int id, double angle = 0.01, int size = 20, double dl = 0.01)
        {

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{dl, angle, id}, true/*relative angle*/);

            if(size < 21)
                return;
            auto[idLeft, idRight] = riverGeom.AddBiffurcation(id, dl);
            cout << "Ids" << endl;
            cout << idLeft << " " << idRight << endl;
            AddBranches(riverGeom, idLeft, angle, size - 5);
            AddBranches(riverGeom, idRight, angle, size - 5);

        }

        Geometry CustomRiverTree(double dl, double eps)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(eps);
            riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);

            auto rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            AddBranches(riverGeom, rootBranchID, 0.001, 25);
            return riverGeom;
        }

        Geometry Box()
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(0.001);
            riverGeom.SetSquareBoundary({0., 0.}, {1., 1.}, 0.5);

            return riverGeom;
        }
    }

}