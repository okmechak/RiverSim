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
   //Stop condition of river growth simulation.
   bool StopConditionOfRiverGrowth(const Border& border, const Tree& tree);

   ///One step of forward river evolution.
   void ForwardRiverEvolution(
       const Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, const Border& border, const string file_name);

   ///One step of backward river evolution and its data.
   bool BackwardRiverEvolution(
       const Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, const Border& border, 
       GeometryDifference& gd, const string file_name);

   ///Evaluate series parameters near tip in predefined geometry. Used fot testing purposes
   bool EvaluateSeriesParams(
       const Model& mdl, Triangle& tria, River::Solver& sim, Tree& tree, const Border& border, 
       GeometryDifference& gd, const string file_name);
}