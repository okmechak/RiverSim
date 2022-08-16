#include "region.hpp"

#include <algorithm>

namespace River
{
    //Region
    Region::Region(t_Region region)
    {
        for(auto &[boundary_id, boundary]: region)
            this->at(boundary_id) = boundary;
    }

    void Region::Check()
    {
        if (this->empty())
            throw Exception("Boundary is empty.");
        
        vector<t_boundary_id> boundary_ids;

        for(const auto& [boundary_id, simple_boundary]: *this)
        {
            boundary_ids.push_back(boundary_id);

            if(simple_boundary.vertices.size() != simple_boundary.lines.size())
                throw Exception("Vertices and lines sizes, are different in boudary");
        }

        auto num_of_boundary_ids = boundary_ids.size();
        sort( boundary_ids.begin(), boundary_ids.end() );
        boundary_ids.erase( 
            unique( boundary_ids.begin(), boundary_ids.end() ), boundary_ids.end() );
        
        if(num_of_boundary_ids != boundary_ids.size())
            throw Exception("Boundary: Boundary ids should be unique numbers!");
    }

    t_PointList Region::GetHolesList() const
    {
        return holes;
    }

    Sources Region::MakeRectangular(double width, double height, double source_x_position)
    {
        (*this)[1] = 
        {
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {source_x_position, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            }
        };/*Outer Boundary*/

        Sources sources;
        sources[1/*source_id*/] = {1/*boundary id*/, 1/*vertice position*/};
            
        return sources;
    }

    Sources Region::MakeRectangularWithHole(double width, double height, double source_x_position)
    {
        (*this)[1] = 
        {/*Outer Boundary*/
            {/*vertices(counterclockwise order)*/
                {0, 0},
                {source_x_position, 0}, 
                {width, 0}, 
                {width, height}, 
                {0, height}
            }, 
            {/*lines*/
                {0, 1, 1},
                {1, 2, 2},
                {2, 3, 3},
                {3, 4, 4},
                {4, 0, 5} 
            }
        };

        Sources sources;
        sources[1] = {1, 1};

        (*this)[2] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.25*width, 0.75*height},
                {0.75*width, 0.75*height}, 
                {0.75*width, 0.25*height}, 
                {0.25*width, 0.25*height}
            }, 
            {/*lines*/
                {0, 1, 6},
                {1, 2, 7},
                {2, 3, 8},
                {3, 0, 9} 
            }
        };
        sources[2] = {2, 1};

        (*this)[3] =
        {/*Hole Boundary*/
            {/*vertices*/
                {0.8*width, 0.9*height},
                {0.9*width, 0.9*height}, 
                {0.9*width, 0.8*height}, 
                {0.8*width, 0.8*height}
            }, 
            {/*lines*/
                {0, 1, 10},
                {1, 2, 11},
                {2, 3, 12},
                {3, 0, 13} 
            }
        };
        sources[3] = {3, 3};

        holes = {{width/2, height/2}, {0.85*width, 0.85*height}};
        
        return sources;
    }

    pair<t_vert_pos, t_vert_pos> Region::GetAdjacentVerticesPositions(
            const t_vert_pos vertices_size, 
            const t_vert_pos vertice_pos)
    {
        if (vertice_pos >= vertices_size)
            throw Exception("Vertice position is bigger then number of vertices: vertice pos " + to_string(vertice_pos));

        t_vert_pos 
            left_pos = vertice_pos - 1, 
            right_pos = vertice_pos + 1;

        if(vertice_pos == vertices_size - 1 )
            right_pos = 0;
        else if(vertice_pos == 0)
            left_pos = vertices_size - 1;
            
        return {left_pos, right_pos};
    }

    double Region::NormalAngle(const Point& left, const Point& center, const Point& right)
    {
        const auto
            left_vector = center - left,
            right_vector = right - center;

        const auto
            angle_relative  = left_vector.angle(right_vector),
            angle_absolute = left_vector.angle();
        
        auto normal_angle = (M_PI + angle_relative) / 2 + angle_absolute;

        if(normal_angle >= 2*M_PI)
            normal_angle -= 2*M_PI;
        else if(normal_angle <= -2*M_PI)
            normal_angle += 2*M_PI;

        return normal_angle;
    }

    double Region::GetVerticeNormalAngle(const t_PointList& vertices, const t_vert_pos vertice_pos)
    {
        const auto vertices_size = vertices.size();
        const auto [vertice_pos_left, vertice_pos_right] = GetAdjacentVerticesPositions(vertices_size, vertice_pos);

        const auto& p_source = vertices.at(vertice_pos), 
            p_left = vertices.at(vertice_pos_left), 
            p_right = vertices.at(vertice_pos_right);
        
        auto normal_angle = NormalAngle(p_left, p_source, p_right);
        
        if(normal_angle >= 2*M_PI)
            normal_angle -= 2*M_PI;
        
        return normal_angle;
    }

    River::t_rivers_interface Region::GetSourcesIdsPointsAndAngles(const Sources& sources) const
    {
        t_rivers_interface sources_ids_points_and_angles;

        for(const auto& [source_id, value]: sources)
        {
            auto boundary_id = value.first;
            auto vertice_pos = value.second;
            auto& simple_boundary = this->at(boundary_id);

            sources_ids_points_and_angles[source_id] = {
                simple_boundary.vertices.at(vertice_pos),
                GetVerticeNormalAngle(
                    simple_boundary.vertices,
                    vertice_pos)
                };
        }

        return sources_ids_points_and_angles;
    }

    /**
        Rivers Vertices Generation
    */
    void RiversBoundary(Boundary &rivers_boundary, const Rivers& rivers, const unsigned river_id, const RegionParams& rp)
    {
        if (rivers.empty())
            return;
            //throw Exception("Boundary generator: trying to generate tree boundary from empty tree.");

        const auto& river_smooth_branch
            = rivers.at(river_id).generateSmoothBoundary(rp.smoothness_degree, rp.ignored_smoothness_length);
        const auto source_angle = rivers.at(river_id).SourceAngle();
        
        Boundary left_boundary, right_boundary;

        auto n = river_smooth_branch.vertices.size() - 1;

        left_boundary.vertices.resize(n);
        right_boundary.vertices.resize(n);
        
        for(unsigned i = 0; i < n; ++i)
        {
            if(i == 0)
            {
                left_boundary.vertices.at(i) = river_smooth_branch.vertices.at(i) 
                    + Point{rp.river_width / 2, 0}.rotate(source_angle + M_PI / 2);
                right_boundary.vertices.at(i) = river_smooth_branch.vertices.at(i) 
                    + Point{rp.river_width / 2, 0}.rotate(source_angle - M_PI / 2);
            }
            else
            {
                left_boundary.vertices.at(i) = river_smooth_branch.vertices.at(i) 
                    + river_smooth_branch.Vector(i - 1).normalize().rotate(M_PI / 2) * rp.river_width / 2;
                right_boundary.vertices.at(i) = river_smooth_branch.vertices.at(i) 
                    + river_smooth_branch.Vector(i - 1).normalize().rotate(-M_PI / 2) * rp.river_width / 2;
            }
        }
        reverse(right_boundary.vertices.begin(), right_boundary.vertices.end());

        // initialization of lines, but indexes are still wrong
        left_boundary.lines = river_smooth_branch.lines;
        right_boundary.lines = river_smooth_branch.lines; reverse(right_boundary.lines.begin(), right_boundary.lines.end());

        if(rivers.HasSubBranches(river_id))
        {
            auto[left_b_id, right_b_id] = rivers.GetSubBranchesIds(river_id);

            rivers_boundary.Append(left_boundary);

            RiversBoundary(rivers_boundary, rivers, left_b_id, rp);

            rivers_boundary.vertices.pop_back();

            RiversBoundary(rivers_boundary, rivers, right_b_id, rp);

            rivers_boundary.Append(right_boundary);
        }
        else
        {
            rivers_boundary.Append(left_boundary);
            rivers_boundary.vertices.push_back(river_smooth_branch.vertices.at(n));
            rivers_boundary.Append(right_boundary);
        }
        rivers_boundary.FixLinesIndices();
    }

    Boundary BoundaryGenerator(const Sources& sources, const Region &region, const Rivers &rivers, const RegionParams& rp)
    {
        Boundary final_boundary;

        auto sources_loc = sources;

        for(auto [boundary_id, boundary]: region)
        {
            for(const auto&[source_id, source_coord]: sources_loc)
            {
                auto boundary_id_s = source_coord.first;
                auto vertice_pos = source_coord.second;
                if(boundary_id == boundary_id_s /*checks if current boundary contains river*/)
                {
                    Boundary rivers_boundary;
                    RiversBoundary(
                        rivers_boundary, rivers, source_id, rp);
                    boundary.ReplaceElement(vertice_pos, rivers_boundary);

                    //shifting absolute vertice position of source after addition
                    for(auto&[source_id_m, source_coord_m]: sources_loc)
                        if(source_coord_m.first == boundary_id && source_coord_m.second > vertice_pos && rivers_boundary.vertices.size() > 1)
                            source_coord_m.second += rivers_boundary.vertices.size() - 1;
                }
            }
            final_boundary.Append(boundary);
        }

        if (final_boundary.lines.size() != final_boundary.vertices.size())
            throw Exception("BoundaryGenerator: Size of lines and vertices are different.");
            
        return final_boundary;
    }

    //Source: https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
    // Given three collinear points p, q, r, the function checks if
    // point q lies on line segment 'pr'
    bool onSegment(Point p, Point q, Point r)
    {
        if (q.x < max(p.x, r.x) && q.x > min(p.x, r.x) &&
            q.y < max(p.y, r.y) && q.y > min(p.y, r.y))
           return true;
    
        return false;
    }
  
    // To find orientation of ordered triplet (p, q, r).
    // The function returns following values
    // 0 --> p, q and r are collinear
    // 1 --> Clockwise
    // 2 --> Counterclockwise
    int orientation(Point p, Point q, Point r)
    {
        // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
        // for details of below formula.
        double val = (q.y - p.y) * (r.x - q.x) -
                  (q.x - p.x) * (r.y - q.y);
    
        if (val == 0.) return 0;  // collinear
    
        return (val > 0) ? 1: 2; // clock or counterclock wise
    }
  
    // The main function that returns true if line segment 'p1q1'
    // and 'p2q2' intersect.
    bool doIntersect(Point p1, Point q1, Point p2, Point q2)
    {
        auto 
            r1 = q1 - p1,
            r2 = q2 - p2;

        p1 += r1 * 5e-6;
        q1 -= r1 * 5e-6;
        p2 += r2 * 5e-6;
        q2 -= r2 * 5e-6;

        // Find the four orientations needed for general and
        // special cases
        int o1 = orientation(p1, q1, p2);
        int o2 = orientation(p1, q1, q2);
        int o3 = orientation(p2, q2, p1);
        int o4 = orientation(p2, q2, q1);
    
        // General case
        if (o1 != o2 && o3 != o4)
            return true;
    
        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == 0 && onSegment(p1, p2, q1)) return true;
    
        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == 0 && onSegment(p1, q2, q1)) return true;
    
        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == 0 && onSegment(p2, p1, q2)) return true;
    
         // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == 0 && onSegment(p2, q1, q2)) return true;
    
        return false; // Doesn't fall in any of the above cases
    }

    size_t NumOfBoundaryIntersection(const Boundary &region_boundary, const Boundary &tip_boundary)
    {
        size_t intersection_detected = 0;

        for(const auto& tip_line: tip_boundary.lines)
        {
            for(const auto& boundary_line: region_boundary.lines)
            {
                //Check for intersection
                auto p1 = tip_boundary.vertices[tip_line.p1];
                auto q1 = tip_boundary.vertices[tip_line.p2];
                auto p2 = region_boundary.vertices[boundary_line.p1];
                auto q2 = region_boundary.vertices[boundary_line.p2];
                
                //Ommit cases with self intersection. Cos every boundary includes also tip line.
                //UPD: becouse of spliting rivers there is no more self intersection.
                //But still it is good feature
                if (p1 == p2 && q1 == q2)
                    continue;
                
                if (doIntersect(p1, q1, p2, q2))
                    intersection_detected += 1;
            }
        }
        return intersection_detected;
    }

    inline double distance_from_a_point_to_a_segmen(double x1, double y1, double x2, double y2, double x, double y)
    {
        return abs((x2 - x1)*(y1 - y) - (x1 - x)*(y2 - y1))/sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
    }

    ///Distance of point to boundary.
    double DistanceFromPointToBoundary(const Boundary &boundary, const Point& point)
    {
        double distance = __DBL_MAX__;
        auto & v = boundary.vertices;
        for(auto const &l: boundary.lines)
        {
            auto cur_distance = distance_from_a_point_to_a_segmen(
                v[l.p1].x, v[l.p1].y, v[l.p2].x, v[l.p2].y, 
                point.x, point.y);
            if (cur_distance < distance)
                distance = cur_distance;
        }

        return distance;
    }

    ///Distance of points to boundary.
    double DistanceFromPointsToBoundary(const Boundary &boundary, const t_PointList& points)
    {
        double distance = __DBL_MAX__;

        for(auto const & p: points)
        {
            auto cur_distance = DistanceFromPointToBoundary(boundary, p);
            if (cur_distance < distance)
                distance = cur_distance;
        }

        return distance;
    }
}