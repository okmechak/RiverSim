#include "riversim.hpp"

void dealii_test()
{
    using namespace dealii;
    cout << "Dealii started" << endl;

    Triangulation<2> triangulation;
    GridGenerator::hyper_cube (triangulation);
    triangulation.refine_global (4);
    ofstream out ("grid-1.eps");
    GridOut grid_out;
    grid_out.write_eps (triangulation, out);
    cout << "Grid written to grid-1.eps" << endl;
}

void boost_test(int argc, char *argv[])
{
    std::cout << argv[0] << argc << std::endl;
    boost::program_options::options_description desc("All options");

}

void tethex_test()
{  
    tethex::Point();
}


void tetgen_test()
{
    tetgenio in, out;

    //input mesh dimmension
    in.mesh_dim = 2;

    // All indices start from 1.
    in.firstnumber = 1;

    in.numberofpoints = 4;
    in.pointlist = new REAL[in.numberofpoints * in.mesh_dim];
    in.pointlist[0]  = 0;  // node 1.
    in.pointlist[1]  = 0;
    in.pointlist[2]  = 1;  // node 2.
    in.pointlist[3]  = 0;
    in.pointlist[4]  = 2;  // node 3.
    in.pointlist[5]  = 2;
    in.pointlist[6]  = 0;  // node 4.
    in.pointlist[7]  = 1;
    
    //in.segmentconstraintlist = new REAL[4 * 2];
    
    tetgenio::facet *f = new tetgenio::facet;
    in.facetlist = f;
    f->numberofpolygons = 1;
    tetgenio::polygon *p = new tetgenio::polygon[f->numberofpolygons];
    f = in.facetlist;
    f->polygonlist = p;
    f->numberofholes = 0;
    f->holelist = NULL;
    p = f->polygonlist;
    p->numberofvertices = 4;
    p->vertexlist = new int[p->numberofvertices];
    p->vertexlist[0] = 1;
    p->vertexlist[1] = 2;
    p->vertexlist[2] = 3;
    p->vertexlist[3] = 4;
    

    


    in.numberofedges = 4;
    in.edgelist = new int[2 * 4];
    in.edgelist[0] = 1; //edge 1 
    in.edgelist[1] = 2;

    in.edgelist[2] = 2; //edge 2
    in.edgelist[3] = 3;

    in.edgelist[4] = 3; //edge 3
    in.edgelist[5] = 4;

    in.edgelist[6] = 4; //edge 4
    in.edgelist[7] = 1;


    //and only one facet
    //in.numberoffacets = 0;
    //in.facetlist = new tetgenio::facet[in.numberoffacets];
    //in.facetmarkerlist = new int[in.numberoffacets];

    // Facet 1. The leftmost facet.
    //f = in.facetlist;
    //f->numberofpolygons = 1;
    //f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
    //f->numberofholes = 0;
    //f->holelist = NULL;
    //p = &f->polygonlist[0];
    //p->numberofvertices = 4;
    //p->vertexlist = new int[p->numberofvertices];
    //p->vertexlist[0] = 1;
    //p->vertexlist[1] = 2;
    //p->vertexlist[2] = 3;
    //p->vertexlist[3] = 4;
//
    //in.facetmarkerlist[0] = -1;

    
    // Output the PLC to files 'barin.node' and 'barin.poly'.
    char ch[] = "barin";
    in.save_nodes(ch);
    in.save_poly(ch);
    in.save_edges(ch);


    // Tetrahedralize the PLC. Switches are chosen to read a PLC (p),
    //   do quality mesh generation (q) with a specified quality bound
    //   (1.414), and apply a maximum volume constraint (a0.1).
    tetgenbehavior b;
    b.plc = 1; //read plc format
    //b.quality = 1.4; //sets quality
    //b.meditview = 1;
    b.verbose = 1;
    
    tetrahedralize(&b, &in, &out);

    // Output mesh to files 'barout.node', 'barout.ele' and 'barout.face'.
    out.save_nodes(ch);
    out.save_elements(ch);
    out.save_faces(ch);
}

void triangle_test()
{
    
}

void gmsh_test()
{
    gmsh::initialize();
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::model::add("square");
    gmsh::model::geo::addPoint(0, 0, 0, 0.1, 1);
    gmsh::model::geo::addPoint(1, 0, 0, 0.1, 2);
    gmsh::model::geo::addPoint(1, 1, 0, 0.1, 3);
    gmsh::model::geo::addPoint(0, 1, 0, 0.1, 4);
    gmsh::model::geo::addPoint(0.5, 0.5, 0, 0.1, 5);

    gmsh::model::geo::addLine(1, 2, 1);
    gmsh::model::geo::addLine(2, 3, 2);
    gmsh::model::geo::addLine(3, 4, 3);
    gmsh::model::geo::addLine(4, 5, 4);

    // try automatic assignement of tag
    int line5 = gmsh::model::geo::addLine(5, 1);
    gmsh::model::geo::addCurveLoop({1, 2, 3, 4, line5}, 1);
    gmsh::model::geo::addPlaneSurface({1}, 6);
    gmsh::model::geo::synchronize();
    gmsh::model::mesh::generate(2);
    gmsh::write("square.msh");
}