#include <iostream>
#include <fstream>
#include <cmath>

//most fundamental class in library
#include <deal.II/grid/tria.h>

//needed for loops over cells/faces
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

//standard grid functionality generators
#include <deal.II/grid/grid_generator.h>

//some mesh manifold description(for faces and cell which are not straight lines)
#include <deal.II/grid/manifold_lib.h>

//output grid in different formats
#include <deal.II/grid/grid_out.h>

#include "river_sim.hpp"


using namespace std;
using namespace dealii;

void second_grid(){
    Triangulation<2> triangulation;
    const Point<2> center(1, 0);
    const double inner_rad = 0.5,
                 outer_rad = 1.0;
    
    GridGenerator::hyper_shell (triangulation, 
                center, inner_rad, outer_rad, 10);

    //manifold description
    const SphericalManifold<2> manifold_description(center);
    triangulation.set_manifold(0, manifold_description);
    triangulation.set_all_manifold_ids(0);

    for(unsigned int step = 0; step < 5; ++step){
        for(auto cell: triangulation.active_cell_iterators())
             for(unsigned int v = 0; v < GeometryInfo<2>::vertices_per_cell; ++v){
                 const double dist_from_center = center.distance(cell->vertex(v));
                 if(fabs(dist_from_center - inner_rad) < 1e-10){
                    cell->set_refine_flag();
                    break;
                 }
             }  
        triangulation.execute_coarsening_and_refinement();
    }

    //saving result
    ofstream out("grid_2.eps");
    GridOut grid_out;
    grid_out.write_eps(triangulation, out);

    cout << "grid saving done" << endl;
}

int main(int argc, char const **argv)
{
    cout << "river simulation" << endl;    

    second_grid();

    return 0;
}
