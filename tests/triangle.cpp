//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Triangle Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "mesh.hpp"
#include "physmodel.hpp"
#include "tree.hpp"
#include "boundary.hpp"

using namespace std;

const double eps = 1e-13;
namespace utf = boost::unit_test;
using namespace River;



// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( memory_leak_test, 
    *utf::tolerance(eps))
{
    //Model object setup
    Model mdl;

    //Boundary object setup.. Rectangular boundaries
    auto boundary_ids = mdl.boundary_ids;
    auto region_size = vector<double>{mdl.width, mdl.height};
    auto sources_x_coord = vector<double>{mdl.dx};
    auto sources_id = vector<int>{1};
    
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    
    //Tree object setup
    mdl.tree.Initialize(mdl.border.GetSourcesIdsPointsAndAngles());

    
    auto mesh = BoundaryGenerator(mdl);

    //Triangle mesh object setup
    for(unsigned long int i = 0; i < 1e4; i ++)
    {
        auto newmesh = mesh;
        Triangle tria;
        tria.Quite =  true;
        tria.generate(newmesh);
        newmesh.convert();
    }
    
    //Triangle mesh object setup
    Triangle tria;
    for(unsigned long int i = 0; i < 1e4; i ++)
    {
        auto newmesh = mesh;
        tria.Quite =  true;
        tria.generate(newmesh);
        newmesh.convert();
    }
}