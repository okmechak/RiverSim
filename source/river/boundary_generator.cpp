#include "boundary_generator.hpp"

namespace River
{
    /**
        Tree Vertices Generation
        \todo chandge parameter order
    */
    void TreeVertices(vector<Point> &tree_vertices, unsigned id, const Tree& tr, double eps)
    {
        if (tr.Empty())
            throw Exception("Boundary generator: trying to generate tree boundary from empty tree.");

        const auto br = tr.GetBranch(id);
        vector<Point> left_vector, right_vector;
        left_vector.reserve(br->Size());
        right_vector.reserve(br->Size());

        for(unsigned i = 0; i < br->Size(); ++i)
        {
            if(i == 0)
            {
                left_vector.push_back(
                    br->GetPoint(i) + Point{Polar{eps/2, br->SourceAngle() + M_PI/2}});
                right_vector.push_back(
                    br->GetPoint(i) + Point{Polar{eps/2, br->SourceAngle() - M_PI/2}});
            }
            else
            {
                left_vector.push_back(
                    br->GetPoint(i) + br->Vector(i).normalize().rotate(M_PI/2)*eps/2);
                right_vector.push_back(
                    br->GetPoint(i) + br->Vector(i).normalize().rotate(-M_PI/2)*eps/2);
            }
        }

        if(tr.HasSubBranches(id))
        {
            auto[left_b_id, right_b_id] = tr.GetSubBranchesId(id);
            tree_vertices.insert(end(tree_vertices), 
                left_vector.begin(), left_vector.end() - 1);
            TreeVertices(tree_vertices, left_b_id, tr, eps);
            tree_vertices.pop_back();
            TreeVertices(tree_vertices, right_b_id, tr, eps);
            tree_vertices.insert(end(tree_vertices), 
                right_vector.rbegin() + 1, right_vector.rend());
        }
        else
        {
            tree_vertices.insert(end(tree_vertices), 
                left_vector.begin(), left_vector.end() - 1);
            tree_vertices.push_back(br->GetPoint(br->Size() - 1));
            tree_vertices.insert(end(tree_vertices), 
                right_vector.rbegin() + 1, right_vector.rend());
        }
    }
    
    SimpleBoundary TreeBoundary(const Tree& tree, unsigned source_id, int boundary_id, double eps)
    {
        SimpleBoundary tree_boundary;
        tree_boundary.name = "Tree id = " + to_string(source_id);
        tree_boundary.inner_boundary = false;
        
        TreeVertices(tree_boundary.vertices, source_id, tree, eps);

        auto vertices_num = tree_boundary.vertices.size();
        if(vertices_num > 1)
        {
            tree_boundary.lines.reserve(vertices_num - 1);
            
            for(size_t i = 0; i < vertices_num - 1; ++i)
            {
                tree_boundary.lines.push_back(
                    Line(i, i + 1, boundary_id));
            }
        }

        return tree_boundary;
    }

    SimpleBoundary SimpleBoundaryGenerator(const Model& model)
    {
        SimpleBoundary boundary;

        for(auto simple_boundary: model.boundary.simple_boundaries)
        {
            for(const auto&[source_id, vertice_pos]: simple_boundary.sources)
            {
                auto tree_boundary = TreeBoundary(model.trees, source_id, model.river_boundary_id, model.mesh.eps);
                simple_boundary.ReplaceElement(vertice_pos, tree_boundary);
            }
            boundary.Append(simple_boundary);
        }

        if (boundary.lines.size() != boundary.vertices.size())
            throw Exception("Size of lines and vertices are different.");
            
        return boundary;
    }

    tethex::Mesh BoundaryGenerator(const Model& model)
    {
        auto boundary  = SimpleBoundaryGenerator(model);

        vector<tethex::Point> tet_vertices, tet_holes;
        vector<tethex::MeshElement *> tet_lines, tet_triangles;

        tet_vertices.reserve(boundary.vertices.size());
        tet_lines.reserve(boundary.lines.size());
        for(size_t i = 0; i < boundary.vertices.size(); ++i)
        {
            tet_vertices.push_back(boundary.vertices.at(i));
            tet_lines.push_back(new tethex::Line(
                boundary.lines.at(i).p1, 
                boundary.lines.at(i).p2, 
                boundary.lines.at(i).boundary_id));
        }
        
        auto holes = model.boundary.GetHolesList();
        tet_holes.reserve(holes.size());
        for(auto & hole: holes)
            tet_holes.push_back(hole); 
        
        return tethex::Mesh{tet_vertices, tet_lines, tet_triangles, tet_holes};
    }
}