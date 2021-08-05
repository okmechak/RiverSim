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
#include <math.h>
///\endcond

#include "boundary_generator.hpp"
#include "solver.hpp"

namespace River
{
        //ProgramOptions
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

    bool ProgramOptions::operator==(const ProgramOptions& po) const
    {
        return simulation_type == po.simulation_type 
            && number_of_steps == po.number_of_steps
            && abs(maximal_river_height - po.maximal_river_height) < EPS
            && number_of_backward_steps == po.number_of_backward_steps
            && save_vtk == po.save_vtk
            && save_each_step == po.save_each_step
            && verbose == po.verbose
            //&& output_file_name == po.output_file_name
            && input_file_name == po.input_file_name;
    }

        //SeriesParameters
    void SeriesParameters::record(const map<t_branch_id, vector<double>>& id_series_params)
    {
        for(const auto&[branch_id, series_params]: id_series_params)
        {
            if (!this->count(branch_id))
                (*this)[branch_id] = {{}, {}, {}};
            
            size_t i = 0;
            for(const auto& sp: series_params)
            {
                (*this)[branch_id].at(i).push_back(sp);
                ++i;
            }
        } 
    }

    //BackwardData
    bool BackwardData::operator==(const BackwardData& data) const
    {
        return 
            a1 == data.a1 && a2 == data.a2 && a3 == data.a3 
            && init == data.init 
            && backward == data.backward 
            && backward_forward == data.backward_forward
            && branch_lenght_diff == data.branch_lenght_diff;
    }

    ostream& operator <<(ostream& write, const BackwardData & data)
    {
        for(size_t i = 0; i < data.a1.size(); ++i)
        {
            write 
                << "a1 = " << data.a1.at(i)
                << ", a2 = " << data.a2.at(i)
                << ", a3 = " << data.a3.at(i) << endl;
            
            write 
                << "init point = " << data.init.at(i) 
                << ", backward point = " << data.backward.at(i)
                << ", backward-forward point = " << data.backward_forward.at(i) << endl;
        }
        
        write << "branch diff = " << data.branch_lenght_diff << endl;

        return write;
    }

    


    //SolverParams
    ostream& operator <<(ostream& write, const SolverParams & sp)
    {
        write << "\t quadrature_degree = "   << sp.quadrature_degree << endl;
        write << "\t refinment_fraction = "  << sp.refinment_fraction << endl;
        write << "\t adaptive_refinment_steps = " << sp.adaptive_refinment_steps << endl;
        write << "\t tollerance = "          << sp.tollerance << endl;
        write << "\t number of iteration = " << sp.num_of_iterrations << endl;
        return write;
    }

    bool SolverParams::operator==(const SolverParams& sp) const
    {
        return 
            abs(tollerance - sp.tollerance) < EPS 
            && num_of_iterrations  == sp.num_of_iterrations
            && adaptive_refinment_steps == sp.adaptive_refinment_steps
            && abs(refinment_fraction - sp.refinment_fraction) < EPS
            && quadrature_degree == sp.quadrature_degree
            && renumbering_type == sp.renumbering_type
            && abs(max_distance - sp.max_distance) < EPS;
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

        write << "\t field_value = "       << mdl.field_value << endl;
        write << "\t eta = "               << mdl.eta << endl;
        write << "\t bifurcation_type = "  << mdl.bifurcation_type << endl;
        write << "\t bifurcation_threshold = " << mdl.bifurcation_threshold << endl;
        write << "\t bifurcation_min_dist = " << mdl.bifurcation_min_dist << endl;
        write << "\t bifurcation_angle = " << mdl.bifurcation_angle << endl;

        write << "\t growth_type = "      << mdl.growth_type << endl;
        write << "\t growth_threshold = " << mdl.growth_threshold << endl;
        write << "\t growth_min_distance = " << mdl.growth_min_distance << endl;

        write << "\t ds = "               << mdl.ds << endl;        

        return write;
    }

    bool Model::operator==(const Model& model) const
    {
        return 
               abs(dx - model.dx) < EPS 
            && abs(width - model.width) < EPS 
            && abs(height - model.height) < EPS 
            && abs(field_value - model.field_value) < EPS 
            && abs(eta - model.eta) < EPS 
            && abs(bifurcation_threshold - model.bifurcation_threshold) < EPS 
            && abs(bifurcation_min_dist - model.bifurcation_min_dist) < EPS 
            && abs(bifurcation_angle - model.bifurcation_angle) < EPS 
            && abs(growth_threshold - model.growth_threshold) < EPS 
            && abs(growth_min_distance - model.growth_min_distance) < EPS 
            && abs(ds - model.ds) < EPS 
            && river_boundary_id  == model.river_boundary_id
            && bifurcation_type == model.bifurcation_type
            && growth_type == model.growth_type;
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
            
        if(bifurcation_threshold > 100 || bifurcation_threshold < -100)
            cout << "abs(bifurcation_threshold) value is very big: " << abs(bifurcation_threshold) << endl;

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

        if(mesh.ratio <= 1.38)
            throw Exception("mesh ratio parameter can't be smaller than 1.38 this corresponds to 36 degree: " + to_string(mesh.ratio));
        
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

    //Model
    bool Model::q_bifurcate(const vector<double>& a) const
    {
        if(bifurcation_type == 1)
        {
            if(prog_opt.verbose)
                cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
            return (a.at(2)/a.at(0) <= bifurcation_threshold);
        }
        else if(bifurcation_type == 2)
        {
            if(prog_opt.verbose)
                cout << "a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
            return (a.at(0) >= bifurcation_threshold);
        }
        else if(bifurcation_type == 3)
        {
            if(prog_opt.verbose)
                cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold
                     << " a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
            return a.at(2)/a.at(0) <= bifurcation_threshold && a.at(0) >= bifurcation_threshold;
        }
        else if(bifurcation_type == 0)
            return false;
        else 
            throw Exception("Wrong bifurcation_type value!");
    }

    bool Model::q_bifurcate(const vector<double>& a, double branch_lenght) const
    {
        cout << " branch_lenght = " << branch_lenght << ", bifurcation_min_dist = " << bifurcation_min_dist << endl;
        return q_bifurcate(a) && branch_lenght >= bifurcation_min_dist;
    }

    bool Model::q_growth(const vector<double>& a) const
    {
        cout << "a1 = " << a.at(0) << ", growth threshold = " << growth_threshold << endl;
        return a.at(0) >= growth_threshold;
    }

    Polar Model::next_point(const vector<double>& series_params) const
    {
        auto beta = series_params.at(1)/series_params.at(0),
            dl = ds * pow(series_params.at(0), eta);

        if(growth_type == 0)
        {
            double phi = -atan(2 * beta * sqrt(dl));
            return {dl, phi};
        }
        else if(growth_type == 1)
        {
            auto dy = 1 / pow(beta, 2) / 9 
                * ( pow( 27 / 2 * dl / pow(beta, 2) + 1, 2./3.) - 1 ),
                dx = 2*sqrt( pow(dy, 3) / pow(beta, 2) + pow(dy, 4) / pow(beta, 3));
                
            return ToPolar(Point{dx, dy}.rotate(-M_PI/2));
        }
        else
            throw Exception("Invalid value of growth_type!");
    }

    Polar Model::next_point(const vector<double>& series_params, double branch_lenght, double max_a)
    {
        auto normalized_series_params = series_params;
        normalized_series_params.at(0) /= max_a;

        auto eta_temp = eta;
        if(branch_lenght < growth_min_distance)
            eta = 0; 
        
        auto p = next_point(normalized_series_params);

        eta = eta_temp;
        
        return p;
    }

    void Model::Clear()
    {
        border.clear();
        sources.clear();
        boundary_conditions.clear();
        tree.Clear();
    }

    void Model::InitializeLaplace()
    {
        Clear();

        field_value = 0;

        border[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {dx, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            },
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {NEUMAN, 0};
        boundary_conditions[4] = {NEUMAN, 1};
        boundary_conditions[5] = {NEUMAN, 0};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        tree.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializePoisson()
    {
        Clear();

        field_value = 1;
        
        border[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {dx, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            },
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {NEUMAN, 0};
        boundary_conditions[4] = {NEUMAN, 0};
        boundary_conditions[5] = {NEUMAN, 0};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        tree.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializeDirichlet()
    {
        Clear();

        field_value = 1;

        border[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {dx, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            },
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer rectangular boudary"
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {DIRICHLET, 0};
        boundary_conditions[4] = {DIRICHLET, 0};
        boundary_conditions[5] = {DIRICHLET, 0};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        tree.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializeDirichletWithHole()
    {
        Clear();

        field_value = 1;

        border[1] = 
        {/*Outer Boundary*/
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {dx, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            }, 
            false/*this is not inner boudary*/,
            {}/*hole*/,
            "outer boudary"/*just name*/
        };
        sources[1] = {1, 1};
        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {NEUMAN, 0};
        boundary_conditions[4] = {NEUMAN, 1};
        boundary_conditions[5] = {NEUMAN, 0};

        border[2] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.25*width, 0.75*height},
                {0.75*width, 0.75*height}, 
                {0.75*width, 0.25*height}, 
                {0.25*width, 0.25*height}
            }, 
            {/*lines*/
                {0, 1, 6},
                {1, 2, 7},
                {2, 3, 8},
                {3, 0, 9} 
            }, 
            true/*this is inner boudary*/,
            {{0.5*width, 0.5*height}}/*holes*/,
            "hole"/*just name*/
        };
        sources[2] = {2, 0};
        boundary_conditions[6] = {DIRICHLET, 0};
        boundary_conditions[7] = {DIRICHLET, 0};
        boundary_conditions[8] = {DIRICHLET, 0};
        boundary_conditions[9] = {DIRICHLET, 0};

        border[3] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.8*width, 0.9*height},
                {0.9*width, 0.9*height}, 
                {0.9*width, 0.8*height}, 
                {0.8*width, 0.8*height}
            }, 
            {/*lines*/
                {0, 1, 10},
                {1, 2, 11},
                {2, 3, 12},
                {3, 0, 13} 
            }, 
            true/*this is inner boudary*/,
            {{0.85*width, 0.85*height}}/*holes*/,
            "hole"/*just name*/
        };
        sources[3] = {3, 3};
        boundary_conditions[10] = {DIRICHLET, 1};
        boundary_conditions[11] = {DIRICHLET, 1};
        boundary_conditions[12] = {DIRICHLET, 1};
        boundary_conditions[13] = {DIRICHLET, 1};

        boundary_conditions[river_boundary_id] = {DIRICHLET, 0}; 

        tree.Initialize(border.GetSourcesIdsPointsAndAngles(sources));
    }

    void Model::RevertLastSimulationStep()
    {
        auto tips_ids = tree.TipIdsAndPoints();

        tree.remove_tip_points();

        for(auto& [key, vector_value]: sim_data)
            vector_value.pop_back();

        for(auto& [branch_id, vector_value]: series_parameters)
            if(tips_ids.count(branch_id))
                vector_value.pop_back();

        //\todo how about backward simulation?
    }

    /*
        RiverSimulation
    */
    SimpleBoundary RiverSimulation::generate_boudaries(string file_name)
    {
        auto boundary = SimpleBoundaryGenerator(*model);
        if(boundary.vertices.empty())
            throw Exception("TriangulateBoundaries: boundary is empty");
        
        return boundary;
    }

    void RiverSimulation::generate_mesh_file(SimpleBoundary& boundary, string file_name)
    {
        auto mesh = tethex::Mesh(boundary);
        if(model->prog_opt.debug)
            mesh.write(file_name + "_boundary.msh");

        triangle->mesh_params->tip_points = model->tree.TipPoints();
        triangle->generate(mesh);//triangulation

        if(model->prog_opt.debug)
            mesh.write(file_name + "_triangles.msh");

        mesh.convert();//convertaion from triangles to quadrangles

        mesh.write(file_name + ".msh");
    }

    void RiverSimulation::solve(string mesh_file_name)
    {
        solver->clear();
        solver->OpenMesh(mesh_file_name + ".msh");
        solver->static_refine_grid(*model, model->tree.TipPoints());
        solver->run();
        if (model->prog_opt.save_vtk || model->prog_opt.debug)
            solver->output_results(mesh_file_name);
    }

    map<t_branch_id, vector<double>> RiverSimulation::integrate_series_parameters()
    {
        if(!solver->solved())
            throw Exception("EvaluateSeriesParameteresOfTips: run of solver wasn't called");

        map<t_branch_id, vector<double>> id_series_params;
        for(const auto id: model->tree.TipBranchesIds())
        {
            auto tip_point = model->tree.at(id).TipPoint();
            auto tip_angle = model->tree.at(id).TipAngle();
            id_series_params[id] = solver->integrate(*model, tip_point, tip_angle);
        }

        return id_series_params;
    }

    map<t_branch_id, vector<double>> RiverSimulation::solve_and_evaluate_series_parameters(string output_file_name)
    {
        print(model->prog_opt.verbose, "Boundary generation...");
        auto boundary = generate_boudaries(output_file_name);
        
        print(model->prog_opt.verbose, "Mesh generation...");
        generate_mesh_file(boundary, output_file_name);
        
        print(model->prog_opt.verbose, "Solving...");
        solve(output_file_name);

        print(model->prog_opt.verbose, "Series parameters integration...");
        auto id_series_params = integrate_series_parameters();

        return id_series_params;
    }

    double RiverSimulation::get_max_a1(const map<t_branch_id, vector<double>>& id_series_params)
    {
        double max_a = 0.;
        for(const auto&[id, series_params]: id_series_params)
            if(max_a < series_params.at(0))
                max_a = series_params.at(0);

        return max_a;
    }

    void RiverSimulation::grow_tree(const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        auto& tree = model->tree;
        if(tree.empty())
            throw Exception("GrowTree: tree is empty");

        for(const auto&[id, series_params]: id_series_params)
            if(model->q_growth(series_params))
            {
                if(model->q_bifurcate(series_params, tree.at(id).Lenght()))
                {
                    auto tip_point = tree.at(id).TipPoint();
                    auto tip_angle = tree.at(id).TipAngle();
                    auto br_left = Branch(tip_point, tip_angle + model->bifurcation_angle);
                    br_left.AddPoint(Polar{model->ds, 0});
                    auto br_right = Branch(tip_point, tip_angle - model->bifurcation_angle);
                    br_right.AddPoint(Polar{model->ds, 0});
                    tree.AddSubBranches(id, br_left, br_right);
                }
                else
                    tree.at(id).AddPoint(
                        model->next_point(series_params, tree.at(id).Lenght(), max_a));
            }
    }

    void RiverSimulation::shrink_tree(const map<t_branch_id, vector<double>>& id_series_params, double max_a)
    {
        if(model->tree.empty())
            throw Exception("ShrinkTree: tree is empty");

        for(const auto&[id, series_params]: id_series_params)
            if(model->q_growth(series_params))
                model->tree.at(id).
                    Shrink(
                        model->next_point(
                            series_params, 
                            100/*we are not constraining here speed growth near 
                            biffuraction points, so we set some value greater than its limit*/, 
                            max_a).r);
    }

    map<t_branch_id, vector<double>> RiverSimulation::one_linear_step(
        string output_file_name, double backwardforward_max_a1)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);
        
        print(model->prog_opt.verbose, "Tree growth...");
        auto max_a1 = get_max_a1(id_series_params);
        //this functionality is in bacward forward growth, where we should use presaved a1 max parameters from backward evolution
        if(backwardforward_max_a1 > 0)
            max_a1 = backwardforward_max_a1;

        grow_tree(id_series_params, max_a1);

        return id_series_params;
    }

    void RiverSimulation::one_non_linear_step(string output_file_name, double max_a1_step1, double max_a1_step2)
    {              
        one_linear_step(output_file_name + "_first_half_step", max_a1_step1);
        
        if( model->prog_opt.debug)
            Save(*model, output_file_name + "_first_half_step");

        for(unsigned step = 0; true; ++step)
        {                                   
            print(model->prog_opt.verbose, "non-euler solver cycle----------#" + to_string(step) + "--------------------------------");
            
            auto bif_type = model->bifurcation_type;
            model->bifurcation_type = 0;//no biffurcation

            one_linear_step(output_file_name + "_second_half_step_" + to_string(step), max_a1_step2);

            model->bifurcation_type = bif_type;

            if (model->prog_opt.debug)
                Save(*model, output_file_name + "_second_half_step_" + to_string(step));
                    
            if(model->tree.maximal_tip_curvature_distance() >= model->solver_params.max_distance)
            {
                model->tree.flatten_tip_curvature();
                model->tree.remove_tip_points();
            }
            else
            {
                model->RevertLastSimulationStep();
                break;
            }
        }
    }

    map<t_branch_id, vector<double>> RiverSimulation::one_shrink_step(string output_file_name)
    {
        auto id_series_params = solve_and_evaluate_series_parameters(output_file_name);
        
        print(model->prog_opt.verbose, "Shrinking of trees...");
        auto max_a1 = get_max_a1(id_series_params);
        shrink_tree(id_series_params, max_a1);

        print(model->prog_opt.verbose, "Handle branches with zero lenght(if they are) and collect data...");
        for(auto id: model->tree.zero_lenght_tip_branches_ids(0.01 * model->ds))
        {
            const auto& [branch_left, branch_right] = model->tree.GetSubBranches(id);
            auto biff_diff = abs(branch_left.Lenght() - branch_right.Lenght());
            model->backward_data[id].branch_lenght_diff = biff_diff;
            model->tree.DeleteSubBranches(id);
        }

        return id_series_params;
    }

    void RiverSimulation::one_backward_step(string output_file_name)
    {
        //save initial tree
        auto initial_tree = model->tree;
        map<t_branch_id, vector<double>> tip_id_series_params;
        vector<double> presaved_max_a1_parameters;

        for(unsigned step = 0; step < model->prog_opt.number_of_backward_steps; step++)
        {
            print(model->prog_opt.verbose, "backward cycle--------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_backward_" + to_string(step);

            auto id_series_params = one_shrink_step(output_file_name);

            presaved_max_a1_parameters.push_back(get_max_a1(id_series_params));
            
            if (tip_id_series_params.empty())
                tip_id_series_params = id_series_params;

            if (model->prog_opt.debug)
                Save(*model, output_file_name);
        }

        model->SaveCurrentTree();
        auto temp_bif = model->bifurcation_type;
        model->bifurcation_type = 0;//no biffuraction

        for(unsigned step = 0; step < model->prog_opt.number_of_backward_steps; step++)
        {
            print(model->prog_opt.verbose, "backward forward cycle------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_backwardforward_" + to_string(step);
            
            double 
                max_a1_step1 = presaved_max_a1_parameters.at(step), 
                max_a1_step2 = -1;

            if (step < model->prog_opt.number_of_backward_steps - 1)
                max_a1_step2 = presaved_max_a1_parameters.at(step + 1);

            one_non_linear_step(output_file_name, max_a1_step1, max_a1_step2);

            if (model->prog_opt.debug)
                Save(*model, output_file_name);
        }

        auto backwardforward_tree = model->tree;
        model->RestoreTree();
        model->bifurcation_type = temp_bif;

        //collect data
        collect_backward_data(initial_tree, backwardforward_tree, tip_id_series_params);
    }

    void RiverSimulation::collect_backward_data(Tree& init, Tree& forwrdbackward, map<t_branch_id, vector<double>>& tip_id_series_params)
    {
        for(const auto&[id, series]: tip_id_series_params)
        {
            model->backward_data[id].a1.push_back(series.at(0));
            model->backward_data[id].a2.push_back(series.at(1));
            model->backward_data[id].a3.push_back(series.at(2));
        }
        
        auto tip_points_init = init.TipIdsAndPoints(),
            tip_points_final = forwrdbackward.TipIdsAndPoints(),
            tip_poins_cur = model->tree.TipIdsAndPoints();

        for(const auto&[id, point]: tip_points_init)
            if (tip_points_final.count(id) && tip_poins_cur.count(id))
            {
                model->backward_data[id].init.push_back(point);
                model->backward_data[id].backward.push_back(tip_poins_cur.at(id));
                model->backward_data[id].backward_forward.push_back(tip_points_final.at(id));
            }
    }

    void RiverSimulation::linear_solver()
    {   
        for(unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {
            if (growth_stop_condition())
                break;
            
            print(model->prog_opt.verbose, "----------------------------------------#" + to_string(step) + "----------------------------------------");

            string output_file_name = model->prog_opt.output_file_name;
            if(model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            auto id_series_params = one_linear_step(output_file_name);
            
            model->series_parameters.record(id_series_params);
            model->sim_data["MeshSize"].push_back(solver->NumberOfRefinedCells());
            model->sim_data["DegreeOfFreedom"].push_back(solver->NumberOfDOFs());

            Save(*model, output_file_name);
        }
    }
    
    void RiverSimulation::non_linear_solver()
    {
        for (unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {                                   
            print(model->prog_opt.verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if (model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            one_non_linear_step(output_file_name);
            Save(*model, output_file_name);
        }
    }

    void RiverSimulation::backward_solver()
    {
        for (unsigned step = 0; step < model->prog_opt.number_of_steps; ++step)
        {                                   
            print(model->prog_opt.verbose, "global cycle----------------------------#" + to_string(step) + "----------------------------------------");
            
            string output_file_name = model->prog_opt.output_file_name;
            if (model->prog_opt.save_each_step || model->prog_opt.debug)
                output_file_name += "_" + to_string(step);

            one_backward_step(output_file_name);
            Save(*model, output_file_name);
        }

    }
}