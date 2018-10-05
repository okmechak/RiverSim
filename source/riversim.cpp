#include "riversim.hpp"

void dealii_test()
{
    dealii::Triangulation<2> tria;
}

void boost_test()
{
    boost::program_options::options_description desc("All options");

}

void gmsh_test()
{
    gmsh::initialize();
    gmsh::finalize();
}