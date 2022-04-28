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

#include "model.hpp"

namespace River
{
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

    //Model
    void Model::InitializeLaplace()
    {
        solver_params.field_value = 0;

        region[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},//0
                {dx, 0}, //2
                {width, 0}, //4
                {width, height}, //5
                {0, height}//6
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 1},
                {2, 3, 2},
                {3, 4, 3},
                {4, 0, 2},
            }
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {NEUMAN, 0};
        boundary_conditions[3] = {NEUMAN, 1};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        rivers.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializePoisson()
    {
        solver_params.field_value = 1;
        
        region[1] = 
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
            }
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {NEUMAN, 0};
        boundary_conditions[4] = {NEUMAN, 0};
        boundary_conditions[5] = {NEUMAN, 0};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        rivers.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializeDirichlet()
    {
        solver_params.field_value = 1;

        region[1] = 
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
            }
        };/*Outer Boundary*/

        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};

        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {DIRICHLET, 0};
        boundary_conditions[4] = {DIRICHLET, 0};
        boundary_conditions[5] = {DIRICHLET, 0};
        boundary_conditions[river_boundary_id] = {DIRICHLET, 0};
            
        rivers.Initialize({{1, {{dx, 0}, M_PI/2.}}});
    }

    void Model::InitializeDirichletWithHole()
    {
        solver_params.field_value = 1;

        region[1] = 
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
            }
        };
        sources[1] = {1, 1};
        boundary_conditions[1] = {DIRICHLET, 0};
        boundary_conditions[2] = {DIRICHLET, 0};
        boundary_conditions[3] = {NEUMAN, 0};
        boundary_conditions[4] = {NEUMAN, 1};
        boundary_conditions[5] = {NEUMAN, 0};

        region[2] =
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
            }
        };
        sources[2] = {2, 0};
        boundary_conditions[6] = {DIRICHLET, 0};
        boundary_conditions[7] = {DIRICHLET, 0};
        boundary_conditions[8] = {DIRICHLET, 0};
        boundary_conditions[9] = {DIRICHLET, 0};

        region[3] =
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
            }
        };
        sources[3] = {3, 3};
        boundary_conditions[10] = {DIRICHLET, 1};
        boundary_conditions[11] = {DIRICHLET, 1};
        boundary_conditions[12] = {DIRICHLET, 1};
        boundary_conditions[13] = {DIRICHLET, 1};

        boundary_conditions[river_boundary_id] = {DIRICHLET, 0}; 

        rivers.Initialize(region.GetSourcesIdsPointsAndAngles(sources));
    }

    bool Model::q_bifurcate(const vector<double>& a) const
    {
        if(bifurcation_type == 1)
        {
            if(verbose)
                cout << "a3/a1 = " <<  a.at(2)/a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
            return (a.at(2)/a.at(0) <= bifurcation_threshold);
        }
        else if(bifurcation_type == 2)
        {
            if(verbose)
                cout << "a1 = " <<  a.at(0) << ", bif thr = " << bifurcation_threshold << endl;
            return (a.at(0) >= bifurcation_threshold);
        }
        else if(bifurcation_type == 3)
        {
            if(verbose)
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

    ostream& operator <<(ostream& write, const Model & mdl)
    {
        write << "Mesh Parameters:" << endl;
        write << mdl.mesh_params;

        write << "Integration Parameters:" << endl;
        write << mdl.integr;

        write << "Solver parameters:" << endl;
        write << mdl.solver_params;

        write << "Region:" << endl;
        write << mdl.region;

        write << "Rivers:" << endl;
        write << mdl.rivers;

        write << "Model Parameters:" << endl;
        write << "\t dx = "                << mdl.dx << endl;
        write << "\t width = "             << mdl.width << endl;
        write << "\t height = "            << mdl.height << endl;
        write << "\t river_boundary_id = " << mdl.river_boundary_id << endl;
        write << "\t river_width = "       << mdl.river_width << endl;

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
        return simulation_type == model.simulation_type 
            && number_of_steps == model.number_of_steps
            && abs(maximal_river_height - model.maximal_river_height) < EPS
            && number_of_backward_steps == model.number_of_backward_steps
            &&   abs(dx - model.dx) < EPS 
            && abs(width - model.width) < EPS 
            && abs(height - model.height) < EPS 
            && abs(eta - model.eta) < EPS 
            && abs(bifurcation_threshold - model.bifurcation_threshold) < EPS 
            && abs(bifurcation_min_dist - model.bifurcation_min_dist) < EPS 
            && abs(bifurcation_angle - model.bifurcation_angle) < EPS 
            && abs(growth_threshold - model.growth_threshold) < EPS 
            && abs(growth_min_distance - model.growth_min_distance) < EPS 
            && abs(ds - model.ds) < EPS 
            && abs(river_width - model.river_width) < EPS 
            && river_boundary_id  == model.river_boundary_id
            && bifurcation_type == model.bifurcation_type
            && growth_type == model.growth_type;
    }

    void Model::CheckParametersConsistency() const
    {
        //program run parameters
        if(maximal_river_height < 0)
            throw Exception("Invalid value of maximal_river_height = " + to_string(maximal_river_height) + ", it should be in range greater then 0 up to height.");
        else if (maximal_river_height > height)
            cout << "maximal_river_height is greater then height of region, so it does not have any effect." << endl;

        if(number_of_backward_steps > 30)
            cout << "number_of_backward_steps = " + to_string(number_of_backward_steps) + " parameter is very big, unpredictable behaviour may occur." << endl;

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
        
        if(river_width < 0)
            throw Exception("mesh river_width parameter can't be negative: " + to_string(river_width));

        if(river_width >  0.1 * ds)
            throw Exception("mesh river_width should be much smaller than ds river_width << ds, at least 0.1: river_width" + to_string(river_width) + " ds: " + to_string(ds));
            
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
        if(mesh_params.exponant < 0)
            throw Exception("mesh-exp parameter can't be negative: " + to_string(mesh_params.exponant)
                + "best values are in range from >0 to 200");
        
        if(mesh_params.exponant > 100)
            cout << "Mesh refinment function is very close to step function: " << mesh_params.exponant << endl;
        
        if(mesh_params.refinment_radius < 0)
            throw Exception("mesh refinment-radius parameter can't be negative: " + to_string(mesh_params.refinment_radius));
        
        if(mesh_params.min_area < 0)
            throw Exception("mesh-min-area parameter can't be negative: " + to_string(mesh_params.min_area));

        if(mesh_params.min_area < 1e-12)
            cout << "Triangle can't handle such small values for mesh-min-area: " << mesh_params.min_area <<
            " For smaller elemets consider using static_refinment_steps" << endl;
        if(mesh_params.min_area > mesh_params.max_area)
            cout << "mesh-min-area is greater than mesh-max-area. It is not standard case for program." << endl;
        
        if(mesh_params.min_angle < 0 || mesh_params.min_angle > 35)
            throw Exception("Wrong values of mesh-min-angle it should be in range (0, 35): " + to_string(mesh_params.min_angle));
        
        if(mesh_params.max_area < 0)
            throw Exception("mesh-max-area parameter can't be negative: " + to_string(mesh_params.min_area));

        if(mesh_params.sigma < 0)
            throw Exception("mesh sigma parameter can't be negative: " + to_string(mesh_params.sigma));

        if(mesh_params.ratio <= 1.38)
            throw Exception("mesh ratio parameter can't be smaller than 1.38 this corresponds to 36 degree: " + to_string(mesh_params.ratio));
        
        if(mesh_params.max_edge < 0)
            throw Exception("mesh max_edge parameter can't be negative " + to_string(mesh_params.max_edge));
        
        if(mesh_params.min_edge < 0)
            throw Exception("mesh min_edge parameter can't be negative " + to_string(mesh_params.min_edge));

        if(mesh_params.max_edge < 0.001)
            cout << "mesh max_edge parameter is to small " + to_string(mesh_params.max_edge) << endl;

        if(mesh_params.max_edge < mesh_params.min_edge)
            throw Exception("mesh min_edge is bigger than max_edge :" + to_string(mesh_params.min_edge) + ">" + to_string(mesh_params.max_edge));


        //Integration
        if(integr.weigth_func_radius < 0)
            throw Exception("Integration weigth_func_radius parameter can't be negative: " + to_string(integr.weigth_func_radius));

        if(integr.integration_radius < 0)
            throw Exception("Integration integration_radius parameter can't be negative: " + to_string(integr.integration_radius));

        if(integr.exponant < 0)
            throw Exception("Integration exponant parameter can't be negative: " + to_string(integr.exponant));


        //Solver
        if(solver_params.refinment_fraction < 0 || solver_params.refinment_fraction > 1)
            throw Exception("Solver refinment_fraction parameter can't be negative or greater then 1.: " + to_string(solver_params.refinment_fraction));

        if(solver_params.adaptive_refinment_steps >= 5)
            cout << "Solver adaptive_refinment_steps parameter is very large, and simulation can take a long time: " << solver_params.adaptive_refinment_steps << endl;

        if(solver_params.static_refinment_steps >= 5)
            cout << "Solver static_refinment_steps parameter is very large, and simulation can take a long time: " << solver_params.static_refinment_steps << endl;

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

    void Model::clear()
    {
        region.clear();
        sources.clear();
        boundary_conditions.clear();
        rivers.Clear();
    }

    void Model::RevertLastSimulationStep()
    {
        auto tips_ids = rivers.TipIdsAndPoints();

        rivers.remove_tip_points();

        for(auto& [key, vector_value]: sim_data)
            vector_value.pop_back();

        for(auto& [branch_id, vector_value]: series_parameters)
            if(tips_ids.count(branch_id))
                vector_value.pop_back();

        //\todo how about backward simulation?
    }

    void Model::collect_backward_data(Rivers& init, Rivers& forwrdbackward, map<t_branch_id, vector<double>>& tip_id_series_params)
    {
        for(const auto&[id, series]: tip_id_series_params)
        {
            backward_data[id].a1.push_back(series.at(0));
            backward_data[id].a2.push_back(series.at(1));
            backward_data[id].a3.push_back(series.at(2));
        }
        
        auto tip_points_init = init.TipIdsAndPoints(),
            tip_points_final = forwrdbackward.TipIdsAndPoints(),
            tip_poins_cur = rivers.TipIdsAndPoints();

        for(const auto&[id, point]: tip_points_init)
            if (tip_points_final.count(id) && tip_poins_cur.count(id))
            {
                backward_data[id].init.push_back(point);
                backward_data[id].backward.push_back(tip_poins_cur.at(id));
                backward_data[id].backward_forward.push_back(tip_points_final.at(id));
            }
    }
}