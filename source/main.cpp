#include <iostream>
#include "riversim.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    dealii_test();
    boost_test();
    gmsh_test();
    tethex_test();
    cout << "Hello" << endl;
    return 0;
}