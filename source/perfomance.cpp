#include "river.hpp"
#include "io.hpp"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])

{
    try
    {
        //Program options
        auto vm = River::process_program_options(argc, argv);

        //Handle help option
        if (vm.count("help") || vm.count("version"))
            return 0;

        //riversim signature print
        if (!vm.count("suppress-signature") && vm.count("verbose"))
            River::print_ascii_signature();

        //autput and input files names initialization
        string output_file_name = vm["output"].as<string>(),
            input_file = 
                vm.count("input") ? input_file = vm["input"].as<string>() : "";
            
        River::Model mdl;

        //initialization mdl object
        {
            bool q_update_border = false;
            if(vm.count("input"))
                River::Open(mdl, vm["input"].as<string>(), q_update_border);

            River::SetupModelParamsFromProgramOptions(vm, mdl);//..if there are so.

            mdl.CheckParametersConsistency();

            if(mdl.prog_opt.verbose)
                cout << mdl << endl;

            if(!q_update_border)
            {
                mdl.border.MakeRectangular(
                    {mdl.width, mdl.height}, 
                    mdl.boundary_ids,
                    {mdl.dx},
                    {1});

                mdl.tree.Initialize(
                    mdl.border.GetSourcesPoint(), 
                    mdl.border.GetSourcesNormalAngle(), 
                    mdl.border.GetSourcesId());
            }
        }

        River::Triangle tria;
        tria.AreaConstrain = true;
        tria.CustomConstraint = true;
        tria.MaxTriaArea = mdl.mesh.max_area;
        tria.MinAngle = mdl.mesh.min_angle;
        tria.Verbose = false;
        tria.Quite =  true;
        tria.mesh_params = &mdl.mesh;

        //згенерувати геометрію всередині 
        //РіверСолвер і зробити цілу еволюцію там
        //
        //
        const int dim = 2;
        FE_Q<dim> fe(2);
        River::RiverSolver r(fe);
        r.run();

    }
    catch(const River::Exception& caught)
    {
        cout << caught.what() << endl;
    }
    catch(...) 
    {
        cout << "got undefined error" << endl;
        throw;
    }
    
    return 0;
}