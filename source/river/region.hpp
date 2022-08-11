#pragma once

#include "rivers.hpp"

namespace River
{
    
    typedef map<t_boundary_id, Boundary> t_Region;

    class RegionParams
    {
        public:
            ///Smoothnes minimal degree. This value sets threshold for degree between adjacent points below which it should be ignored. This creates smaller mesh.
            double smoothness_degree = 0.2;
    
            ///Smoothnes minimal length. This value sets threshold for length where smoothnest near tip will be ignored. Ideally it should be bigger then integration radius.
            double ignored_smoothness_length = 0.01;
    
            double river_width = 1e-7;
    
            t_boundary_id river_boundary_id = 100;
    
            bool operator==(const RegionParams &rp) const
            {
                return 
                       abs(smoothness_degree - rp.smoothness_degree) < EPS 
                    && abs(ignored_smoothness_length - rp.ignored_smoothness_length) < EPS 
                    && abs(river_width - rp.river_width) < EPS 
                    && river_boundary_id == rp.river_boundary_id;
            }
    };

    /*! \brief Structure which defines Region of region.
        
        \details It is a combination of simple boundaries which of each has  it own unique id.
        \imageSize{GeometryClasses.jpg, height:40%;width:40%;, }
    */
    class Region: public t_Region
    {
        public:
            
            ///Constructor
            Region(t_Region region = {});

            ///Initialize rectangular Region.
            Sources MakeRectangular(double width = 1, double height = 1, double source_x_position = 0.25);
            
            ///Initialize rectangular with hole Region.
            Sources MakeRectangularWithHole(double width = 1, double height = 1, double source_x_position = 0.25);

            ///Some basic checks of data structure.
            void Check();

            ///Returns vector of all holes.
            t_PointList GetHolesList() const;

            ///Returns map of source points ids and coresponding source point and angle of tree \ref Branch.
            t_rivers_interface GetSourcesIdsPointsAndAngles(const Sources& sources) const;

            ///Array of holes. Which will be eliminated by mesh generator.
            t_PointList holes;

            ///Returns normal angle based on three points.
            static double NormalAngle(const Point& left, const Point& center, const Point& right);

            ///Returns andjancent vertice indexes.
            static pair<t_vert_pos, t_vert_pos> GetAdjacentVerticesPositions(
                const t_vert_pos vertices_size, 
                const t_vert_pos vertice_pos);
            
            ///Returns normal angle at point with vertice_pos index.
            static double GetVerticeNormalAngle(    
                const t_PointList& vertices, 
                const t_vert_pos vertice_pos);
    };

    ///Generates trees boundary.
    void RiversBoundary(
        Boundary &rivers_boundary, const Rivers& rivers, const unsigned river_id, const RegionParams& rp);

    ///Generates boundary from region and rivers.
    Boundary BoundaryGenerator(
        const Sources& sources, const Region &region, const Rivers &rivers, const RegionParams& rp);

    ///Checks for intersections of region boundary with tips.
    size_t NumOfBoundaryIntersection(const Boundary &region_boundary, const Boundary &tip_boundary);
}