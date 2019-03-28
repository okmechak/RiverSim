#include "utilities.hpp"

namespace River
{

string ProgramTitle = "RiverSim 2018";

void print_ascii_signature()
{
    cout << endl;
    cout << "     _)                    _)           " << endl;
    cout << "  __| |\\ \\   / _ \\  __| __| | __ `__ \\  " << endl;
    cout << " |    | \\ \\ /  __/ |  \\__ \\ | |   |   | " << endl;
    cout << "_|   _|  \\_/ \\___|_|  ____/_|_|  _|  _| " << endl;         
    cout << endl;            
}


void print_version()
{
    cout << ProgramTitle+ " version: 0.8.9" << endl;
}


cxxopts::ParseResult process_program_options(int argc, char *argv[])
{
    using namespace cxxopts;
     
    Options options(
        "riversim", ProgramTitle + " is used in my master work to simulate river growth according to \nLaplace model(for more details pls see references)");

    //declare supported options
    options.add_options()
    ("h,help", "produce help message")
    ("v,version", "print version string")
    ("suppress-signature", "suppress signature printing")
    ("f,input-mesh", "input mesh file", value<string>())
    ("o,output-mesh", "save output mesh", value<string>()->default_value("out_mesh.msh"))
    ("V,verbose", "print detailed log to terminal")
    ("Q,quiet", "print detailed log to terminal")
    ("G,use-gmsh", "Mesh Generator: false(default) - Triangle, true - Gmsh")
    ("s,simulate", "Run simulation in Deal.II", value<bool>()->default_value("true"))
    ("D,dev-mode", "Suppress standart flow", value<bool>()->default_value("false"))
    ("d,draw-mesh", "draw mesh using gmsh fltk submodule", value<bool>()->default_value("true"))
    ("Z,visualise", "Run GMSH gui and visualise current mesh", value<bool>()->default_value("true"))
    ("m,mesh", "Turn on/off mesh generation", value<bool>()->default_value("true"))
    ("n,number-of-steps", "Number of steps to simulate", value<unsigned>()->default_value("3"))
    ("ds", "ds - minimal lenght of growing", value<double>()->default_value("0.1"))
    ("dx", "dx - shift of initial river position from beginning of coordinates", value<double>()->default_value("0.5"))
    ("A,mesh-max-area", "constraints maximal area of triangle element", value<double>()->default_value("-1."))
    ("q,mesh-min-angle", "constraints minimal angle of triangle element", value<double>()->default_value("-1."))
    ("e,eps", "Width of branch", value<double>()->default_value("0.000001"))
    ("r,ref-num", "Number of solver adaptive refinments", value<int>()->default_value("3"))
    ("g,geom-type", "Geometry type: 0 - Simple Box, 1 - Simple River, 2 - Single Tip", value<int>()->default_value("0"))
    ("t,test-flag", "Test flag for development purposes");

    auto result = options.parse(argc, argv);

    if (result.count("help"))
        cout << options.help({""}) << endl;

    if (result.count("version"))
        print_version();

    return result;
}

} // namespace River