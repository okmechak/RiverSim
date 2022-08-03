#pragma once

#include "model.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace River
{
    ///Holds for each tip id its series parameters.
    typedef map<t_branch_id, vector<double>> t_ids_series_params;
    
    ///Save current state of program which includes Geometry(Rivers, Boundary), current model parameters(Model) and backward simulation data(GeometryDifference).
    void Save(const Model& model, const string file_name);
    
    ///Opens state of program from json file which includes Geometry(Rivers, Boundary) and current model parameters(Model).
    void Open(Model& model, const string file_name);

    void to_json(json& j, const Point& p);
    void from_json(const json& j, Point& p);

    void to_json(json& j, const Polar& p); 
    void from_json(const json& j, Polar& p);

    void to_json(json& j, const BoundaryCondition& bc);
    void from_json(const json& j, BoundaryCondition& bc);

    void to_json(json& j, const BoundaryConditions& bc);
    void from_json(const json& j, BoundaryConditions& bc);

    void to_json(json& j, const Line& line);
    void from_json(const json& j, Line& line);

    void to_json(json& j, const Boundary& boundary); 
    void from_json(const json& j, Boundary& boundary); 

    void to_json(json& j, const RegionParams& region_params); 
    void from_json(const json& j, RegionParams& region_params); 

    void to_json(json& j, const Region& region); 
    void from_json(const json& j, Region& region); 

    void to_json(json& j, const Sources& sources); 
    void from_json(const json& j, Sources& sources); 

    void to_json(json& j, const Branch& branch); 
    void from_json(const json& j, Branch& branch); 

    void to_json(json& j, const Rivers& tree);
    void from_json(const json& j, Rivers& tree);

    void to_json(json& j, const MeshParams& data);
    void from_json(const json& j, MeshParams& data);

    void to_json(json& j, const IntegrationParams& data);
    void from_json(const json& j, IntegrationParams& data);

    void to_json(json& j, const SolverParams& data);
    void from_json(const json& j, SolverParams& data);

    void to_json(json& j, const Model& data);
    void from_json(const json& j, Model& data);

     ///Prints any object
    template<typename T> string print(T object)
    {
        std::ostringstream stream;
        stream.precision(16);

        json j = object;

        stream << setw(4) << j;
        return stream.str();
    }
}//namespace River