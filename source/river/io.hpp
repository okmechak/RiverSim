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
    + `Border`(in* / out)
    + `Trees`(in* / out)
    + `GeometryDifference`(out)
    + `RuntimeInfo`(out)

    __in*__ - means that it is optional input object.
    __out__ - output contains this information.

    \note "Trees" can't be specified without `Border` object.

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

    ## Border

    Border contains information about River::Border class, which specifies border points, connections between them
    and source points.

    Border has next fields
    + `coords` - vector of border coordinates. \see River::Border::vertices
    + `lines` - vector of border lines and its id. Id handles boundary condition:
        0 - zero Direchlet, 1 - Laplacea zero flow.
        \see River::Border::lines
    + `SourceIds` - contains pairs of source id number and coordinate position number in
        "coords" vector.

    ### Example 
    
    Rectangular border with one source:

    ```json
    "Border": {
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

    Trees structure is representation of River::Tree object. This structure contains information about river 
    network geometry, its separate branches(River::BranchNew) and relationship between them.
    
    It contains next objects:

    + `Branches` - vector of `branches` objects.
    + "Relations" - vector of three element items. First number indicates source branch id,
        second - left branch id, third - right branch id.
    + "SourceIds" - holds source branches.

    ### `Branches`

    For details about `Branch` object see River::BranchNew. Each `Branch` consists of next objects:

    + "coords" - coordintates of branch. See River::BranchNew::points.
    + "sourceAngle" - direction of growth of source point branch. See River::BranchNew::source_angle.
    + "id" - unique number which is referenced by `Tree:Relations` object.

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
    
    ## GeometryDifference
    
    Obcjects: 

    + "AlongBranches" - contains information about difference allong branches.
    + "BiffuractionPoints" - contains geometry difference at biffuratcion points.

    \see River::GeometryDifference

    ## RuntimeInfo

    Object holds some general information and perfomance measurments. Like total time of simulation, 
    time of each cycle, start date, end date and input file.

    \see River::Timing

    ## [Example of JSON file output](https://github.com/okmechak/RiverSim/blob/master/research/test_tree.json)
*/

#pragma once

#include "border.hpp"
#include "physmodel.hpp"
#include "tree.hpp"
#include "cxxopts.hpp"
#include "utilities.hpp"

namespace River
{
    /*! \brief Processing of program options.
        \details for full list of program options see `.\riversim -h` or River::Model.
    */
    cxxopts::ParseResult process_program_options(int argc, char* argv[]);

    ///Initializes River::Model object by Program Options values.
    Model& SetupModelParamsFromProgramOptions(const cxxopts::ParseResult& vm, Model& mdl);

    /*! Save current state of program which includes Geometry(Tree, Border), current model parameters(Model) and backward simulation data(GeometryDifference).

      \todo pass not a file name but stream out object.
    */
    void Save(const Model& mdl, const string file_name, const string input_file = "");
    
    /*! Opens state of program from json file which includes Geometry(Tree, Border) and current model parameters(Model).

        \todo pass not a file name but stream in object.
    */
    void Open(Model& mdl, string file_name);

}//namespace River