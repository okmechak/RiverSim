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

/*!
    \file riversim.hpp
    \brief Some general routines like backward, forward simulation etc.
    @{
    \details Here is placed different routine functions, like
        - forward river evolution
        - backward river evolution
        - statistics and postrocessing
        - etc..
    @}
*/ 

#pragma once

#include "tree.hpp"
#include "mesh.hpp"
#include "solver.hpp"
#include "physmodel.hpp"
#include "boundary.hpp"
#include "io.hpp"

namespace River
{
    class ForwardRiverSimulation
    {
        public:
            ForwardRiverSimulation(Model* model, Triangle* triangle, Solver* solver):
                model(model), 
                triangle(triangle),
                solver(solver)
            {}

            void euler_solver();
            
            void one_step(string output_file_name);
            double maximal_distance();
            void advanced_solver();

        protected: 

            void generate_boudaries(string file_name);
            void generate_mesh_file(string file_name);
            void solve(string mesh_file_name);
            map<t_branch_id, vector<double>> evaluate_series_parameters();
            double get_max_a1(const map<t_branch_id, vector<double>>& id_series_params);
            void grow_tree(const map<t_branch_id, vector<double>>& id_series_params, double max_a);
            
            /*! Stop condition of river growth simulation.
                Evaluates if tip of tree is close enough to border. If it is, then 
                it retuns _True_.
                \imageSize{StopConditionOfRiverGrowth.jpg, height:40%;width:40%;, }
                Example of usage in main.cpp function: 
                \snippet main.cpp StopConditionExample
                \param[in] border Boundary geometry
                \param[in] tree River tree or network
                \param[in] model Holds maximal value of y-coord, which can be reached by river
                \return  Boolean value wich states is river tree close enough to border
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
            bool growth_stop_condition()
            {
                return false;
            }

            Model* model = NULL;
            Triangle* triangle = NULL;
            Solver* solver = NULL;

            SimpleBoundary boundary;
    };

    
    tethex::Mesh TriangulateBoundaries(Model& model, Triangle& tria, const string file_name);
    
    void SolvePDE(Model& model, Solver& sim, const string file_name);
    
    map<t_branch_id, vector<double>> EvaluateSeriesParameteresOfTips(Model &model, Solver& sim);
    
    double MaximalA1Value(const map<t_branch_id, vector<double>>& ids_seriesparams_map);

    void GrowTree(Model &model, const map<t_branch_id, vector<double>>& id_series_params, double max_a);

    void ShrinkTree(Model& model, const map<t_branch_id, vector<double>>& id_series_params, double max_a);

    /*!One step of forward river evolution.*/
    void ForwardRiverEvolution(
        Model& model, Triangle& tria, Solver& sim,
        const string file_name, double max_a_backward = -1);
    
    ///This function only makes evaluation of bacward river growth based on pde solution and geometry
    ///but still it returns some data like difference betweem branches if they reached zero.
    map<t_branch_id, vector<double>> BackwardRiverEvolution(const Model& model, Solver& sim);

    ///One step of backward river evolution and its data.
    void BackwardForwardRiverEvolution(
        Model& model, Triangle& tria, Solver& sim, const string file_name);

    ///Evaluate series parameters near tip in predefined geometry. Used fot testing purposes
    void EvaluateSeriesParams(
        Model& model, Triangle& tria, Solver& sim, const string file_name);
}