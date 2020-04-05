#include "mesh.hpp"
#include "physmodel.hpp"
#include "io.hpp"
#include "tethex.hpp"
#include "boundary_generator.hpp"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        //Program options
        auto vm = River::process_program_options(argc, argv);

        if (vm.count("help") || vm.count("version"))
            return 0;

        cout << "Creation of Model object.." << endl;
        auto model = River::InitializeModelObject(vm);
        cout << model << endl;

        River::print(model.prog_opt.verbose, "BoundaryGenerator call..");
        auto boundary = River::SimpleBoundaryGenerator(model);
        cout << boundary << endl;

        River::print(model.prog_opt.verbose, "Convertion to tethex mesh object");
        auto mesh = tethex::Mesh(boundary);
        mesh.info();
        mesh.statistics();
        mesh.write(model.prog_opt.output_file_name + "_boundary.msh");
        
        //Triangle mesh object setup
        River::print(model.prog_opt.verbose, "Creation of Traingle object..");
        River::Triangle triangle(&model.mesh);

        River::print(model.prog_opt.verbose, "Mesh generation..");
        triangle.generate(mesh);

        River::print(model.prog_opt.verbose, "Write mesh to file: " + model.prog_opt.output_file_name + ".msh");
        mesh.write(model.prog_opt.output_file_name + ".msh");

        River::print(model.prog_opt.verbose, "Done.");
    }
    catch(const River::Exception& caught)
    {
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