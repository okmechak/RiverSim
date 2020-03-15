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
            
        River::Model model;

        //initialization mdl object
        
        if(vm.count("input"))
        {
            model.prog_opt.input_file_name = vm["input"].as<string>();
            River::Open(model);
        }

        River::SetupModelParamsFromProgramOptions(vm, model);//..if there are so.

        model.CheckParametersConsistency();

        if(model.border.vertices.empty())
        {
            model.border.MakeRectangular(
                {model.width, model.height}, 
                model.boundary_ids,
                {model.dx},
                {1});

            model.tree.Initialize(
                model.border.GetSourcesPoint(), 
                model.border.GetSourcesNormalAngle(), 
                model.border.GetSourcesId());
        }

        //згенерувати геометрію всередині 
        //РіверСолвер і зробити цілу еволюцію там
        //
        //
        const int dim = 2;
        FE_Q<dim> fe(2);
        River::RiverSolver r(fe, &model);
        r.run();

    }
    catch(const River::Exception& caught)
    {
        cout << caught.what() << endl;
    }
    catch(...) 
    {
        cout << "GOT UNDEFINED ERROR" << endl;
        throw;
    }
    
    return 0;
}