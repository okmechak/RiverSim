#include "boundary_generator.hpp"

namespace River
{
    /**
        Tree Vertices Generation
        \todo chandge parameter order
    */
    void TreeVertices(vector<Point> &tree_vertices, unsigned id, const Tree& tr, double eps)
    {
        if (tr.empty())
            throw Exception("Boundary generator: trying to generate tree boundary from empty tree.");

        const auto& br = tr.at(id);
        vector<Point> left_vector, right_vector;
        left_vector.reserve(br.size());
        right_vector.reserve(br.size());

        for(unsigned i = 0; i < br.size(); ++i)
        {
            if(i == 0)
            {
                left_vector.push_back(
                    br.GetPoint(i) + Point{Polar{eps/2, br.SourceAngle() + M_PI/2}});
                right_vector.push_back(
                    br.GetPoint(i) + Point{Polar{eps/2, br.SourceAngle() - M_PI/2}});
            }
            else
            {
                left_vector.push_back(
                    br.GetPoint(i) + br.Vector(i).normalize().rotate(M_PI/2)*eps/2);
                right_vector.push_back(
                    br.GetPoint(i) + br.Vector(i).normalize().rotate(-M_PI/2)*eps/2);
            }
        }

        if(tr.HasSubBranches(id))
        {
            auto[left_b_id, right_b_id] = tr.GetSubBranchesIds(id);
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
            tree_vertices.push_back(br.GetPoint(br.size() - 1));
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
        SimpleBoundary final_boundary;
        auto sources = model.sources;

        for(auto [boundary_id, simple_boundary]: model.border)
        {
            for(const auto&[source_id, value]: sources)
            {
                auto boundary_id_s = value.first;
                auto vertice_pos = value.second;
                if(boundary_id == boundary_id_s)
                {
                    auto tree_boundary = TreeBoundary(model.tree, source_id, model.river_boundary_id, model.mesh.eps);
                    simple_boundary.ReplaceElement(vertice_pos, tree_boundary);

                    //shifting absolute vertice position of source after addition
                    for(auto&[source_id_m, value_m]: sources)
                        if(value_m.first == boundary_id && value_m.second > vertice_pos && tree_boundary.vertices.size() > 1)
                            value_m.second += tree_boundary.vertices.size() - 1;
                }
            }
            final_boundary.Append(simple_boundary);
        }

        //hotfix. first index of last line element isn't shifted by lenght of last added tree.
        auto last_pos = final_boundary.lines.size() - 1;
        if(last_pos >= 2)
            final_boundary.lines.at(last_pos).p1 = final_boundary.lines.at(last_pos - 1).p2;

        if (final_boundary.lines.size() != final_boundary.vertices.size())
            throw Exception("Size of lines and vertices are different.");
            
        return final_boundary;
    }
}