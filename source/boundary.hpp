#pragma once

///\cond
#include <vector>
#include <string>
#include <map>
///\endcond

#include "point.hpp"

using namespace std;

/*! \namespace River
    \brief River namespace holds everything that is related to River simulation.
*/
namespace River
{   
    ///Vertice positionn index data type.
    typedef size_t t_vert_pos;
    ///Boundary id data type.
    typedef long int t_boundary_id;
    ///Source id data type.
    typedef size_t t_source_id;
    
    ///Source point coordinate data type. Pair which holds boundary id and vertice position on this id.
    typedef pair<t_boundary_id, t_vert_pos> t_source_coord;
    ///Vector of source point ids data type.
    typedef vector<t_source_id> t_sources_ids;
    ///Map structure which holds source id and its coordinates.
    typedef map<t_source_id/*or river id*/, t_source_coord> t_Sources;
    ///Map structure which holds source id and its source point coordinates.
    class Sources: public t_Sources
    {
        public:
            ///Returns all sources ids.
            t_sources_ids GetSourcesIds() const;
    };

    /*! \brief 
        Line - holds indexes of __p1__, __p2__ vertices and id.
        Its structure is depended on Mesh data structure.
    */
    class Line
    {
        public:
            ///Creates line with zero points indexes and boundary condition.
            Line() = default;

            Line(const t_vert_pos p1v, const t_vert_pos p2v, const t_boundary_id boundary_id):
                p1(p1v), 
                p2(p2v), 
                boundary_id(boundary_id)
            {};

            Line(const Line& line) = default;

            ///First point index.
            t_vert_pos p1 = 0; 
            ///Second point index.
            t_vert_pos p2 = 0;

            ///Boundary condition id.
            t_boundary_id boundary_id = 0;

            bool operator==(const Line& line) const;
    };

    ///Vector of points data type.
    typedef vector<Point> t_PointList;
    
    ///Vector of lines data type.
    typedef vector<Line> t_LineList;

    /*! \brief Structure which defines simple boundary data structure.
        
        \details Simple boundary means only one boundary.
        \imageSize{BorderClass.jpg, height:40%;width:40%;, }
    */
    class Boundary
    {
        public:
            Boundary(){};
            Boundary(t_PointList v, t_LineList l): vertices{v}, lines{l} {};
            ///Vertices of boundary vector.
            t_PointList vertices;
            ///Connvections between boundaries.
            t_LineList lines;
            ///Appends another simple boundary at the end of current boundary.
            void Append(const Boundary& boundary);
            ///Replace on element of boundary with whole simple boundary structure.
            void ReplaceElement(t_vert_pos vertice_pos, const Boundary& boundary);
            /// Fixes line indices with correct one, like 0 1, 1 2, 2 3 etc.
            void FixLinesIndices(const bool is_closed_boundary = false);
            ///Generate smooth boundaries with removed some vertices. Coarsening is controlled by min_degree and also by ignored_distance. Smoothing starts from end.
            Boundary generateSmoothBoundary(const double min_degree, const double ignored_distance) const;
            bool operator==(const Boundary& simple_boundary) const;
    };

    ///Source point of river holds coordinates and dirrection.
    typedef pair<Point, double> t_branch_source;

    ///Data structure - interface between boundary sources and source points of \ref Rivers
    typedef map<t_source_id/*same as branch id*/, t_branch_source/*t_branch_source*/> t_rivers_interface;
}//namespace River