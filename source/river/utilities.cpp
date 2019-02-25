#include "utilities.hpp"

namespace River
{


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
    cout << "RiverSim  version: 0.8.9" << endl;
}



    
po::variables_map process_program_options(int argc, char *argv[])
{
    //Declaring group of options that will be allowed on both
    //command line and config file
    po::options_description config(
        "River Sim 2018. Program is used in my master work to simulate river growth according to \nLaplace model(for more details pls see references)");

    //declare supported options
    config.add_options()
    ("help,h", "produce help message")
    ("version,v", "print version string")
    ("input-mesh,f", po::value<vector<string>>(), "input mesh file")
    ("suppress-signature", "suppress signature printing")
    ("output-mesh,o", po::value<string>()->default_value("out_mesh.msh"), "save output mesh")
    ("Verbose,V", po::value<bool>()->default_value(false), "print detailed log to terminal")
    ("Quiet,Q", po::value<bool>()->default_value(false), "print detailed log to terminal")
    ("MeshMaxArea,A", po::value<double>()->default_value(-1.), "constraints maximal area of triangle element") //FIXME.. some errors appears: faile d
    ("MeshMinAngle,q", po::value<double>()->default_value(-1.), "constraints minimal angle of triangle element")
    ("Eps,e", po::value<double>()->default_value(0.000001), "Width of branch") //conversion using boost::any_cast
    ("ref-num,r", po::value<int>()->default_value(3), "Number of solver adaptive refinments")
    ("geom-type,g", po::value<int>()->default_value(0), "Geometry type: 0 - Simple Box, 1 - Simple River, 2 - Single Tip")
    ("simulate,s", po::value<bool>()->default_value(true), "Run simulation in Deal.II")


    po::positional_options_description p;
    p.add("input-mesh", -1);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, config), vm);
    po::notify(vm);

    if (vm.count("help"))
        cout << config << endl;

    if (vm.count("version"))
        print_version();

    return vm;
}

} // namespace River