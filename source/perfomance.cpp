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
            
        auto model = River::InitializeModelObject(vm);

        //згенерувати геометрію всередині 
        //РіверСолвер і зробити цілу еволюцію там
        //
        //
        const int dim = 2;
        River::RiverSolver<dim> r(&model);
        r.one_step_forward();
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
    catch(const exception& caught)
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