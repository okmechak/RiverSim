#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/function.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>
#include <deal.II/numerics/data_out.h>

#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <fstream>
#include <string.h>

#include <tetgen.h>
#include <gmsh.h>
#include "triangle.h"
#include "tethex.hpp"

using namespace dealii;
using namespace std;
using namespace gmsh;

namespace po = boost::program_options;
namespace mdl = gmsh::model;
namespace msh = gmsh::model::mesh;
namespace geo = gmsh::model::geo;


class Mesh
{
    public:
        Mesh();
        ~Mesh();
        void TriangleGenerator();
        void TriangleGeneratorExample();
        void Print(
            struct triangulateio &io,
            char * options,
            bool markers = true,
            bool reporttriangles = true,
            bool reportneighbors = true,
            bool reportsegments = true,
            bool reportedges = true,
            bool reportnorms = true);

    private:

};

Mesh::Mesh(){}

void Mesh::Print(
    struct triangulateio &io,
    char * options,
    bool markers,
    bool reporttriangles,
    bool reportneighbors,
    bool reportsegments,
    bool reportedges,
    bool reportnorms)
{
    int i, j;

    cout << endl << endl << "Triangle Print" << endl;
    cout << "Options: " << endl;
    if (strchr(options, 'p'))
        cout << " -p - read PSLG" << endl;
    if (strchr(options, 'z'))
        cout << " -z - numbering starts from zero" << endl;
    if (strchr(options, 'u'))
        cout << " -u - user defined constrain" << endl;
    if (strchr(options, 'r'))
        cout << " -r - refine" << endl;
    if (strchr(options, 'q'))
        cout << " -q - quality min 20 degree" << endl;
    if (strchr(options, 'a'))
        cout << " -a - maximum triangle area constrain" << endl;
    if (strchr(options, 'D'))
        cout << " -D - all traingles will be Delaunay" << endl;
    if (strchr(options, 'c'))
        cout << " -c - enclose convex hull" << endl;
    if (strchr(options, 'C'))
        cout << " -C - check final mesh" << endl;
    if (strchr(options, 'A'))
        cout << " -A - assign additional attribute to each triangle which specifies segment which it belongs too" << endl;
    if (strchr(options, 'e'))
        cout << " -e - output list of edges" << endl;

    if (strchr(options, 'v'))
        cout << " -v - outputs voronoi diagram" << endl;
    if (strchr(options, 'n'))
        cout << " -n - outputs neighboors" << endl;
    if (strchr(options, 'B'))
        cout << " -B - suppress boundary markers" << endl;
    if (strchr(options, 'P'))
        cout << " -P - suppress output poly file" << endl;
    if (strchr(options, 'N'))
        cout << " -N - suppress output nodes file" << endl;
    if (strchr(options, 'E'))
        cout << " -E - suppress output elements file" << endl;
    if (strchr(options, 'O'))
        cout << " -O - suppress holes" << endl;

    if (strchr(options, 'o'))
        cout << " -o2 - second order mesh" << endl;
    if (strchr(options, 'Y'))
        cout << " -Y - prohibits stainer points on boundary" << endl;
    if (strchr(options, 'S'))
        cout << " -S - specify max number off added Steiner points" << endl;

    if (strchr(options, 'i'))
        cout << " -i - use incremental algorithm" << endl;
    if (strchr(options, 'F'))
        cout << " -F - use Fortune algorithm" << endl;
    
    if (strchr(options, 'Q'))
        cout << " -Q - quite" << endl;
    if (strchr(options, 'V'))
        cout << " -V - verbose" << endl;



    if (io.numberofpoints > 0 && io.pointlist != NULL){
        cout << "Total of points     : " << io.numberofpoints << endl;
        cout << "      of attributes : " << io.numberofpointattributes << endl;
        if (io.pointmarkerlist == NULL) 
            cout << "Markers aren't set " << endl;

        for (i = 0; i < io.numberofpoints; i++) {
            cout << "Point " << i << ": ";

            for (j = 0; j < 2; j++)
                cout << io.pointlist[i * 2 + j] << " ";

            if (io.numberofpointattributes > 0 && io.pointattributelist != NULL){ 
                cout << "   attributes  ";
                for (j = 0; j < io.numberofpointattributes; j++) 
                    cout << io.pointattributelist[i * io.numberofpointattributes + j] << " ";
            }
            else
                cout << " point attributes aren't set" << endl;

            if (io.pointmarkerlist != NULL) 
                cout << "   marker " << io.pointmarkerlist[i];

            cout << endl;
        }        
    }
    else
        cout << "Points aren't set";

    cout << endl;

    if (reporttriangles || reportneighbors) {
        cout << "Total of triangles:  " << io.numberoftriangles << endl;
        cout << "      of attributes: " << io.numberoftriangleattributes << endl;
        cout << "      of corners:    " << io.numberofcorners << endl;
        if (io.numberoftriangles > 0 && io.trianglelist != NULL){
            cout << "Number of triangles: " << io.numberoftriangles << endl;
            for (i = 0; i < io.numberoftriangles; i++) {
                if (reporttriangles) {
                    cout << "Triangle " << i << " points: ";
                    for (j = 0; j < io.numberofcorners; j++) 
                        cout << io.trianglelist[i * io.numberofcorners + j] << " ";
                    if (io.numberoftriangleattributes > 0 && io.triangleattributelist != NULL) {
                        cout << "   attributes";
                        for (j = 0; j < io.numberoftriangleattributes; j++) 
                            cout << "  " << io.triangleattributelist[i *
                                                    io.numberoftriangleattributes + j];
                    }
                    else
                        cout << " no attributes ";

                    if (io.trianglearealist != NULL)
                        cout << " area constrain " << io.trianglearealist[i];
                    else
                        cout << " no area constrain";
                }

                if (reportneighbors) {
                    cout << "  " << " neighbors";
                    for (j = 0; j < 3; j++)
                        cout << " " << io.neighborlist[i * 3 + j];
                }
                cout << endl;
            }
        }
        else
            cout << "Triangles aren't set";
    }
    cout << endl;

    cout << "Total of holes   : " << io.numberofholes << endl;
    cout << "Total of regions : " << io.numberofregions << endl;


    if (reportsegments){
        cout << "Total of segments:  " << io.numberofsegments << endl;
        if (io.numberofsegments > 0 && io.segmentlist != NULL)
            for (i = 0; i < io.numberofsegments; i++) {
                cout << "Segment " << i << " points: ";

                for (j = 0; j < 2; j++)
                    cout << "  " << io.segmentlist[i * 2 + j];

                if (markers && io.segmentmarkerlist != NULL)
                    cout << "   marker " << io.segmentmarkerlist[i];
                else 
                    cout << " no marker" << endl;
                
                cout << endl;   
            }
        else
            cout << "Segments aren't set";
    }
    cout << endl;

    if (reportedges){
        cout << "Total of edges:  " << io.numberofedges << endl;
        if(io.numberofedges > 0 && io.edgelist != NULL)
            for (i = 0; i < io.numberofedges; i++) {
                cout << "Edge " << i << " points:";
                for (j = 0; j < 2; j++) 
                    cout << "  " << io.edgelist[i * 2 + j];
                if (reportnorms && (io.edgelist[i * 2 + 1] == -1)) 
                    for (j = 0; j < 2; j++) 
                        cout << "  " << io.normlist[i * 2 + j];
                if (markers && io.edgemarkerlist != NULL)
                    cout << "   marker " << io.edgemarkerlist[i];
                else
                    cout << " no marker";
                    
                
                cout << endl;
            }
        else
            cout << "Edges aren't set";
    }
    cout << endl;
}

void Mesh::TriangleGeneratorExample()
{
    /*
        p - reads PSLG.. 
                1) will generate Constrained Delaunay. 
        
                2) if are used -s(theoretical interest), 
                    -q(angles greter than 20, quality q23), -a maximum triangle area(a0.1) or 
                    -u.. will generate 
                        conforming constrained Delaunay
                3) if -D - conforming Delaunay triangulation(every triangle is Delaunay)
                4) if -r(refine) then segments of prev coarser mesh will be preserved

        u - user-defined constraint, see triumsuitetable() for more details

        z - numbering starts from zero
        r - refine previously generated mesh, with preserving of segments
        q - quality(minimu 20 degree).. also angle can be specified by q30
        a - maximum triangle area constrain. a0.1
        D - all triangles will be delaunay. Not just constrained delaunay.
        
        c - enclose convex hullwith segments
        C - check final mesh if it was conf with X
        
        A - asign additional regional attribute to each triangle, and itspecifies to
            each closed segment region it belongs. (has effect with -p and without -r)
        e - outputs list of edges
        v - outputs voronoi diagram
        n - outputs neighbours
        
        B - suppress boundary markers in output file
        P - suppress output poly file
        N - suppress node file
        E - suppress ele file
        I - suppress file mesh iteration number
        O - suppress holes
        X - suppress exact arithmetics

        o2 - generates second order mesh
        Y - prohibits stainer points on mesh boundary
        YY -                  ------ on any segment
        S - specify max num of added steiner points


        i - use incremental algorithm instead of divide and conqure
        F - fortune algorithm instead of delauna


        Q - quite
        V - verbose
        h - help
    
        numbering starts from 1 unless z is specified

        pointlist: array of point coordinates, each point ocupies two reals
        pointattributelist: array of point attributes
            each point occupies 'numberofpointattributes' REALs

        pointmarkerlist: array of point markers; one int per point

        trianglelist: array of triangle corner list
        triangleattributelist
        trianglearealist - area constrains
        neighborlist

        segmentlist - array of segment endpoints, two per segment
        segmentmarkerlist - array of segment markers, one int per segment
        holelist - array of holes

        regionlist - array regional attributes and area constrains

        edgelist - output only
        edgemarkerlist - output
    */
    struct triangulateio in, mid, out, vorout;
    set_tria_to_default(&in);
    set_tria_to_default(&mid);
    set_tria_to_default(&out);
    set_tria_to_default(&vorout);


    /* Define input points. */

    in.numberofpoints = 4;
    in.numberofpointattributes = 1;
    in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
    in.pointlist[0] = 0.0;
    in.pointlist[1] = 0.0;
    in.pointlist[2] = 1.0;
    in.pointlist[3] = 0.0;
    in.pointlist[4] = 1.0;
    in.pointlist[5] = 10.0;
    in.pointlist[6] = 0.0;
    in.pointlist[7] = 10.0;
    in.pointattributelist = (REAL *) malloc(in.numberofpoints *
                                            in.numberofpointattributes *
                                            sizeof(REAL));
    in.pointattributelist[0] = 0.0;
    in.pointattributelist[1] = 1.0;
    in.pointattributelist[2] = 11.0;
    in.pointattributelist[3] = 10.0;
    in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));
    in.pointmarkerlist[0] = 0;
    in.pointmarkerlist[1] = 2;
    in.pointmarkerlist[2] = 0;
    in.pointmarkerlist[3] = 0;

    in.numberofsegments = 0;
    in.numberofholes = 0;
    in.numberofregions = 1;
    in.regionlist = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
    in.regionlist[0] = 0.5;
    in.regionlist[1] = 5.0;
    in.regionlist[2] = 7.0;            /* Regional attribute (for whole mesh). */
    in.regionlist[3] = 0.1;            /* Area constraint that will not be used. */


    /* Make necessary initializations so that Triangle can return a */
    /*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

    mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
    /* Not needed if -N switch used or number of point attributes is zero: */
    mid.pointattributelist = (REAL *) NULL;
    mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */
    mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
    /* Not needed if -E switch used or number of triangle attributes is zero: */
    mid.triangleattributelist = (REAL *) NULL;
    mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */
    /* Needed only if segments are output (-p or -c) and -P not used: */
    mid.segmentlist = (int *) NULL;
    /* Needed only if segments are output (-p or -c) and -P and -B not used: */
    mid.segmentmarkerlist = (int *) NULL;
    mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
    mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

    vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
    /* Needed only if -v switch used and number of attributes is not zero: */
    vorout.pointattributelist = (REAL *) NULL;
    vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
    vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

    /* Triangulate the points.  Switches are chosen to read and write a  */
    /*   PSLG (p), preserve the convex hull (c), number everything from  */
    /*   zero (z), assign a regional attribute to each element (A), and  */
    /*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
    /*   neighbor list (n).                                              */

    char triswitches1[] = {'p','c', 'z', 'A', 'e', 'v', 'b', '\0'};
    triangulate(triswitches1, &in, &mid, &vorout);

    printf("Output triangulation:\n\n");
    Print(mid, triswitches1);
    printf("Initial Voronoi diagram:\n\n");
    Print(vorout, triswitches1);

    /* Attach area constraints to the triangles in preparation for */
    /*   refining the triangulation.                               */

    /* Needed only if -r and -a switches used: */
    mid.trianglearealist = (REAL *) malloc(mid.numberoftriangles * sizeof(REAL));
    mid.trianglearealist[0] = 3.0;
    mid.trianglearealist[1] = 1.0;

    /* Make necessary initializations so that Triangle can return a */
    /*   triangulation in `out'.                                    */

    out.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
    /* Not needed if -N switch used or number of attributes is zero: */
    out.pointattributelist = (REAL *) NULL;
    out.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
    /* Not needed if -E switch used or number of triangle attributes is zero: */
    out.triangleattributelist = (REAL *) NULL;

    /* Refine the triangulation according to the attached */
    /*   triangle area constraints.                       */

    char triswitches2[] = {'p','r', 'a', 'z', 'B', 'P', '\0'};
    triangulate(triswitches2, &mid, &out, (struct triangulateio *) NULL);

    printf("Refined triangulation:\n\n");
    Print(out, triswitches2);

    /* Free all allocated arrays, including those allocated by Triangle. */

    free(in.pointlist);
    free(in.pointattributelist);
    free(in.pointmarkerlist);
    free(in.regionlist);
    free(mid.pointlist);
    free(mid.pointattributelist);
    free(mid.pointmarkerlist);
    free(mid.trianglelist);
    free(mid.triangleattributelist);
    free(mid.trianglearealist);
    free(mid.neighborlist);
    free(mid.segmentlist);
    free(mid.segmentmarkerlist);
    free(mid.edgelist);
    free(mid.edgemarkerlist);
    free(vorout.pointlist);
    free(vorout.pointattributelist);
    free(vorout.edgelist);
    free(vorout.normlist);
    free(out.pointlist);
    free(out.pointattributelist);
    free(out.trianglelist);
    free(out.triangleattributelist);
}   

void Mesh::TriangleGenerator()
{
    
    triangulateio in, out, vorout;
    set_tria_to_default(&in);
    set_tria_to_default(&out);
    set_tria_to_default(&vorout);
    //input data
    /*

    */
    char triswitches[] = {'p','z', 'V', 'A', 'q', '2', '0', 'D', 'C',  'e', 'c', 'v', 'n',  '\0'};

    vector <double> pointList = 
        {0.0, 0.0,  //0
         1.0, 0.0,  //2
         1.0, 10.0,  //3
         0.0, 10.0  //4
        };

    vector <double> pointAttributeList = 
        {0.0, 1.0, 11.0, 10.0};
    
    vector <int> pointMarkerList = 
        {0, 2, 0, 0};

    vector <int> segmentList = 
        {
        //0, 1, //1
        //1, 2, //2
        //2, 3, //3
        //3, 1
        };
    
    vector <int> segmentMarkerList = 
        {
        //    0, 1, 2, 3
        };
    
    vector <double> regionList = 
        {0.5, 5.0, 7.0, 0.1};

    //points
    in.numberofpoints = 4;
    in.numberofpointattributes = 1;
    in.pointlist = &pointList[0];
    in.pointattributelist = &pointAttributeList[0];
    in.pointmarkerlist = &pointMarkerList[0];



    //region
    in.numberofregions = 1;
    in.regionlist = &regionList[0];

    //edge - read only

    cout << endl << endl << "Input point set:" << endl << endl;
    Print(in, triswitches);

    //mesh generation main call
    cout << endl << "Triangulation Started" << endl << endl;
    triangulate(triswitches, &in, &out, &vorout);

    cout << endl << endl << "Output triangulation:" << endl << endl;
    Print(out, triswitches);

    cout << endl << endl << " Voronoi diagram:" << endl << endl;
    Print(vorout, triswitches);
}

Mesh::~Mesh()
{
    trifree(NULL);
}

class RiverSim
{
    public:
        RiverSim(po::variables_map &vm);
        ~RiverSim();
        void run();

    private:
        void geo_mesh_generator();
        void gmsh_mesh_generator();
        void mesh_covertor();
        void make_grid ();
        void make_custom_grid();
        void setup_system ();
        void assemble_system ();
        void solve ();
        void output_results () const;

        const static int dim = 2;

        Triangulation<dim>   triangulation;
        FE_Q<dim>            fe;
        DoFHandler<dim>      dof_handler;
        SparsityPattern      sparsity_pattern;
        SparseMatrix<double> system_matrix;
        Vector<double>       solution;
        Vector<double>       system_rhs;

        //options fro command line 
        po::variables_map    option_map;  
};


RiverSim::RiverSim (po::variables_map &vm): fe (1), dof_handler(triangulation)
{
    option_map = vm;

    /*
        GMSH intialization
    */

    gmsh::initialize();
    gmsh::option::setNumber("Mesh.RecombineAll", 1);
    gmsh::option::setNumber("Mesh.RecombinationAlgorithm", 1);
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::option::setNumber("General.Terminal", (int)option_map["gmsh-log"].as<bool>());
    mdl::add("square");
}


RiverSim::~RiverSim()
{
    gmsh::finalize();
}


void RiverSim::geo_mesh_generator()
{
    geo::addPoint(0, 0, 0, 0.1, 1);
    geo::addPoint(0.5, 0, 0, 0.1, 2);
    geo::addPoint(1, 0, 0, 0.1, 3);
    geo::addPoint(1, 1, 0, 0.1, 4);
    geo::addPoint(0, 1, 0, 0.1, 5);
    geo::addPoint(0.5, 0.2, 0, 0.1, 6);

    geo::addLine(1, 2, 1);
    geo::addLine(2, 3, 2);
    geo::addLine(3, 4, 3);
    geo::addLine(4, 5, 4);
    geo::addLine(5, 1, 5);
    geo::addLine(2, 6, 6);
    geo::addLine(6, 2, 7);

    geo::addCurveLoop({1, 2, 3, 4, 5, 6, 7}, 1);
    geo::addPlaneSurface({1}, 6);
    geo::synchronize();
    mdl::mesh::generate(2);
}


void RiverSim::gmsh_mesh_generator()
{
    //TODO: implement right workwflow
    
    /*
        defining of geometry
    */
    cout << "discr entity" << endl;
    
    mdl::addDiscreteEntity(1, 1);
    
    
    cout << "set nodes" << endl;
    msh::setNodes(1, 1, 
        {1, 2, 3, 4, 5, 6, 7}, 
        {0.,  0.,  0.,   //node 1
         0.5, 0.,  0.,   //node 2
         1.,  0.,  0.,   //node 3
         1.,  1.,  0.,   //node 4
         0.,  1.,  0.,   //node 5
         0.5, 0.2, 0.,   //node 6
         0.5, 0.6,  0.   //node 7
        });

    cout << "set elements" << endl;
    msh::setElements(1, 1, 
        {1},//line element 
        {{1, 2, 3, 4, 5, 6, 7}},//line tags
        {{1, 2, 2, 6, 6, 2, 2, 3, 3, 4, 4, 5, 5, 1}}); //lines

    geo::addPoint(0.5, 0.6, 0., 0.01, 99);
    geo::addCurveLoop({1}, 100);
    geo::addPlaneSurface({100}, 101);
    geo::synchronize();

    mdl::mesh::generate(2);
}


void RiverSim::make_custom_grid()
{
    static const Point<2> vertices_1[] = 
    {   Point<2> (0., 0.),
        Point<2> (1,  0.),
        Point<2> (0,  1.),
        Point<2> (1., 1.)
    }; 
    
    const unsigned int
    n_vertices = sizeof(vertices_1) / sizeof(vertices_1[0]);
    
    const vector<Point<dim> > vertices (&vertices_1[0],
                                         &vertices_1[n_vertices]);
    
    static const int cell_vertices[][GeometryInfo<dim>::vertices_per_cell]
            = {{0, 1, 2, 3}};

    const unsigned int
        n_cells = sizeof(cell_vertices) / sizeof(cell_vertices[0]);

    vector<CellData<dim> > cells (n_cells, CellData<dim>());
    
    for (unsigned int i=0; i<n_cells; ++i)
    {
        for (unsigned int j=0;
                j<GeometryInfo<dim>::vertices_per_cell;
                ++j)
            cells[i].vertices[j] = cell_vertices[i][j];
        cells[i].material_id = 0;
    }
    
    triangulation.create_triangulation (vertices,
                                    cells,
                                    SubCellData());
    
    triangulation.refine_global (5);
}

void RiverSim::setup_system ()
{
    dof_handler.distribute_dofs (fe);
    cout << "Number of degrees of freedom: "
              << dof_handler.n_dofs()
              << endl; 

    DynamicSparsityPattern dsp(dof_handler.n_dofs());

    DoFTools::make_sparsity_pattern (dof_handler, dsp);
    sparsity_pattern.copy_from(dsp);
    system_matrix.reinit (sparsity_pattern);
    solution.reinit (dof_handler.n_dofs());
    system_rhs.reinit (dof_handler.n_dofs());
}

void RiverSim::assemble_system ()
{
    QGauss<2>  quadrature_formula(2);
    FEValues<2> fe_values (fe, quadrature_formula,
                           update_values | update_gradients | update_JxW_values);
    const unsigned int   dofs_per_cell = fe.dofs_per_cell;
    const unsigned int   n_q_points    = quadrature_formula.size();
    FullMatrix<double>   cell_matrix (dofs_per_cell, dofs_per_cell);
    Vector<double>       cell_rhs (dofs_per_cell);
    std::vector<types::global_dof_index> local_dof_indices (dofs_per_cell);
    DoFHandler<2>::active_cell_iterator
    cell = dof_handler.begin_active(),
    endc = dof_handler.end();

    for (; cell!=endc; ++cell)
    {
        fe_values.reinit (cell);
        cell_matrix = 0;
        cell_rhs = 0;
        for (unsigned int q_index = 0; q_index < n_q_points; ++q_index)
        {
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                for (unsigned int j = 0; j < dofs_per_cell; ++j)
                    cell_matrix(i, j) += (fe_values.shape_grad (i, q_index) *
                                     fe_values.shape_grad (j, q_index) *
                                     fe_values.JxW (q_index));    
            for (unsigned int i = 0; i < dofs_per_cell; ++i)
                 cell_rhs(i) += (fe_values.shape_value (i, q_index) *
                              1 *
                              fe_values.JxW (q_index));
        }

        cell->get_dof_indices (local_dof_indices);

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
            for (unsigned int j = 0; j < dofs_per_cell; ++j)
                system_matrix.add (local_dof_indices[i],
                               local_dof_indices[j],
                               cell_matrix(i, j));

        for (unsigned int i = 0; i < dofs_per_cell; ++i)
            system_rhs(local_dof_indices[i]) += cell_rhs(i);
    }

    std::map<types::global_dof_index,double> boundary_values;

    VectorTools::interpolate_boundary_values (dof_handler,
                                              0,
                                              ZeroFunction<2>(),
                                              boundary_values);

    MatrixTools::apply_boundary_values (boundary_values,
                                        system_matrix,
                                        solution,
                                        system_rhs);
}

void RiverSim::solve ()
{
    SolverControl           solver_control (1000, 1e-12);
    SolverCG<>              solver (solver_control);
    solver.solve (system_matrix, solution, system_rhs,
                  PreconditionIdentity());
}


void RiverSim::output_results () const
{
    DataOut<2> data_out;
    data_out.attach_dof_handler (dof_handler);
    data_out.add_data_vector (solution, "solution");
    data_out.build_patches ();
    std::ofstream output ("solution.vtk");
    data_out.write_vtk (output);
}


void RiverSim::run ()
{
    make_custom_grid ();
    setup_system ();
    assemble_system ();
    solve ();
    output_results ();

    if(option_map.count("b1"))
        gmsh_mesh_generator();
    else
        geo_mesh_generator();

    if (option_map["draw-mesh"].as<bool>())
        gmsh::fltk::run();

    if(option_map.count("output"))
        gmsh::write(option_map["output"].as<string>());
}

void gmsh_possibilites(int argc, char *argv[]);