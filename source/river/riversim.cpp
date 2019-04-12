/*
 * riversim - river growth simulation.
 * Copyright (c) 2019 Oleg Kmechak
 * Report issues: github.com/okmechak/RiverSim/issues
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "riversim.hpp"
namespace River
{
    void ForwardRiverEvolution(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, string file_name)
   {
        auto mesh = BoundaryGenerator(mdl, tree, border);
        
        //FIXME:
        //preparing mesh constraint function
        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);
        mesh.convert();
        mesh.write(file_name + ".msh");//FIXME

        //Simulation
        //Deal.II library
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(file_name + ".msh");
        sim.run();
        sim.output_results(file_name);

        for(auto id: tree.TipBranchesId())
        {
            auto tip_point = tree.GetBranch(id).TipPoint();
            auto tip_angle = tree.GetBranch(id).TipAngle();
            auto series_params = sim.integrate(mdl, tip_point, tip_angle);

            if(mdl.q_growth(series_params))
            {
                if(mdl.q_biffurcate(series_params))
                {
                    auto br_left = BranchNew(tip_point, tip_angle + mdl.biffurcation_angle);
                    br_left.AddPoint(Polar{mdl.ds, 0});
                    auto br_right = BranchNew(tip_point, tip_angle - mdl.biffurcation_angle);
                    br_right.AddPoint(Polar{mdl.ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.GetBranch(id).AddPoint(mdl.next_point(series_params));
            }
        }
        sim.clear();
   }


   void BackwardRiverEvolution(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, string file_name)
   {

       
   }
   
}//namespace River