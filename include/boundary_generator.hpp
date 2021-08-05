/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues
      
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

#pragma once

#include "tree.hpp"

namespace River
{
    ///Generates trees boundary
    void TreeVertices(t_PointList &tree_vector, unsigned id, const Tree& trees, double eps);
    SimpleBoundary TreeBoundary(const Tree& tree, unsigned source_id, int boundary_id, double eps);

    SimpleBoundary SimpleBoundaryGenerator(Sources &sources, Boundaries &boundaries, 
        const Tree &tree, const t_boundary_id river_boundary_id, const double mesh_eps);
}