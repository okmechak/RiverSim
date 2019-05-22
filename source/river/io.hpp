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

/** @file io.hpp
 *   Contains functionality to read and write state of program(simulation data).
 *   
 *   Save/Open parameters of model, border and tree to/from file.
 */

#pragma once

#include "border.hpp"
#include "physmodel.hpp"
#include "tree.hpp"
#include "utilities.hpp"

namespace River
{
    Model& SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm, Model& mdl);
    void Save(const Model& mdl, const Timing& time, const Border& border, const Tree& tr, const GeometryDifference &gd, const string file_name, const string input_file = "");
    void Open(Model& mdl, Border& border, Tree& tr, GeometryDifference &gd, string file_name);

}//namespace River