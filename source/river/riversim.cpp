#include "riversim.hpp"
namespace River
{
    namespace SimpleGeo{

        void AddBranches(Geometry &riverGeom, int id, double angle = 0.01, int size = 20, double dl = 0.1)
        {

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{dl, angle, id}/*relative angle*/);

            if(size == 2)
                return;
            auto[idLeft, idRight] = riverGeom.AddBiffurcation(id, dl);
            AddBranches(riverGeom, idLeft, angle, size - 1);
            AddBranches(riverGeom, idRight, angle, size - 1);

        }

        Geometry CustomRiverTree(Model &model, int size)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            int rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            AddBranches(riverGeom, rootBranchID, 0.001, size, model.ds);
            return riverGeom;
        }

        Geometry Box(Model &model)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            return riverGeom;
        }
        
        Geometry SingleTip(Model &model, int size)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            auto rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{model.ds, 0, rootBranchID}/*relative angle*/);

            return riverGeom;
        }
    }

}