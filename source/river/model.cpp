///\cond
#include <iostream>
#include <random>
///\endcond

#include "model.hpp"

namespace River
{
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
            return (a.at(2)/a.at(0) <= bifurcation_threshold);
        else if(bifurcation_type == 2)
            return (a.at(0) >= bifurcation_threshold);
        else if(bifurcation_type == 3)
            return a.at(2)/a.at(0) <= bifurcation_threshold && a.at(0) >= bifurcation_threshold;
        else if(bifurcation_type == 4)
        {
            auto r = ((double) rand() / (RAND_MAX)) + 1;
            return a.at(0) * r >= bifurcation_threshold;
        }
        else if(bifurcation_type == 0)
            return false;
        else 
            throw Exception("Wrong bifurcation_type value!");
    }

    bool Model::q_bifurcate(const vector<double>& a, double branch_lenght) const
    {
        return q_bifurcate(a) && branch_lenght >= bifurcation_min_dist;
    }

    bool Model::q_growth(const vector<double>& a) const
    {
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
            auto dy = pow(beta, -2) / 9 * ( pow( 27 / 2 * dl / pow(beta, -2) + 1, 2./3.) - 1 ),
                dx = 2 * sqrt( pow(dy, 3) / pow(beta, -2) + pow(dy, 4) / pow(beta, -3));
                
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

    void Model::clear()
    {
        region.clear();
        sources.clear();
        boundary_conditions.clear();
        rivers.Clear();
    }
}