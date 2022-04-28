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

/*! \file model.hpp
    \brief Contains all RiverSim program parameters.
    \details RiverSim - is big program with a lot of parameters.
    Program handles geometry, boudary conditions, mesh generation, FEM solvers etc. And each this module has
    a lot of parameters. 

    These parameters can be specified by JSON file(\ref io.hpp) or through program options(\ref River::process_program_options).
 */
#pragma once

#include "io.hpp"

using namespace std;

namespace River
{

    /*! \brief Physical model parameters.
        \details 
        Holds parameters related to model. Region, numerical preocessing, parameters of growth etc.
    */
    class RiverSolver
    {   
        public:
            RiverSolver(const cxxopts::ParseResult po):
            model{getModel(po)},
            prog_opt{getProgramOptions(po)},
            verbose{prog_opt.verbose},
            triangle{model.mesh_params},
            solver{model.solver_params, prog_opt.verbose}
            {
                if (model.region.empty())
                    model.InitializeLaplace();

                for(auto& source_id: model.sources.GetSourcesIds())
                    model.rivers.at(source_id).AddPoint(Polar{0.1, 0}, model.river_boundary_id);
            };

            RiverSolver(const Model model_in, ProgramOptions prog_opt): 
                model{model_in}, 
                prog_opt{prog_opt},
                verbose{prog_opt.verbose},
                triangle{model.mesh_params},
                solver{model.solver_params, prog_opt.verbose}  
            {
                if (model.region.empty())
                    model.InitializeLaplace();

                for(auto& source_id: model.sources.GetSourcesIds())
                    model.rivers.at(source_id).AddPoint(Polar{0.1, 0}, model.river_boundary_id);
            };

            int run();

            void linearSolver();
            void nonLinearSolver();
            void backwardSolver();

            map<t_branch_id, vector<double>> linearStep(
                string output_file_name, double backwardforward_max_a1 = -1.);
            void nonLinearStep(string output_file_name, double max_a1_step1 = -1, double max_a1_step2 = -1);
            map<t_branch_id, vector<double>> shrinkStep(string output_file_name);
            void backwardStep();
            
            map<t_branch_id, vector<double>> solve_and_evaluate_series_parameters(string output_file_name);
            double get_max_a1(const map<t_branch_id, vector<double>>& id_series_params);

            
            /*! Stop condition of river growth simulation.
                Evaluates if tip of river is close enough to border. If it is, then 
                it retuns _True_.
                \imageSize{StopConditionOfRiverGrowth.jpg, height:40%;width:40%;, }
                Example of usage in riversim.cpp function: 
                \param[in] border Boundary geometry
                \param[in] Rivers network
                \param[in] model Holds maximal value of y-coord, which can be reached by river
                \return  Boolean value wich states is river close enough to border
                \exception None
                \pre No postcinditions
                \post No postcinditions
                \remark Some remark goes here
                \note Just example of node tag
                \bug works only for rectangular region and do not consider bottom line
                \warning   Improper use can crash your application
                \todo generalize to any boundary shape.
                \todo in qudrangular shape crossing of bottom line isn't handled.
            */
            bool growth_stop_condition() { return false; }

            Model model;
            
            ProgramOptions prog_opt;

            bool verbose;

            Triangle triangle;
            
            Solver solver;
    };
}