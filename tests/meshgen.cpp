//Link to Boost
#define BOOST_TEST_DYN_LINK

//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "BoundaryGenerator Functions"

//VERY IMPORTANT - include this last
#include <boost/test/unit_test.hpp>

#include <math.h>
#include "physmodel.hpp"

using namespace River;

const double eps = 1e-15;
namespace utf = boost::unit_test;

// ------------- Tests Follow --------------
BOOST_AUTO_TEST_CASE( BoundaryGenerator_test, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.01;
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.5, 0.6, 0.7, 0.8};
    auto sources_id = vector<int>{1, 2, 3, 4};
    
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);
    
    mdl.tree.Initialize(mdl.border.GetSourcesPoint(), mdl.border.GetSourcesNormalAngle(), mdl.border.GetSourcesId());

    BOOST_TEST(mdl.border.GetSourcesId() == mdl.tree.SourceBranchesID());
    
    BOOST_TEST_MESSAGE( "Boundary Generator start" );
    auto mesh = BoundaryGenerator(mdl);
    BOOST_TEST_MESSAGE( "Boundary Generator done" );


    BOOST_TEST(mesh.get_n_vertices() == 8);
    //Vertices test
    //1
    BOOST_TEST(mesh.get_vertex(0).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(0).get_coord(1) == 0.);
    //2
    BOOST_TEST(mesh.get_vertex(1).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(1).get_coord(1) == 1.);
    //3
    BOOST_TEST(mesh.get_vertex(2).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(2).get_coord(1) == 1.);
    //4
    BOOST_TEST(mesh.get_vertex(3).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(3).get_coord(1) == 0.);
    //5
    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    //6
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.6);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 0.);
    //7
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.7);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 0.);
    //8
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 0.);

    //line lements
    BOOST_TEST(mesh.get_n_lines() == 8);
    //1
    BOOST_TEST(mesh.get_line(0).get_vertex(0) == 0);
    BOOST_TEST(mesh.get_line(0).get_vertex(1) == 1);
    //2
    BOOST_TEST(mesh.get_line(1).get_vertex(0) == 1);
    BOOST_TEST(mesh.get_line(1).get_vertex(1) == 2);
    //3
    BOOST_TEST(mesh.get_line(2).get_vertex(0) == 2);
    BOOST_TEST(mesh.get_line(2).get_vertex(1) == 3);
    //4
    BOOST_TEST(mesh.get_line(3).get_vertex(0) == 3);
    BOOST_TEST(mesh.get_line(3).get_vertex(1) == 4);
    //5
    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 0);
}



BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.02;
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.5, 0.6, 0.7, 0.8};
    auto sources_id = vector<int>{1, 2, 3, 4};
    
    
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    mdl.tree.Initialize(mdl.border.GetSourcesPoint(), mdl.border.GetSourcesNormalAngle(), mdl.border.GetSourcesId());
    mdl.tree.AddPolars({{1, 0}, {2, 0}, {3, 0}, {4, 0}}, sources_id);

    BOOST_TEST(mdl.border.GetSourcesId() == mdl.tree.SourceBranchesID());
    
    auto mesh = BoundaryGenerator(mdl);


    BOOST_TEST(mesh.get_n_vertices() == 16);
    //Vertices test
    //1
    BOOST_TEST(mesh.get_vertex(0).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(0).get_coord(1) == 0.);
    //2
    BOOST_TEST(mesh.get_vertex(1).get_coord(0) == 1.);
    BOOST_TEST(mesh.get_vertex(1).get_coord(1) == 1.);
    //3
    BOOST_TEST(mesh.get_vertex(2).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(2).get_coord(1) == 1.);
    //4
    BOOST_TEST(mesh.get_vertex(3).get_coord(0) == 0.);
    BOOST_TEST(mesh.get_vertex(3).get_coord(1) == 0.);
    //5
    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 0.);
    //7
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.59);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(8).get_coord(0) == 0.6);
    BOOST_TEST(mesh.get_vertex(8).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(9).get_coord(0) == 0.61);
    BOOST_TEST(mesh.get_vertex(9).get_coord(1) == 0.);
    //8
    BOOST_TEST(mesh.get_vertex(10).get_coord(0) == 0.69);
    BOOST_TEST(mesh.get_vertex(10).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(11).get_coord(0) == 0.7);
    BOOST_TEST(mesh.get_vertex(11).get_coord(1) == 3.);
    BOOST_TEST(mesh.get_vertex(12).get_coord(0) == 0.71);
    BOOST_TEST(mesh.get_vertex(12).get_coord(1) == 0.);
    //9
    BOOST_TEST(mesh.get_vertex(13).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(13).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(14).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(14).get_coord(1) == 4.);
    BOOST_TEST(mesh.get_vertex(15).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(15).get_coord(1) == 0.);

    //line lements
    BOOST_TEST(mesh.get_n_lines() == 16);
    //1
    BOOST_TEST(mesh.get_line(0).get_vertex(0) == 0);
    BOOST_TEST(mesh.get_line(0).get_vertex(1) == 1);
    BOOST_TEST(mesh.get_line(0).get_material_id() == 1);
    //2
    BOOST_TEST(mesh.get_line(1).get_vertex(0) == 1);
    BOOST_TEST(mesh.get_line(1).get_vertex(1) == 2);
    BOOST_TEST(mesh.get_line(1).get_material_id() == 2);
    //3
    BOOST_TEST(mesh.get_line(2).get_vertex(0) == 2);
    BOOST_TEST(mesh.get_line(2).get_vertex(1) == 3);
    BOOST_TEST(mesh.get_line(2).get_material_id() == 3);
    //4
    BOOST_TEST(mesh.get_line(3).get_vertex(0) == 3);
    BOOST_TEST(mesh.get_line(3).get_vertex(1) == 4);
    BOOST_TEST(mesh.get_line(3).get_material_id() == river_boundary_id);
    //5
    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    BOOST_TEST(mesh.get_line(4).get_material_id() == river_boundary_id);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    BOOST_TEST(mesh.get_line(5).get_material_id() == river_boundary_id);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    BOOST_TEST(mesh.get_line(6).get_material_id() == river_boundary_id);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 8);
    BOOST_TEST(mesh.get_line(7).get_material_id() == river_boundary_id);
    //9
    BOOST_TEST(mesh.get_line(8).get_vertex(0) == 8);
    BOOST_TEST(mesh.get_line(8).get_vertex(1) == 9);
    BOOST_TEST(mesh.get_line(8).get_material_id() == river_boundary_id);
    //10
    BOOST_TEST(mesh.get_line(9).get_vertex(0) == 9);
    BOOST_TEST(mesh.get_line(9).get_vertex(1) == 10);
    BOOST_TEST(mesh.get_line(9).get_material_id() == river_boundary_id);
    //11
    BOOST_TEST(mesh.get_line(10).get_vertex(0) == 10);
    BOOST_TEST(mesh.get_line(10).get_vertex(1) == 11);
    BOOST_TEST(mesh.get_line(10).get_material_id() == river_boundary_id);
    //12
    BOOST_TEST(mesh.get_line(11).get_vertex(0) == 11);
    BOOST_TEST(mesh.get_line(11).get_vertex(1) == 12);
    BOOST_TEST(mesh.get_line(11).get_material_id() == river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(12).get_vertex(0) == 12);
    BOOST_TEST(mesh.get_line(12).get_vertex(1) == 13);
    BOOST_TEST(mesh.get_line(12).get_material_id() == river_boundary_id);
    //14
    BOOST_TEST(mesh.get_line(13).get_vertex(0) == 13);
    BOOST_TEST(mesh.get_line(13).get_vertex(1) == 14);
    BOOST_TEST(mesh.get_line(13).get_material_id() == river_boundary_id);
    //15
    BOOST_TEST(mesh.get_line(14).get_vertex(0) == 14);
    BOOST_TEST(mesh.get_line(14).get_vertex(1) == 15);
    BOOST_TEST(mesh.get_line(14).get_material_id() == river_boundary_id);
    //16
    BOOST_TEST(mesh.get_line(15).get_vertex(0) == 15);
    BOOST_TEST(mesh.get_line(15).get_vertex(1) == 0);
    BOOST_TEST(mesh.get_line(15).get_material_id() == river_boundary_id);
}



BOOST_AUTO_TEST_CASE( BoundaryGenerator_test_new_new, 
    *utf::tolerance(eps))
{   
    Model mdl;
    mdl.mesh.eps = 0.02;
    auto river_boundary_id = 4;
    auto boundary_ids = vector<int>{1, 2, 3, river_boundary_id};
    auto region_size = vector<double>{1, 1};
    auto sources_x_coord = vector<double>{0.5, 0.8};
    auto sources_id = vector<int>{1, 2};
    
    mdl.border.MakeRectangular(
        region_size, 
        boundary_ids,
        sources_x_coord,
        sources_id);

    mdl.tree.Initialize(mdl.border.GetSourcesPoint(), mdl.border.GetSourcesNormalAngle(), mdl.border.GetSourcesId());
    mdl.tree.AddPolars({{1, 0}, {2, 0}}, sources_id);
    mdl.tree.AddPolars({{1, 0}, {2, 0}}, sources_id);

    BOOST_TEST(mdl.border.GetSourcesId() == mdl.tree.SourceBranchesID());
    
    auto mesh = BoundaryGenerator(mdl);

    BOOST_TEST(mesh.get_n_lines() == 14);
    BOOST_TEST(mesh.get_n_vertices() == 14);

    BOOST_TEST(mesh.get_vertex(4).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5).get_coord(0) == 0.49);
    BOOST_TEST(mesh.get_vertex(5).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(6).get_coord(0) == 0.5);
    BOOST_TEST(mesh.get_vertex(6).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(7).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(7).get_coord(1) == 1.);
    BOOST_TEST(mesh.get_vertex(8).get_coord(0) == 0.51);
    BOOST_TEST(mesh.get_vertex(8).get_coord(1) == 0.);

    BOOST_TEST(mesh.get_vertex(5+4).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(5+4).get_coord(1) == 0.);
    BOOST_TEST(mesh.get_vertex(5+5).get_coord(0) == 0.79);
    BOOST_TEST(mesh.get_vertex(5+5).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(5+6).get_coord(0) == 0.8);
    BOOST_TEST(mesh.get_vertex(5+6).get_coord(1) == 4.);
    BOOST_TEST(mesh.get_vertex(5+7).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(5+7).get_coord(1) == 2.);
    BOOST_TEST(mesh.get_vertex(5+8).get_coord(0) == 0.81);
    BOOST_TEST(mesh.get_vertex(5+8).get_coord(1) == 0.);


    BOOST_TEST(mesh.get_line(4).get_vertex(0) == 4);
    BOOST_TEST(mesh.get_line(4).get_vertex(1) == 5);
    BOOST_TEST(mesh.get_line(4).get_material_id() == river_boundary_id);
    //6
    BOOST_TEST(mesh.get_line(5).get_vertex(0) == 5);
    BOOST_TEST(mesh.get_line(5).get_vertex(1) == 6);
    BOOST_TEST(mesh.get_line(5).get_material_id() == river_boundary_id);
    //7
    BOOST_TEST(mesh.get_line(6).get_vertex(0) == 6);
    BOOST_TEST(mesh.get_line(6).get_vertex(1) == 7);
    BOOST_TEST(mesh.get_line(6).get_material_id() == river_boundary_id);
    //8
    BOOST_TEST(mesh.get_line(7).get_vertex(0) == 7);
    BOOST_TEST(mesh.get_line(7).get_vertex(1) == 8);
    BOOST_TEST(mesh.get_line(7).get_material_id() == river_boundary_id);
    //9
    BOOST_TEST(mesh.get_line(8).get_vertex(0) == 8);
    BOOST_TEST(mesh.get_line(8).get_vertex(1) == 9);
    BOOST_TEST(mesh.get_line(8).get_material_id() == river_boundary_id);
    //10
    BOOST_TEST(mesh.get_line(9).get_vertex(0) == 9);
    BOOST_TEST(mesh.get_line(9).get_vertex(1) == 10);
    BOOST_TEST(mesh.get_line(9).get_material_id() == river_boundary_id);
    //11
    BOOST_TEST(mesh.get_line(10).get_vertex(0) == 10);
    BOOST_TEST(mesh.get_line(10).get_vertex(1) == 11);
    BOOST_TEST(mesh.get_line(10).get_material_id() == river_boundary_id);
    //12
    BOOST_TEST(mesh.get_line(11).get_vertex(0) == 11);
    BOOST_TEST(mesh.get_line(11).get_vertex(1) == 12);
    BOOST_TEST(mesh.get_line(11).get_material_id() == river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(12).get_vertex(0) == 12);
    BOOST_TEST(mesh.get_line(12).get_vertex(1) == 13);
    BOOST_TEST(mesh.get_line(12).get_material_id() == river_boundary_id);
    //13
    BOOST_TEST(mesh.get_line(13).get_vertex(0) == 13);
    BOOST_TEST(mesh.get_line(13).get_vertex(1) == 0);
    BOOST_TEST(mesh.get_line(13).get_material_id() == river_boundary_id);
}