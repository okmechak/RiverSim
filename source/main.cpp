#include <iostream>
#include "riversim.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    tetgen_test();
    dealii_test();
    boost_test(argc, argv);
    gmsh_test();
    tethex_test();
    cout << "Hello" << endl;
    return 0;
}