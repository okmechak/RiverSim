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
    void Model::CheckParametersConsistency() const
    {
        River::print(prog_opt.verbose, "Input parameters check..");
        //program run parameters
        if(prog_opt.maximal_river_height < 0)
            throw invalid_argument("Invalid value of maximal_river_height = " + to_string(prog_opt.maximal_river_height) + ", it should be in range greater then 0 up to height.");
        else if (prog_opt.maximal_river_height > height)
            cout << "maximal_river_height is greater then height of region, so it does not have any effect.";

        //Model
        if(dx < 0 || dx > width)
            throw invalid_argument("Invalid value of dx = " + to_string(dx) + ", it should be in range(0, width).");
        if(width < 0)
            throw invalid_argument("width parameter can't be negative: " + to_string(width));
        if(width > 1000)
            cout << "Width value is very large: " << width << endl;
        if(height < 0)
            throw invalid_argument("height parameter can't be negative: " + to_string(height));
        if(height > 1000)
            cout << "Height value is very large: " << height << endl;
        if(boundary_ids.size() != 4)
            throw invalid_argument("boundary_ids size isn't 4, which corresponds to rectangular region: " + to_string(boundary_ids.size()));
        if(boundary_condition != 0 && boundary_condition!= 1)
            throw invalid_argument("Wrong value of boundary condition: " + to_string(boundary_condition) + ". It should be 0 or 1");
        if(biffurcation_threshold > 100 || biffurcation_threshold < -100)
            cout << "abs(biffurcation_threshold) value is very big: " << abs(biffurcation_threshold) << endl;

        if(biffurcation_threshold_2 > 100 || biffurcation_threshold_2 < -100)
            cout << "abs(biffurcation_threshold_2) value is very big: " << abs(biffurcation_threshold_2) << endl;

        if(biffurcation_min_dist < 0)
            throw invalid_argument("biffurcation_min_dist parameter can't be negative: " + to_string(biffurcation_min_dist));

        if(growth_type != 0 && growth_type!= 1)
            throw invalid_argument("Wrong value of growth_type: " + to_string(growth_type) + ". It should be 0 or 1");

        if(growth_threshold < 0)
            throw invalid_argument("growth_threshold parameter can't be negative: " + to_string(growth_threshold));
        
        if(growth_min_distance < 0)
            throw invalid_argument("growth_min_distance parameter can't be negative: " + to_string(growth_min_distance));
        
        if(ds <= 0)
            throw invalid_argument("ds parameter can't be negative or zero: " + to_string(ds));

        if(ds < 1e-7)
            cout << "ds is to small = " << ds << endl;

        if(ds > 1000)
            cout << "ds is to big = " << ds << endl;
        

        //Mesh
        if(mesh.exponant < 0)
            throw invalid_argument("mesh-exp parameter can't be negative: " + to_string(mesh.exponant)
                + "best values are in range from >0 to 200");
        
        if(mesh.exponant > 100)
            cout << "Mesh refinment function is very close to step function: " << mesh.exponant << endl;
        
        if(mesh.refinment_radius < 0)
            throw invalid_argument("mesh refinment-radius parameter can't be negative: " + to_string(mesh.refinment_radius));
        
        if(mesh.min_area < 0)
            throw invalid_argument("mesh-min-area parameter can't be negative: " + to_string(mesh.min_area));

        if(mesh.min_area < 1e-12)
            cout << "Triangle can't handle such small values for mesh-min-area: " << mesh.min_area <<
            " For smaller elemets consider using static_refinment_steps" << endl;
        if(mesh.min_area > mesh.max_area)
            cout << "mesh-min-area is greater than mesh-max-area. It is not standard case for program." << endl;
        
        if(mesh.min_angle < 0 || mesh.min_angle > 35)
            throw invalid_argument("Wrong values of mesh-min-angle it should be in range (0, 35): " + to_string(mesh.min_angle));
        
        if(mesh.max_area < 0)
            throw invalid_argument("mesh-max-area parameter can't be negative: " + to_string(mesh.min_area));

        if(mesh.eps < 0)
            throw invalid_argument("mesh eps parameter can't be negative: " + to_string(mesh.eps));

        if(mesh.eps >  0.1 * ds)
            throw invalid_argument("mesh eps should be much smaller than ds eps << ds, at least 0.1: eps" + to_string(mesh.eps) + " ds: " + to_string(ds));

        if(mesh.sigma < 0)
            throw invalid_argument("mesh sigma parameter can't be negative: " + to_string(mesh.sigma));

        if(mesh.static_refinment_steps >= 5)
            cout << "mesh static_refinment_steps parameter is very large, and simulation can take a long time: " << mesh.static_refinment_steps << endl;

        if(mesh.ratio < 1.9)
            throw invalid_argument("mesh ratio parameter can't be smaller than 1.9: " + to_string(mesh.ratio));
        
        if(mesh.max_edge < 0)
            throw invalid_argument("mesh max_edge parameter can't be negative " + to_string(mesh.max_edge));
        
        if(mesh.min_edge < 0)
            throw invalid_argument("mesh min_edge parameter can't be negative " + to_string(mesh.min_edge));

        if(mesh.max_edge < 0.001)
            cout << "mesh max_edge parameter is to small " + to_string(mesh.max_edge) << endl;

        if(mesh.max_edge < mesh.min_edge)
            throw invalid_argument("mesh min_edge is bigger than max_edge :" + to_string(mesh.min_edge) + ">" + to_string(mesh.max_edge));


        //Integration
        if(integr.weigth_func_radius < 0)
            throw invalid_argument("Integration weigth_func_radius parameter can't be negative: " + to_string(integr.weigth_func_radius));

        if(integr.integration_radius < 0)
            throw invalid_argument("Integration integration_radius parameter can't be negative: " + to_string(integr.integration_radius));

        if(integr.exponant < 0)
            throw invalid_argument("Integration exponant parameter can't be negative: " + to_string(integr.exponant));


        //Solver
        if(solver_params.quadrature_degree < 0)
            throw invalid_argument("Solver quadrature_degree parameter can't be negative: " + to_string(solver_params.quadrature_degree));

        if(solver_params.refinment_fraction < 0 || solver_params.refinment_fraction > 1)
            throw invalid_argument("Solver refinment_fraction parameter can't be negative or greater then 1.: " + to_string(solver_params.refinment_fraction));

        if(solver_params.adaptive_refinment_steps >= 5)
            cout << "solver adaptive_refinment_steps parameter is very large, and simulation can take a long time: " << solver_params.adaptive_refinment_steps << endl;

        if(solver_params.tollerance < 0)
            throw invalid_argument("Tollerance value of solver is negative: " + to_string(solver_params.tollerance));

        if(solver_params.tollerance < 1e-13)
            cout << "Tollerance value is very small: " << solver_params.tollerance << endl;
        if(solver_params.tollerance > 1e-5)
            cout << "Tollerance value is very big: " << solver_params.tollerance << endl;
        
        if(solver_params.num_of_iterrations < 2000)
            cout << "num_of_iterrations value is very small: " << solver_params.num_of_iterrations << endl;

        River::print(prog_opt.verbose, "Done.");
    }

    void Model::print() const
    {
        cout << "Model Parameters:" << endl;
        cout << "\t dx = " << dx << endl;
        cout << "\t width = " << width << endl;
        cout << "\t height = " << height << endl;
        cout << "\t river_boundary_id = " << river_boundary_id << endl;

        cout << "\t boundary_condition = " << boundary_condition << endl;
        cout << "\t field_value = " << field_value << endl;
        cout << "\t eta = " << eta << endl;
        cout << "\t biffurcation_type = " << biffurcation_type << endl;
        cout << "\t biffurcation_threshold = " << biffurcation_threshold << endl;
        cout << "\t biffurcation_threshold_2 = " << biffurcation_threshold_2 << endl;
        cout << "\t biffurcation_min_dist = " << biffurcation_min_dist << endl;
        cout << "\t biffurcation_angle = " << biffurcation_angle << endl;

        cout << "\t growth_type = " << growth_type << endl;
        cout << "\t growth_threshold = " << growth_threshold << endl;
        cout << "\t growth_min_distance = " << growth_min_distance << endl;

        cout << "\t ds = " << ds << endl;


        cout << "Mesh Parameters:" << endl;
        cout << "\t refinment_radius = " << mesh.refinment_radius << endl;
        cout << "\t exponant = " << mesh.exponant << endl;
        cout << "\t min_area = " << mesh.min_area << endl;
        cout << "\t max_area = " << mesh.max_area << endl;
        cout << "\t min_angle = " << mesh.min_angle << endl;
        cout << "\t max_edge = " << mesh.max_edge << endl;
        cout << "\t min_edge = " << mesh.min_edge << endl;
        cout << "\t ratio = " << mesh.ratio << endl;
        cout << "\t eps = " << mesh.eps << endl;
        cout << "\t sigma = " << mesh.sigma << endl;
        cout << "\t static_refinment_steps = " << mesh.static_refinment_steps << endl;

        cout << "Integration Parameters:" << endl;
        cout << "\t weigth_func_radius = " << integr.weigth_func_radius << endl;
        cout << "\t integration_radius = " << integr.integration_radius << endl;
        cout << "\t exponant = " << integr.exponant << endl;

        cout << "Solver parameters:" << endl;
        cout << "\t quadrature_degree = " << solver_params.quadrature_degree << endl;
        cout << "\t refinment_fraction = " << solver_params.refinment_fraction << endl;
        cout << "\t adaptive_refinment_steps = " << solver_params.adaptive_refinment_steps << endl;
        cout << "\t tollerance = " << solver_params.tollerance << endl;
        cout << "\t number of itteration = " << solver_params.num_of_iterrations << endl;

        cout << "Program optiosn:" << endl;
        cout << "\t verbose = " << prog_opt.verbose << endl;
        cout << "\t number_of_steps = " << prog_opt.number_of_steps << endl;
        cout << "\t maximal_river_height = " << prog_opt.maximal_river_height << endl;
    }
}