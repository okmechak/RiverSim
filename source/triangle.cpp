#include "triangle.hpp"
///\cond
#include <iostream>
#include <numeric>///< iota
#include <iterator>///< ostream_iterator
///\endcond

namespace River{

    void Triangle::print_geometry(const struct triangulateio &io) const
    {
        int i, j, shift = 1;

        if (io.numberofpoints > 0 && io.pointlist != NULL)
        {
            cout << "Total of points     : " << io.numberofpoints << endl;
            cout << "         attributes : " << io.numberofpointattributes << endl;
            if (io.pointmarkerlist == NULL)
                cout << "Markers aren't set " << endl;

            for (i = 0; i < io.numberofpoints; i++)
            {
                cout << "Point " << i + shift << ": ";

                for (j = 0; j < 2; j++)
                    cout << io.pointlist[i * 2 + j] << " ";

                if (io.numberofpointattributes > 0 && io.pointattributelist != NULL)
                {
                    cout << "   attributes  ";
                    for (j = 0; j < io.numberofpointattributes; j++)
                        cout << io.pointattributelist[i * io.numberofpointattributes + j] << " ";
                }
                else
                    cout << " point attributes aren't set ";

                if (io.pointmarkerlist != NULL)
                    cout << "   marker " << io.pointmarkerlist[i];

                cout << endl;
            }
        }
        else
            cout << "Points aren't set";

        cout << endl;

        cout << "Total of triangles:  " << io.numberoftriangles << endl;
        cout << "         attributes: " << io.numberoftriangleattributes << endl;
        cout << "         corners:    " << io.numberofcorners << endl;
        if (io.numberoftriangles > 0 && io.trianglelist != NULL)
        {
            for (i = 0; i < io.numberoftriangles; i++)
            {
                cout << "Triangle " << i + shift << " points: ";
                for (j = 0; j < io.numberofcorners; j++)
                    cout << io.trianglelist[i * io.numberofcorners + j] << " ";
                if (io.numberoftriangleattributes > 0 && io.triangleattributelist != NULL)
                {
                    cout << "   attributes";
                    for (j = 0; j < io.numberoftriangleattributes; j++)
                        cout << "  " << io.triangleattributelist[i * io.numberoftriangleattributes + j];
                }
                else
                    cout << " no attributes ";

                if (io.trianglearealist != NULL)
                    cout << " area constrain " << io.trianglearealist[i];
                else
                    cout << " no area constrain";

                cout << "  "
                     << " neighbors";
                if(io.neighborlist != NULL)
                    for (j = 0; j < 3; j++)
                        cout << " " << io.neighborlist[i * 3 + j];
                else
                    cout << " no neighbors ";

                cout << endl;
            }
        }
        else
            cout << "Triangles aren't set";

        cout << endl;

        cout << "Total of holes   : " << io.numberofholes << endl;
        cout << "Total of regions : " << io.numberofregions << endl;

        cout << "Total of segments:  " << io.numberofsegments << endl;
        if (io.numberofsegments > 0 && io.segmentlist != NULL)
            for (i = 0; i < io.numberofsegments; i++)
            {
                cout << "Segment " << i + shift << " points: ";
                for (j = 0; j < 2; j++)
                    cout << "  " << io.segmentlist[i * 2 + j];
                if (io.segmentmarkerlist != NULL)
                    cout << "   marker " << io.segmentmarkerlist[i];
                else
                    cout << " no marker" << endl;
                cout << endl;
            }
        else
            cout << "Segments aren't set";

        cout << endl;

        cout << "Total of edges:  " << io.numberofedges << endl;
        if (io.numberofedges > 0 && io.edgelist != NULL)
            for (i = 0; i < io.numberofedges; i++)
            {
                cout << "Edge " << i + shift << " points:";
                for (j = 0; j < 2; j++)
                    cout << "  " << io.edgelist[i * 2 + j];
                if (io.normlist != NULL)
                    for (j = 0; j < 2; j++)
                        cout << "  " << io.normlist[i * 2 + j];
                if (io.edgemarkerlist != NULL)
                    cout << "   marker " << io.edgemarkerlist[i];
                else
                    cout << " no marker";
                cout << endl;
            }
        else
            cout << "Edges aren't set";
        cout << endl;
    }

    struct triangulateio Triangle::tethex_to_io(const tethex::Mesh &mesh) const
    {
        struct triangulateio io;
        set_tria_to_default(&io);

        //Points
        if(mesh.get_n_vertices())
        {
            auto vertices_num = mesh.get_n_vertices();
            io.pointlist = new REAL[2 * vertices_num];
            io.pointmarkerlist = new int[vertices_num];
            io.numberofpoints = vertices_num;

            int i = 0;
            for(auto &p: mesh.get_vertices())
            {
                io.pointlist[2 * i ] = p.get_coord(0);
                io.pointlist[2 * i + 1] = p.get_coord(1);
                io.pointmarkerlist[i] = p.regionTag;
                ++i;
            }
        }

        //Holes
        if(mesh.get_n_holes())
        {
            auto holes_num = mesh.get_n_holes();
            io.numberofholes = holes_num;
            io.holelist = new REAL[2 * holes_num];
            
            int i = 0;
            for(auto &p: mesh.get_holes())
            {
                io.holelist[2 * i ] = p.get_coord(0);
                io.holelist[2 * i + 1] = p.get_coord(1);
                ++i;
            }
        }

        //Segments
        if(mesh.get_n_lines())
        {
            auto lines_num = mesh.get_n_lines();
            io.segmentlist = new int[2 * lines_num];
            io.segmentmarkerlist = new int[lines_num];
            io.numberofsegments = lines_num;
            int i = 0;
            for(auto l: mesh.get_lines())
            {
                io.segmentlist[2 * i ] = l->get_vertex(0) + 1;
                io.segmentlist[2 * i + 1] = l->get_vertex(1) + 1;
                io.segmentmarkerlist[i] = l->get_material_id();
                ++i;
            }
        }

        //Triangles
        if(mesh.get_n_triangles())
        {
            auto triangles_num = mesh.get_n_triangles();
            io.trianglelist = new int[3 * triangles_num];
            io.numberoftriangleattributes = 1;
            io.triangleattributelist = new REAL[triangles_num];
            io.numberofsegments = triangles_num;
            int i = 0;
            for(auto t: mesh.get_triangles())
            {
                io.trianglelist[3 * i ] = t->get_vertex(0) + 1;
                io.trianglelist[3 * i + 1] = t->get_vertex(1) + 1;
                io.trianglelist[3 * i + 2] = t->get_vertex(2) + 1;
                io.triangleattributelist[i] = t->get_material_id();
                ++i;
            }
        }

        return io;
    }

    void  Triangle::io_to_tethex(
        const struct triangulateio &io, tethex::Mesh &initMesh) const
    {
        vector<tethex::Point> pointsVal;
        vector<tethex::MeshElement*> segmentsVal;
        vector<tethex::MeshElement*> trianglesVal;

        pointsVal.reserve(io.numberofpoints);
        for(int i = 0; i < io.numberofpoints; ++i)
        {
            auto x = io.pointlist[2 * i],
                y = io.pointlist[2 * i + 1];
            auto regionTag = io.pointmarkerlist[i];

            pointsVal.push_back(tethex::Point(x, y, 0/*z-component*/, regionTag));
        }

        segmentsVal.reserve(io.numberofsegments);
        for(int i = 0; i < io.numberofsegments; ++i)
        {
            auto v1 = io.segmentlist[2*i] - 1,//NOTE vertices index should start from zero
                v2 = io.segmentlist[2*i + 1] - 1,
                regionTag = io.segmentmarkerlist[i];
            if(regionTag == 0)//NOTE boundary_id of inner cells in Deal.II should be -1
                regionTag = -1;
            segmentsVal.push_back(new tethex::Line(v1, v2, regionTag));
        }

        trianglesVal.reserve(io.numberoftriangles);
        for(int i = 0; i < io.numberoftriangles; ++i)
        {
            auto v1 = io.trianglelist[3*i] - 1,//NOTE vertice index should start from zero
                v2 = io.trianglelist[3*i + 1] - 1,
                v3 = io.trianglelist[3*i + 2] - 1;   

            //taking only the first one attribute
            double regionTag = 0;
            if(io.numberoftriangleattributes != 0)
                regionTag = io.triangleattributelist[io.numberoftriangleattributes * i];

            trianglesVal.push_back(new tethex::Triangle(v1, v2, v3, regionTag));
        }

        initMesh.set_vertexes(pointsVal);
        initMesh.set_lines(segmentsVal);
        initMesh.set_triangles(trianglesVal);
    }

    /*
        Triangle Class
    
    */
    Triangle::Triangle()
    {
        set_all_values_to_default();
    }

    Triangle::Triangle(MeshParams mesh_params):
        mesh_params(mesh_params)
    {
        set_all_values_to_default();
        initialize_mesh_parameters(mesh_params);
    }

    void Triangle::initialize_mesh_parameters(const MeshParams &mesh_params)
    {
        AreaConstrain = true;
        CustomConstraint = true;
        MaxTriaArea = mesh_params.max_area;
        MinTriaArea = mesh_params.min_area;
        MaxEdgeLenght = mesh_params.max_edge;
        MinEdgeLenght = mesh_params.min_edge;
        MinAngle = mesh_params.min_angle;
        MaxTriangleRatio = mesh_params.ratio;
        Verbose = false;
        Quite =  true;
    }

    Triangle::~Triangle()
    {   
        //we don't call this function
        //free_allocated_memory();
        //cos it is already present at the end of generate() function
    }

    void Triangle::set_all_values_to_default()
    {
        update_options();
        set_tria_to_default(&in);
        set_tria_to_default(&out);
        set_tria_to_default(&vorout);
    }

    void Triangle::free_allocated_memory()
    {

        triangulateiofree(&in);
        triangulateiofree(&out);
        triangulateiofree(&vorout);
    }

    string Triangle::update_options()
    {
        options = "";
        #define MAX_ANGLE 36
        if (ConstrainAngle)     options += "q";
        if (ConstrainAngle && MinAngle > 0 && MinAngle < MAX_ANGLE)
                                options += to_string(MinAngle);
        else if (ConstrainAngle && MinAngle > MAX_ANGLE)
            throw Exception("Triangle quality angle should be equal or less then 36");
        //if (StartNumberingFromZero)
        //    options += "z";
        if (Refine)             options += "r";
        if (AreaConstrain)      options += "a";
        if (AreaConstrain && MaxTriaArea > 0)
                                options +=  to_string(MaxTriaArea);
        if (CustomConstraint)   options += "u";
        if (DelaunayTriangles)  options += "D";
        if (EncloseConvexHull)  options += "c";
        if (CheckFinalMesh)     options += "C";
        if (AssignRegionalAttributes) options += "A";
        if (VoronoiDiagram)     options += "v";
        if (Quite)              options += "Q";
        else if (Verbose)       options += "V";
        if (Algorithm == FORTUNE)
                                options += "F";
        else if (Algorithm == ITERATOR)
                                options += "i";
        if (ReadPSLG)           options += "p";
        if (SuppressBoundaryMarkers)
                                options += "B";
        if (SuppressPolyFile)   options += "P";
        if (SuppressNodeFile)   options += "N";
        if (SuppressEleFile)    options += "E";
        if (OutputEdges)        options += "e";
        if (ComputeNeighbours)  options += "n";
        if (SuppressMehsFileNumbering)
                                options += "I";
        if (SuppressExactArithmetics)
                                options += "X";
        if (SuppressHoles)      options += "O";
        //if (SecondOrderMesh)
        //    options += "o2";
        if (SteinerPointsOnBoundary) options += "Y";
        if (SteinerPointsOnSegments) options += "YY";
        if (MaxNumOfSteinerPoints > 0)
                                options += "S" + to_string(MaxNumOfSteinerPoints);

        return options;
    }

    string Triangle::print_options(bool qDetailedDescription)
    {
        update_options();

        std::ostringstream stream;
        stream.precision(16);

        stream << "Triangle options command : " << options << endl;

        if(!qDetailedDescription) return "";

        stream << "Detailed description: " << endl << endl; 

        if (ReadPSLG)        stream << "(p) read PSLG" << endl;
        //if (StartNumberingFromZero)
        //    stream << "(z) numbering starts from zero" << endl;
        if (Refine)          stream << "(r) refine" << endl;
        if (ConstrainAngle)  stream << "(q) quality min 20 degree" << endl;
        if (MinAngle > 0)    stream << "                    value:" << MinAngle << endl;
        if(AreaConstrain)    stream << "(a) area constrain: " << endl;
        if (MaxTriaArea > 0) stream << "                  area constrain: " << MaxTriaArea << endl;
        if (CustomConstraint) stream << "(u) imposes a user-defined constraint on triangle size" << endl;
        if (DelaunayTriangles)stream << "(D) all traingles will be Delaunay" << endl;
        if (EncloseConvexHull)stream << "(c) enclose convex hull" << endl;
        if (CheckFinalMesh)  stream << "(C) check final mesh" << endl;
        if (AssignRegionalAttributes)
                             stream << "(A) assign additional attribute to each triangle which specifies segment which it belongs too" << endl;
        if (OutputEdges)     stream << "(e) output list of edges" << endl;
        if (VoronoiDiagram)  stream << "(v) outputs voronoi diagram" << endl;
        if (ComputeNeighbours)stream << "(n) outputs neighboors" << endl;
        if (SuppressBoundaryMarkers)stream << "(B) suppress boundary markers" << endl;
        if (SuppressPolyFile)stream << "(P) suppress output poly file(either don't work)" << endl;
        if (SuppressNodeFile)stream << "(N) suppress output nodes file(either don't work)" << endl;
        if (SuppressEleFile) stream << "(E) suppress output elements file(either don't work)" << endl;
        if (SuppressHoles)   stream << "(O) suppress holes" << endl;
        //if (SecondOrderMesh)
        //    stream << "(o2) second order mesh" << endl;
        if (SteinerPointsOnBoundary || SteinerPointsOnSegments)
                            stream << "(Y) prohibits stainer points on boundary" << endl;
        if (MaxNumOfSteinerPoints > 0)
                            stream << "(S) specify max number off added Steiner points" << endl;
        if (Algorithm == ITERATOR)
                            stream << "(i) use incremental algorithm" << endl;
        if (Algorithm == FORTUNE)
                            stream << "(F) use Fortune algorithm" << endl;
        if (Quite)          stream << "(Q) quite" << endl;
        if (Verbose)        stream << "(V) verbose" << endl;

        return stream.str();
    }

    struct triangulateio* Triangle::get_geometry()
    {
        return &out;
    }

    struct triangulateio* Triangle::get_voronoi()
    {
        return &vorout;
    }

    tethex::Mesh Triangle::generate_quadrangular_mesh(const Boundary &boundary, const t_PointList &holes)
    {
        if (boundary.vertices.empty())
            return tethex::Mesh();

        set_all_values_to_default();

        tethex::Mesh mesh_object(boundary, holes);
        in = tethex_to_io(mesh_object);

        if (Verbose)
        {
            cout << "Input Geometry: " << endl;
            print_geometry(in);
        }

        //Main call to Triangle
        triangulate(options.c_str(), &in, &out, &vorout, &mesh_params);

        if (Verbose)
        {
            cout << "Output Geometry: " << endl;
            print_geometry(out);
        }

        if (Verbose && VoronoiDiagram)
        {
            cout << "Voronoi Diagram: " << endl;
            print_geometry(vorout);
        }

        io_to_tethex(out, mesh_object);
        mesh_object.convert();
        free_allocated_memory();

        return mesh_object;
    }
} //end of River namespace