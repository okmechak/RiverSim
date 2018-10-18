#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_tools.h>
#include <boost/program_options.hpp>
#include "tethex.hpp"
#include <tetgen.h>
#include <iostream>
#include <fstream>

using namespace dealii;
using namespace std;

void dealii_test();
void boost_test(int argc, char *argv[]);
void gmsh_test();
void tethex_test();
void tetgen_test();