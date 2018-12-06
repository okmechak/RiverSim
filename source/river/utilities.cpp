#include "utilities.hpp"

namespace River
{


void printAsciiSignature()
{
    cout << endl;
    cout << "     _)                    _)           " << endl;
    cout << "  __| |\\ \\   / _ \\  __| __| | __ `__ \\  " << endl;
    cout << " |    | \\ \\ /  __/ |  \\__ \\ | |   |   | " << endl;
    cout << "_|   _|  \\_/ \\___|_|  ____/_|_|  _|  _| " << endl;         
    cout << endl;            
}


    
po::variables_map processProgramOptions(int argc, char *argv[])
{
    //Declaring group of options that will be allowed on both
    //command line and config file
    po::options_description config(
        "River Sim 2018. Program is used in my master work to simulate river growth according to \nLaplace model(for more details pls see references)");

    //declare supported options
    config.add_options()
    ("help,h", "produce help message")
    ("version,v", "print version string")
    ("test,t", po::value<string>(), "test option")
    ("sw1,s", po::value<int>()->default_value(0), "custom switch between different flows")
    ("sw2", po::value<int>()->default_value(1), "custom switch between different flows")
    ("sw2", po::value<int>()->default_value(3), "custom switch between different flows")
    ("b1,b", "custom switch between different flows")
    ("b2", "custom switch between different flows")
    ("b3", "custom switch between different flows")
    ("input-mesh,f", po::value<vector<string>>(), "input mesh file")
    ("suppress-signature", "suppress signature printing")
    ("draw-mesh,d", po::value<bool>()->default_value(true), "draw mesh using gmsh fltk submodule")
    ("output,o", po::value<string>()->default_value("out_mesh.mesh"), "save output mesh")
    ("gmsh-log", po::value<bool>()->default_value(false), "print Gmsh log to terminal")
    ("Verbose,V", po::value<bool>()->default_value(false), "print detailed log to terminal")
    ("Quiet,Q", po::value<bool>()->default_value(false), "print detailed log to terminal")
    ("sm", "suppress mesh")("ss", "suppress solver");

    po::positional_options_description p;
    p.add("input-mesh", -1);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, config), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << config << endl;
    }

    return vm;
}

} // namespace River