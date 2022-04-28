/*
    riversim - river growth simulation.
    Copyright (c) 2019 Oleg Kmechak
    Report issues: github.com/okmechak/RiverSim/issues

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.
*/

/*! \file io.hpp
    \brief Contains functionality to read and write state of program(simulation data) and program options.

    \details
    There is few options to communicate with the program.
    First one is program options, like `./riversim --number-of-steps=10` etc. To see
    full list of options and its description see `./riversim --help`.
    Second one option is using json file. Json file produced by the program can be used as input.
    Save/Open parameters of model, border and tree to/from file.

    ## json file format
    json file contains next objects:

    + `Model`(in* / out)
    + `Boundary`(in* / out)
    + `Trees`(in* / out)
    + `GeometryDifference`(out)
    + `RuntimeInfo`(out)

    __in*__ - means that it is optional input object.
    __out__ - output contains this information.

    \note "Trees" can't be specified without `Boundary` object.

    Lets describe each of them.

    ## Model

    Model object contains all simulation model parameters. They are well descibed in `riversim --help`.

    Model consist of three objects and fields. Fields description you can find here River::Model.

    + `Integration` - parameters used for integration  around river tips.
        \see River::IntegrationParams
    + `Mesh` - parameters used for mesh generation and densening triangle size around river tips.
        \see River::MeshParams
    + `Solver` - parameters used by Deal.II solver.
        \see River::SolverParams
    

    ### Example
    
    All model parameters(version 2.4.1)

    ```json
    "Model": {
        "Integration": {
            "exponant": 2.0,
            "radius": 0.03,
            "weightRadius": 0.01
        },
        "Mesh": {
            "eps": 1e-06,
            "exponant": 7.0,
            "maxArea": 100000.0,
            "maxEdge": 1.0,
            "minAngle": 30.0,
            "minArea": 7e-08,
            "minEdge": 8e-08,
            "ratio": 2.3,
            "refinmentRadius": 0.04,
            "sigma": 1.9,
            "staticRefinmentSteps": 3
        },
        "Solver": {
            "adaptiveRefinmentSteps": 0,
            "iterationSteps": 6000,
            "quadratureDegree": 3,
            "refinmentFraction": 0.1,
            "tol": 1e-12
        },
        "bifurcationAngle": 0.6283185307179586,
        "bifurcationMinDistance": 0.05,
        "bifurcationThreshold": -0.1,
        "bifurcationThreshold2": 0.001,
        "bifurcationType": 0,
        "boundaryCondition": 0,
        "boundaryIds": [
            1,
            2,
            3,
            4
        ],
        "ds": 0.003,
        "dx": 0.2,
        "eta": 1.0,
        "fieldValue": 1.0,
        "growthMinDistance": 0.01,
        "growthThreshold": 0.0,
        "growthType": 0,
        "height": 1.0,
        "riverBoundaryId": 4,
        "width": 1.0
    }
    ```

    \note 
    If some fieds of Model Object are ommited then they will be replaced by default values.
    Even empty Model can be specified:
    ```json
    Model: {}
    ```

    ## Boundary

    Boundary contains information about River::Boundary class, which specifies border points, connections between them
    and source points.

    Boundary has next fields
    + `coords` - vector of border coordinates. \see River::Boundary::vertices
    + `lines` - vector of border lines and its id. Id handles boundary condition:
        0 - zero Direchlet, 1 - Laplacea zero flow.
        \see River::Boundary::lines
    + `SourceIds` - contains pairs of source id number and coordinate position number in
        "coords" vector.

    ### Example 
    
    Rectangular border with one source:

    ```json
    "Boundary": {
        "SourceIds": [
            [ 1, 4 ]
            ],
        "coords": [
            [ 1.0, 0.0 ],
            [ 1.0, 1.0 ],
            [ 0.0, 1.0 ],
            [ 0.0, 0.0 ],
            [ 0.2, 0.0 ]
        ],
        "lines": [
            [ 0, 1, 1 ],
            [ 1, 2, 2 ],
            [ 2, 3, 3 ],
            [ 3, 4, 4 ],
            [ 4, 0, 4 ]
        ]
    }
    ```

    ## Trees

    Trees structure is representation of River::Rivers object. This structure contains information about river 
    network geometry, its separate branches(River::Branch) and relationship between them.
    
    It contains next objects:

    + `Branches` - vector of `branches` objects.
    + "Relations" - vector of three element items. First number indicates source branch id,
        second - left branch id, third - right branch id.
    + "SourceIds" - holds source branches.

    ### `Branches`

    For details about `Branch` object see River::Branch. Each `Branch` consists of next objects:

    + "coords" - coordintates of branch. See River::Branch::points.
    + "sourceAngle" - direction of growth of source point branch. See River::Branch::source_angle.
    + "id" - unique number which is referenced by `Rivers:Relations` object.

    #### Branch Example

    ```json
    {
        coords: [
            [ 0.15392550378286424, 0.8574775565643402 ],
            [ 0.15385809127830358, 0.8576768672025086 ],
            [ 0.15379076781691314, 0.8578758571295332 ]
        ],
        "id": 9,
        "sourceAngle": 1.8695242272923212,
        "sourcePoint": [ 0.15392550378286424, 0.8574775565643402 ]
    }
    ```
    \note 
    `sourcePoint` and first element of `coords` are the same.

    ### Example 

    ```json
    "Trees": {
        "Branches": [{<branch1>, <branch2>, ... <branchN>}],
        "Relations": "Relations": [
            [ 1, [ 2, 3 ] ],
            [ 2, [ 4, 5 ] ],
            [ 3, [ 6, 7 ] ]
        ],
        "SourceIds" : [1]
    }
    ```

    ## [Example of JSON file output](https://github.com/okmechak/RiverSim/blob/master/research/test_tree.json)
*/

#pragma once

#include "model.hpp"
#include "cxxopts.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace River
{
    ///Program title.
    string ProgramTitle();
    
    ///Prints to console program logo.
    void print_ascii_signature();

    ///Returns version string defined in \ref version.hpp.
    string version_string();

    ///Prints to console program Version.
    void print_version();

    ///Prints logs depending of log configuration(quiet or verbose)
    void print(const bool flag, const string str);

    /*! \brief Processing of program options.
        \details for full list of program options see `.\riversim -h` or River::Model.
    */
    cxxopts::ParseResult process_program_options(int argc, char* argv[]);

    /*! \brief Global program options. 
        \details Program has some options that isn't part simulation itself but rather ease of use.
        So this object is dedicated for such options.
     */
    class ProgramOptions
    {
        public:

            ///Outputs VTK file of Deal.II solution
            bool save_vtk = false;

            bool save_each_step = false;

            ///If true - then program will print to standard output.
            bool verbose = true;

            ///If true - then program will save additional output files for each stage of simulation.
            bool debug = false;

            string output_file_name = "simdata",
                input_file_name = "";
            
            ///Prints program options structure to output stream.
            friend ostream& operator <<(ostream& write, const ProgramOptions & po);

            bool operator==(const ProgramOptions& po) const;
    };

    ///Initialize model values based on input(from console) program options.
    Model getModel(const cxxopts::ParseResult& vm);

    ///Initialize program options object values based on input(from console) program options.
    ProgramOptions getProgramOptions(const cxxopts::ParseResult& vm);

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

    void to_json(json& j, const Region& region); 
    void from_json(const json& j, Region& region); 

    void to_json(json& j, const Sources& sources); 
    void from_json(const json& j, Sources& sources); 

    void to_json(json& j, const Branch& branch); 
    void from_json(const json& j, Branch& branch); 

    void to_json(json& j, const Rivers& tree);
    void from_json(const json& j, Rivers& tree);

    void to_json(json& j, const SeriesParameters& id_series_params);
    void from_json(const json& j, SeriesParameters& id_series_params);

    void to_json(json& j, const BackwardData& data);
    void from_json(const json& j, BackwardData& data);

    void to_json(json& j, const ProgramOptions& data);
    void from_json(const json& j, ProgramOptions& data);

    void to_json(json& j, const MeshParams& data);
    void from_json(const json& j, MeshParams& data);

    void to_json(json& j, const IntegrationParams& data);
    void from_json(const json& j, IntegrationParams& data);

    void to_json(json& j, const SolverParams& data);
    void from_json(const json& j, SolverParams& data);

    void to_json(json& j, const Model& data);
    void from_json(const json& j, Model& data);
}//namespace River