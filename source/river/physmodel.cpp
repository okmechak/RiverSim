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

///\cond
#include <iostream>
///\endcond

#include "physmodel.hpp"

namespace River
{
    ostream& operator<<(ostream& write, const ProgramOptions & po)
    {
        write << "\t simulation_type = "          << po.simulation_type  << endl;
        write << "\t number_of_steps = "          << po.number_of_steps  << endl;
        write << "\t maximal_river_height = "     << po.maximal_river_height << endl;
        write << "\t number_of_backward_steps = " << po.number_of_backward_steps << endl;
        write << "\t save_vtk = "                 << po.save_vtk         << endl;
        write << "\t verbose = "                  << po.verbose          << endl;
        write << "\t output_file_name = "         << po.output_file_name << endl;
        write << "\t input_file_name = "          << po.input_file_name  << endl;
        write << "\t save each step = "              << po.save_each_step   << endl; 
        return write;
    }

    ///Prints program options structure to output stream.
    ostream& operator <<(ostream& write, const MeshParams & mp)
    {
        write << "\t tip_points:" << endl;
        for(auto &p: mp.tip_points)
            write << "\t\t" << p << endl;
        if(mp.tip_points.empty())
            write << "\t\t empty" << endl;
        write << "\t refinment_radius = " << mp.refinment_radius << endl;
        write << "\t exponant = "  << mp.exponant   << endl;
        write << "\t min_area = "  << mp.min_area   << endl;
        write << "\t max_area = "  << mp.max_area   << endl;
        write << "\t min_angle = " << mp.min_angle  << endl;
        write << "\t max_edge = "  << mp.max_edge   << endl;
        write << "\t min_edge = "  << mp.min_edge   << endl;
        write << "\t ratio = "     << mp.ratio      << endl;
        write << "\t eps = "       << mp.eps        << endl;
        write << "\t sigma = "     << mp.sigma      << endl;
        write << "\t static_refinment_steps = " << mp.static_refinment_steps << endl;
        return write;
    }

    ostream& operator <<(ostream& write, const IntegrationParams & ip)
    {
        write << "\t weigth_func_radius = " << ip.weigth_func_radius << endl;
        write << "\t integration_radius = " << ip.integration_radius << endl;
        write << "\t exponant = "           << ip.exponant           << endl;
        return write;
    }

    ostream& operator <<(ostream& write, const SolverParams & sp)
    {
        write << "\t quadrature_degree = "   << sp.quadrature_degree << endl;
        write << "\t refinment_fraction = "  << sp.refinment_fraction << endl;
        write << "\t adaptive_refinment_steps = " << sp.adaptive_refinment_steps << endl;
        write << "\t tollerance = "          << sp.tollerance << endl;
        write << "\t number of iteration = " << sp.num_of_iterrations << endl;
        return write;
    }

    ostream& operator <<(ostream& write, const Model & mdl)
    {
        write << "Program options:" << endl;
        write << mdl.prog_opt;

        write << "Mesh Parameters:" << endl;
        write << mdl.mesh;

        write << "Integration Parameters:" << endl;
        write << mdl.integr;

        write << "Solver parameters:" << endl;
        write << mdl.solver_params;

        write << "Border:" << endl;
        write << mdl.border;

        write << "Tree:" << endl;
        write << mdl.tree;

        write << "Model Parameters:" << endl;
        write << "\t dx = "                << mdl.dx << endl;
        write << "\t width = "             << mdl.width << endl;
        write << "\t height = "            << mdl.height << endl;
        write << "\t river_boundary_id = " << mdl.river_boundary_id << endl;

        write << "\t boundary_condition = " << mdl.boundary_condition << endl;
        write << "\t field_value = "       << mdl.field_value << endl;
        write << "\t eta = "               << mdl.eta << endl;
        write << "\t bifurcation_type = "  << mdl.bifurcation_type << endl;
        write << "\t bifurcation_threshold = " << mdl.bifurcation_threshold << endl;
        write << "\t bifurcation_threshold_2 = " << mdl.bifurcation_threshold_2 << endl;
        write << "\t bifurcation_min_dist = " << mdl.bifurcation_min_dist << endl;
        write << "\t bifurcation_angle = " << mdl.bifurcation_angle << endl;

        write << "\t growth_type = "      << mdl.growth_type << endl;
        write << "\t growth_threshold = " << mdl.growth_threshold << endl;
        write << "\t growth_min_distance = " << mdl.growth_min_distance << endl;

        write << "\t ds = "               << mdl.ds << endl;        

        return write;
    }

    void Model::InitializeBorderAndTree()
    {
        border.MakeRectangularWithHole();

        tree.Initialize(border.GetSourcesIdsPointsAndAngles());
    }

    void Model::CheckParametersConsistency() const
    {
        River::print(prog_opt.verbose, "Input parameters check...");
        //program run parameters
        if(prog_opt.maximal_river_height < 0)
            throw Exception("Invalid value of maximal_river_height = " + to_string(prog_opt.maximal_river_height) + ", it should be in range greater then 0 up to height.");
        else if (prog_opt.maximal_river_height > height)
            cout << "maximal_river_height is greater then height of region, so it does not have any effect." << endl;

        if(prog_opt.number_of_backward_steps > 30)
            cout << "number_of_backward_steps = " + to_string(prog_opt.number_of_backward_steps) + " parameter is very big, unpredictable behaviour may occur." << endl;
        
        if(prog_opt.output_file_name == "")
            throw Exception("empty output file name");

        //Model
        if(dx < 0 || dx > width)
            throw Exception("Invalid value of dx = " + to_string(dx) + ", it should be in range(0, width).");
        if(width < 0)
            throw Exception("width parameter can't be negative: " + to_string(width));
        if(width > 1000)
            cout << "Width value is very large: " << width << endl;
        if(height < 0)
            throw Exception("height parameter can't be negative: " + to_string(height));
        if(height > 1000)
            cout << "Height value is very large: " << height << endl;
        if(boundary_ids.size() != 4)
            throw Exception("boundary_ids size isn't 4, which corresponds to rectangular region: " + to_string(boundary_ids.size()));
        if(boundary_condition != 0 && boundary_condition!= 1)
            throw Exception("Wrong value of boundary condition: " + to_string(boundary_condition) + ". It should be 0 or 1");
        if(bifurcation_threshold > 100 || bifurcation_threshold < -100)
            cout << "abs(bifurcation_threshold) value is very big: " << abs(bifurcation_threshold) << endl;

        if(bifurcation_threshold_2 > 100 || bifurcation_threshold_2 < -100)
            cout << "abs(bifurcation_threshold_2) value is very big: " << abs(bifurcation_threshold_2) << endl;

        if(bifurcation_min_dist < 0)
            throw Exception("bifurcation_min_dist parameter can't be negative: " + to_string(bifurcation_min_dist));

        if(growth_type != 0 && growth_type!= 1)
            throw Exception("Wrong value of growth_type: " + to_string(growth_type) + ". It should be 0 or 1");

        if(growth_threshold < 0)
            throw Exception("growth_threshold parameter can't be negative: " + to_string(growth_threshold));
        
        if(growth_min_distance < 0)
            throw Exception("growth_min_distance parameter can't be negative: " + to_string(growth_min_distance));
        
        if(ds <= 0)
            throw Exception("ds parameter can't be negative or zero: " + to_string(ds));

        if(ds < 1e-7)
            cout << "ds is to small = " << ds << endl;

        if(ds > 1000)
            cout << "ds is to big = " << ds << endl;

        //Mesh
        if(mesh.exponant < 0)
            throw Exception("mesh-exp parameter can't be negative: " + to_string(mesh.exponant)
                + "best values are in range from >0 to 200");
        
        if(mesh.exponant > 100)
            cout << "Mesh refinment function is very close to step function: " << mesh.exponant << endl;
        
        if(mesh.refinment_radius < 0)
            throw Exception("mesh refinment-radius parameter can't be negative: " + to_string(mesh.refinment_radius));
        
        if(mesh.min_area < 0)
            throw Exception("mesh-min-area parameter can't be negative: " + to_string(mesh.min_area));

        if(mesh.min_area < 1e-12)
            cout << "Triangle can't handle such small values for mesh-min-area: " << mesh.min_area <<
            " For smaller elemets consider using static_refinment_steps" << endl;
        if(mesh.min_area > mesh.max_area)
            cout << "mesh-min-area is greater than mesh-max-area. It is not standard case for program." << endl;
        
        if(mesh.min_angle < 0 || mesh.min_angle > 35)
            throw Exception("Wrong values of mesh-min-angle it should be in range (0, 35): " + to_string(mesh.min_angle));
        
        if(mesh.max_area < 0)
            throw Exception("mesh-max-area parameter can't be negative: " + to_string(mesh.min_area));

        if(mesh.eps < 0)
            throw Exception("mesh eps parameter can't be negative: " + to_string(mesh.eps));

        if(mesh.eps >  0.1 * ds)
            throw Exception("mesh eps should be much smaller than ds eps << ds, at least 0.1: eps" + to_string(mesh.eps) + " ds: " + to_string(ds));

        if(mesh.sigma < 0)
            throw Exception("mesh sigma parameter can't be negative: " + to_string(mesh.sigma));

        if(mesh.static_refinment_steps >= 5)
            cout << "mesh static_refinment_steps parameter is very large, and simulation can take a long time: " << mesh.static_refinment_steps << endl;

        if(mesh.ratio < 1.9)
            throw Exception("mesh ratio parameter can't be smaller than 1.9: " + to_string(mesh.ratio));
        
        if(mesh.max_edge < 0)
            throw Exception("mesh max_edge parameter can't be negative " + to_string(mesh.max_edge));
        
        if(mesh.min_edge < 0)
            throw Exception("mesh min_edge parameter can't be negative " + to_string(mesh.min_edge));

        if(mesh.max_edge < 0.001)
            cout << "mesh max_edge parameter is to small " + to_string(mesh.max_edge) << endl;

        if(mesh.max_edge < mesh.min_edge)
            throw Exception("mesh min_edge is bigger than max_edge :" + to_string(mesh.min_edge) + ">" + to_string(mesh.max_edge));


        //Integration
        if(integr.weigth_func_radius < 0)
            throw Exception("Integration weigth_func_radius parameter can't be negative: " + to_string(integr.weigth_func_radius));

        if(integr.integration_radius < 0)
            throw Exception("Integration integration_radius parameter can't be negative: " + to_string(integr.integration_radius));

        if(integr.exponant < 0)
            throw Exception("Integration exponant parameter can't be negative: " + to_string(integr.exponant));


        //Solver
        if(solver_params.quadrature_degree < 0)
            throw Exception("Solver quadrature_degree parameter can't be negative: " + to_string(solver_params.quadrature_degree));

        if(solver_params.refinment_fraction < 0 || solver_params.refinment_fraction > 1)
            throw Exception("Solver refinment_fraction parameter can't be negative or greater then 1.: " + to_string(solver_params.refinment_fraction));

        if(solver_params.adaptive_refinment_steps >= 5)
            cout << "solver adaptive_refinment_steps parameter is very large, and simulation can take a long time: " << solver_params.adaptive_refinment_steps << endl;

        if(solver_params.tollerance < 0)
            throw Exception("Tollerance value of solver is negative: " + to_string(solver_params.tollerance));

        if(solver_params.tollerance < 1e-13)
            cout << "Tollerance value is very small: " << solver_params.tollerance << endl;
        if(solver_params.tollerance > 1e-5)
            cout << "Tollerance value is very big: " << solver_params.tollerance << endl;
        
        if(solver_params.num_of_iterrations < 2000)
            cout << "num_of_iterrations value is very small: " << solver_params.num_of_iterrations << endl;

        if(solver_params.renumbering_type > 7)
            throw Exception("There is no such type of renumbering: " + to_string(solver_params.renumbering_type));
    }
}