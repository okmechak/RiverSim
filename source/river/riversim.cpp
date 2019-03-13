#include "riversim.hpp"
namespace River
{
    namespace SimpleGeo{

        void AddBranches(Geometry &riverGeom, int id, double angle = 0.01, int size = 20, double dl = 0.1)
        {

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{dl, angle, id}/*relative angle*/);

            if(size == 2)
                return;
            auto[idLeft, idRight] = riverGeom.AddBiffurcation(id, dl);
            AddBranches(riverGeom, idLeft, angle, size - 1);
            AddBranches(riverGeom, idRight, angle, size - 1);

        }

        Geometry CustomRiverTree(Model &model, int size)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            int rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            AddBranches(riverGeom, rootBranchID, 0.001, size, model.ds);
            return riverGeom;
        }

        Geometry Box(Model &model)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            return riverGeom;
        }
        
        Geometry SingleTip(Model &model, int size)
        {
            auto riverGeom = Geometry();
            riverGeom.SetEps(model.eps);
            riverGeom.SetSquareBoundary(
                {0., 0.}, 
                {model.width, model.height}, model.dx);

            auto rootBranchID = 1;
            riverGeom.initiateRootBranch(rootBranchID);

            for (int i = 0; i < size; ++i)
                riverGeom.addPolar(GeomPolar{model.ds, 0, rootBranchID}/*relative angle*/);

            return riverGeom;
        }
    }

    int ForwardRiverEvolution(cxxopts::ParseResult &vm)
    {
            /**
            Definition of all main objects used in project
        */
        ///Model parameters
        Model model;
        model.ds = vm["ds"].as<double>();
        model.dx = vm["dx"].as<double>();
        model.eps = vm["eps"].as<double>();

        tethex::Mesh meshio;
        Geometry geom;
        Triangle tr;
        River::Solver sim;


        /**
                Physical Model Object
            */


        /*
                    Geometry Object
            */
        switch(vm["geom-type"].as<int>())
        {   
            case 0: 
                geom = SimpleGeo::Box(model);
                break;
            case 1:
                geom = SimpleGeo::SingleTip(model, 3);
                break;
            case 2:
                geom = SimpleGeo::CustomRiverTree(model, 3);
                break;
        }

        /**
             *****Main
             *****Program
             *****Cycle
             **/
        River::Gmsh Gmsh;
        for(int i = 0; i < vm["steps"].as<int>(); ++i)
        {
            cout << endl 
                 << "==========" << endl;
            cout << "step - " << i << endl;
            cout << "==========" << endl << endl;

            geom.InitiateMesh(meshio);

            /**
                            Mesh Generation
                       */
                  
            if(vm["mesh"].as<bool>())
            {
                if(vm["use-gmsh"].as<bool>())
                {
                    Gmsh.init();
                    Gmsh.generate(meshio);
                    Gmsh.write(vm["output-mesh"].as<string>());
                }
                else
                {
                    //options
                    tr.Verbose = vm["verbose"].as<bool>();
                    tr.Quite = vm["quiet"].as<bool>();
                    tr.AreaConstrain = tr.ConstrainAngle = true;
                    tr.MaxTriaArea = vm["mesh-max-area"].as<double>();
                    tr.MinAngle = vm["mesh-min-angle"].as<double>();
                    //generate mesh
                    tr.generate(meshio);
                    //convert triangles to quadrangles
                    meshio.convert();
                    meshio.write(vm["output-mesh"].as<string>());
                }
            }
            else
                meshio.write(vm["output-mesh"].as<string>());

            if(vm["verbose"].as<bool>())
                meshio.info();


            /**
                            Solver
                             */
            if(vm["simulate"].as<bool>())
            {
                //Deal.II library
                sim.numOfRefinments = vm["ref-num"].as<int>();
                sim.SetBoundaryRegionValue({Geometry::Markers::Bottom, Geometry::Markers::River}, 0.);
                sim.SetBoundaryRegionValue({Geometry::Markers::Top}, 1.);
                //sim.SetMesh(meshio);
                sim.OpenMesh(vm["output-mesh"].as<string>());
                sim.run(i);

                for(auto p: geom.GetTipPoints())
                {
                    cout << endl << endl;
                    cout << p.first << ", angle : " << p.second << endl;
                    auto series_params = sim.integrate(p.first, p.second);

                    for(auto a: series_params)
                        cout << a << " ";
                    cout << endl;
                    auto newPoint = model.next_point(series_params);
                    newPoint.branchId = p.first.branchId;
                    cout << newPoint.branchId << endl << flush;
                    geom.addPolar(newPoint);
                    cout << endl << endl;
                }
                //freeup resources
                sim.clear();
            }

            /**
                                      Freeup resources
                             */
            geom.clear();
            meshio.clean();
            if(vm["use-gmsh"].as<bool>())
                Gmsh.clear();

        }

        /**
                       Visualisation
                   */
        if(vm["visualise"].as<bool>())
        {
            Gmsh.open(vm["output-mesh"].as<string>());
            Gmsh.start_ui();
        }
    }

}   