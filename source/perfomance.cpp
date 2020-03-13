#include "river/river.hpp"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        const int dim = 2;
        FE_Q<dim> fe(2);
        River::RiverSolver r(fe);

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