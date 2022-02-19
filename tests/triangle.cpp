//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "Triangle Class"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include "mesh.hpp"
#include "physmodel.hpp"
#include "boundary_generator.hpp"

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

    mdl.sources = mdl.border.MakeRectangular();
    
    //Rivers object setup
    mdl.tree.Initialize(mdl.border.GetSourcesIdsPointsAndAngles(mdl.sources));

    
    auto boundary = SimpleBoundaryGenerator(mdl);
    tethex::Mesh mesh(boundary);

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

//mesh constrins test
BOOST_AUTO_TEST_CASE( mesh_min_angle, 
    *utf::tolerance(eps))
{
    Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 0.5;
    mesh_params.min_area = 0.0000000001;
    mesh_params.max_edge = 100;
    mesh_params.min_edge = 0;
    mesh_params.ratio = 100;
    mesh_params.tip_points = {{0.5, 0.5}};
    mesh_params.min_angle = 30;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.1;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_min_angle.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}

BOOST_AUTO_TEST_CASE( mesh_max_area, 
    *utf::tolerance(eps))
{
    Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 0.001;
    mesh_params.min_area = 0;
    mesh_params.max_edge = 100;
    mesh_params.min_edge = 0;
    mesh_params.ratio = 100;
    mesh_params.tip_points = {};
    mesh_params.min_angle = 30;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.1;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_max_area.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}

BOOST_AUTO_TEST_CASE( mesh_min_area, 
    *utf::tolerance(eps))
{
        Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 0.5;
    mesh_params.min_area = 0.0001;
    mesh_params.max_edge = 100;
    mesh_params.min_edge = 0;
    mesh_params.ratio = 100;
    mesh_params.tip_points = {{0.5, 0.5}};
    mesh_params.min_angle = 30;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.1;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_min_area.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}

BOOST_AUTO_TEST_CASE( mesh_max_edge_lenght, 
    *utf::tolerance(eps))
{
        Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 100;
    mesh_params.min_area = 0.0000000001;
    mesh_params.max_edge = 0.1;
    mesh_params.min_edge = 0;
    mesh_params.ratio = 100;
    mesh_params.tip_points = {{0.5, 0.5}};
    mesh_params.min_angle = 1;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.1;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_max_edge_lenght.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}

BOOST_AUTO_TEST_CASE( mesh_min_edge_lenght, 
    *utf::tolerance(eps))
{
            Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 100;
    mesh_params.min_area = 0.0000000001;
    mesh_params.max_edge = 0.1;
    mesh_params.min_edge = 0.01;
    mesh_params.ratio = 100;
    mesh_params.tip_points = {{0.5, 0.5}};
    mesh_params.min_angle = 1;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.1;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_min_edge_lenght.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= 0.3*mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}

BOOST_AUTO_TEST_CASE( mesh_max_triangle_ratio, 
    *utf::tolerance(eps))
{
        Region boundaries;
    boundaries[1] = 
    {
        {
            {0, 0}, 
            {1, 0}, 
            {1, 1}, 
            {0, 1}
        },
        {
            {0, 1, 1}, 
            {1, 2, 2}, 
            {2, 3, 3}, 
            {3, 0, 4}
        }, 
        false, 
        {},
        "mesh_test"
    };

    MeshParams mesh_params;
    mesh_params.max_area = 0.05;
    mesh_params.min_area = 0.0000000001;
    mesh_params.max_edge = 100;
    mesh_params.min_edge = 0;
    mesh_params.ratio = 1.5;
    mesh_params.tip_points = {{0.5, 0.5}};
    mesh_params.min_angle = 0;
    mesh_params.exponant = 2;
    mesh_params.sigma = 10;
    mesh_params.refinment_radius = 0.2;

    Model model;
    model.mesh = mesh_params;
    model.border = boundaries;

    auto mesh_boundary = SimpleBoundaryGenerator(model);
    tethex::Mesh mesh(mesh_boundary);

    //or simply boundaries.at(1) can be used
    Triangle triangle(&model.mesh);

    triangle.generate(mesh);
    mesh.write("mesh_max_triangle_ratio.msh");
    
    auto vertices = mesh.get_vertices();
    for(auto& tria: mesh.get_triangles())
    {
        BOOST_TEST(tria->min_angle(vertices) >= mesh_params.min_angle);
        BOOST_TEST(tria->max_angle(vertices) <= 180 - 2*mesh_params.min_angle);
        BOOST_TEST(tria->measure(vertices) >= 0.1*mesh_params.min_area);
        BOOST_TEST(tria->measure(vertices) <= mesh_params.max_area);
        BOOST_TEST(tria->min_edge(vertices) >= mesh_params.min_edge);
        BOOST_TEST(tria->max_edge(vertices) <= mesh_params.max_edge);
        BOOST_TEST(tria->quality(vertices) <= mesh_params.ratio);
    }
}