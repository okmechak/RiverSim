#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/grid_out.h>

#include <boost/program_options.hpp>

#include <tetgen.h>
#include <gmsh.h>
#include "triangle.h"
#include "tethex.hpp"
#include <iostream>
#include <fstream>

using namespace dealii;
using namespace std;
using namespace gmsh;

void dealii_test();
void boost_test(int argc, char *argv[]);
void gmsh_test();
void tethex_test();
void tetgen_test();
void triangle_test();