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

/** @file riversim.hpp
 *   Rivers Simulation
 *
 *   Here is placed different routine functions, like
 *       - forward river evolution
 *       - backward river evolution
 *       - statistics and postrocessing
 *       - etc..
 */

#pragma once

#include "utilities.hpp"
#include "tree.hpp"
#include "mesh.hpp"
#include "solver.hpp"
#include "physmodel.hpp"
#include "border.hpp"
#include "io.hpp"

namespace River
{
    /* 
        TODO: 
        Implementation of some standard workflows 
        like:
            - forward river evolution
            - backward river evolution
            - statistics and postrocessing
            - evaluation of parameters
            - other
    
    */


   ///One step of forward river evolution.
   void ForwardRiverEvolution(Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, Border& border, string mesh_file)
   {
        auto mesh = BoundaryGenerator(mdl, tree, border);
        
        //FIXME:
        //preparing mesh constraint function
        tria.ref->tip_points = tree.TipPoints();
        tria.generate(mesh);
        mesh.convert();
        mesh.write(mesh_file);

        //Simulation
        //Deal.II library
        sim.SetBoundaryRegionValue(mdl.GetZeroIndices(), 0.);
        sim.SetBoundaryRegionValue(mdl.GetNonZeroIndices(), 1.);
        sim.OpenMesh(mesh_file);
        sim.run(0/*FIXME*/);

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
}