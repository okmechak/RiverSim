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

/*!
    \file main.cpp
    \todo add all practical cases of usage with print screens
    \todo add shell scripts description
    \brief Entry point of program and main loop handler.
    @{
    
    Main incorporates user commands handling and managing of inner simulation objects.
    User commands are simple terminal commands(run ./riversim --help to check them all).
    It setups initial conditions and parameters. Initializes all object that are used in
    simulation like: Geometry, Mesh, Triangle and Simualtion, which are all combined
    into the loop.
    Please review code for further details
    @}
*/

/*! \mainpage Welcome

    \tableofcontents

    \section introduction Introduction
    
    __Here you can find information about RiverSim program. From detailed description to general use cases__.
   
    __RiverSim__ - is simulation of river network growth C++ program, using model based on Laplace equation.
  
    Mathematicaly, program solves Partial Differential Equation(PDE) using Finite Element Method(FEM). Solving of PDE involves setting up boundaries, mesh generation, building a linear system and finaly solution. 
  
    All these steps are done by using different open source C++ libraries: _Triangle_, _Tethex_ and  _Deal.II_, which are combined in one program - __RiverSim__.

    __Here is video demonstration of simulation:__


    \htmlonly

    <iframe 
        width="560" 
        height="315" 
        src="https://www.youtube.com/embed/nMHbZbgenwo" 
        frameborder="0" 
        allow="accelerometer; 
        autoplay; 
        encrypted-media; 
        gyroscope; 
        picture-in-picture" 
        allowfullscreen>
    </iframe>

    \endhtmlonly
    
    \section githubrepo Github Repository and Installation
    
    See [README](https://github.com/okmechak/RiverSim) on github repository for deatailed installation instruction.

    \section illustrations Illustrations
    
    This illustration represents geometrical structure of program and how geometry relates to classes,
    and how classes relates between each other.

    Main geometry classes of program are next:

    + River::Polar
    + River::Point
    + River::Boundary
    + River::BranchNew
    + River::Tree

    \imageSize{GeometryClasses.jpg, height:70%;width:70%;, }

    \section workflow Program Cycle Workflow

    Steps: 

    + First step is to initialize program. This can be accomplished specifing program options
        or input file(\ref io.hpp).
    + 2,3 initialization on model geometry and model parameters.
    + 4 - Triangulation process is started(see \ref mesh.hpp)
        Mesh and boundary files are produced.
    + 5 - PDE solver based on Deal.II library(see \ref solver.hpp)
        VTK file is produced.
    + 6 - Integration of series parameters near tip points(see River::Solver::integrate())
    + 7 - Modification of tree: adding new points to tips. In other words - river growth process.
        At this stage JSON file is produced with current river network state.
    + Proces repeats from step 4, until River::StopConditionOfRiverGrowth is fired or sucessifull
        number of steps is reached.

    \imageSize{ProgramWorkflow.jpg, height:90%;width:90%;, }

    \section futherreading Futher Reading
    
    + Take a look into [File List](files.html). Some more important for understanding files:
        + \ref experiments
        + \ref GeometryPrimitives.hpp
        + \ref boundary.hpp
        + \ref tree.hpp
        + \ref physmodel.hpp
        + \ref io.hpp
    + Next you can overview Classes from those files.
    + For any suggestions or improvments create [issue](github.com/okmechak/RiverSim/issues) on github repo

    \section basicusage Basic Program Usage

    As any Linux program, type in terminal from folder source(in your build folder) command:
    > ./riversim

    More details about parameters you can find in __help__ of program:
    > ./riversim -h
    or
    > ./riversim --help
    
    Typical forward river simulation command:
    > ./riversim -n 100 -o simdata

    And after that, typical backward simulation commnad:
    > ./riversim -n 130 -o revsim --simulation-type=1 simdata.json

    \section team Team

    + student Oleg Kmechak
    + student Stasiek Żukowski
    + prof. dr hab Piotr Szymczak


    \section references References

    [1]: [Problem odwrotny do ewolucji sieci rzecznych]().

    [2]: RiverSim repository - https://github.com/okmechak/RiverSim/

    [3]: Bifurcation dynamics of natural drainage networks Alexander P.Petroff†, Olivier Devauchelle‡, Hansjörg Seybold and Daniel H.Rothman.

    [4]: Path selection in the growth of rivers Yossi Cohen, Olivier Devauchelle, Hansjörg F. Seybold, Robert S. Yia, Piotr Szymczak, and Daniel H. Rothmana.

    [5]: Ramiﬁcation of stream networks Olivier Devauchelle, Alexander P. Petroff, Hansjörg F. Seybold, and Daniel H. Rothman.

    [6]: Shape and dynamics of seepage erosion in a horizontal granular bed Michael Berhanu, Alexander Petroﬀ, Olivier Devauchelle, Arshad Kudrolli, and Daniel H. Rothman.

    \section contacts Contacts

    Oleg Kmechak, University of Warsaw
    oleg.kmechak@gmail.com
    2019
*/

///\cond
#include <iostream>
///\endcond

#include "riversim.hpp"

using namespace River;
using namespace std;

class App
{
    public:
        int Run(int argc, char *argv[])
        {
            try
            {
                //Program options
                auto po = process_program_options(argc, argv);

                if (po.count("help") || po.count("version"))
                    return 0;

                auto model = InitializeModelObject(po);

                if (model.prog_opt.verbose) cout << model << endl;

                //Triangle mesh object setup
                Triangle triangle(&model.mesh);

                //Simulation object setup
                River::Solver sim(&model);

                //MAIN LOOP
                print(model.prog_opt.verbose, "Start of main loop...");

                //forward simulation case
                if(model.prog_opt.simulation_type == 0)
                {
                    print(model.prog_opt.verbose, "Forward river simulation type selected.");
                    ForwardRiverSimulation river(&model, &triangle, &sim);
                    river.non_linear_solver();
                }
                //Backward simulation
                else if(model.prog_opt.simulation_type == 1)
                {
                    print(model.prog_opt.verbose, "Bacward river simulation type selected.");
                    ForwardRiverSimulation river(&model, &triangle, &sim);
                    river.backward_solver();
                }
                //test simulation
                else if(model.prog_opt.simulation_type == 2)
                {   
                    print(model.prog_opt.verbose, "Test river simulation type selected.");

                    //reinitialize geometry
                    model.sources = model.border.MakeRectangular();

                    model.tree.Initialize(model.border.GetSourcesIdsPointsAndAngles(model.sources));

                    auto source_branch_id = 1;//todo
                    model.tree.at(source_branch_id).AddPoint(Polar{0.1, 0});

                    EvaluateSeriesParams(model, triangle, sim, model.prog_opt.output_file_name);
                    Save(model, model.prog_opt.output_file_name);
                }
                //unhandled case
                else 
                    throw Exception("Invalid simulation type selected: " + to_string(model.prog_opt.simulation_type));

                print(model.prog_opt.verbose, "End of main loop...");
                print(model.prog_opt.verbose, "Done.");
            }
            catch(const River::Exception& caught)
            {
                cout << "river excetpion" << endl;
                cout << caught.what() << endl;
                return 1;
            }
            catch(const dealii::ExceptionBase caught)
            {
                cout << "dealii excetpion" << endl;
                cout << caught.what() << endl;
                return 1;
            }
            catch(const cxxopts::OptionException& caught)
            {
                cout << "option excetpion" << endl;
                cout << caught.what() << endl;
                return 2;
            }
            catch(const std::exception& caught)
            {
                cout << "stl exception" << endl;
                cout << caught.what() << endl;
                return 3;
            }
            catch(...) 
            {
                cout << "GOT UNDEFINED ERROR" << endl;
                return 4;
            }

            return 0;
        }
};

/*! Main program
*/
int main(int argc, char *argv[])
{
    App app;
    return app.Run(argc, argv);
}