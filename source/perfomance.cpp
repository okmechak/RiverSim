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
        FE_Q<dim> fe(2);
        River::RiverSolver r(fe, &model);
        r.run();
    }
    catch(const River::Exception& caught)
    {
        cout << caught.what() << endl;
        return 1;
    }
    catch(...) 
    {
        cout << "GOT UNDEFINED ERROR" << endl;
        return 2;
    }
    
    return 0;
}