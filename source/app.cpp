#include "app.hpp"

int App::Run(int argc, char *argv[])
{
    try
    {
        //Program options
        auto po = process_program_options(argc, argv);

        if (po.count("help") || po.count("version"))
            return 0;

        auto model = InitializeModelObject(po);

        if (model.prog_opt.verbose) cout << model << endl;

        //Triangle mesh object setup
        Triangle triangle(&model.mesh);

        //Simulation object setup
        River::Solver sim(&model);

        //MAIN LOOP
        print(model.prog_opt.verbose, "Start of main loop...");

        //forward simulation case
        if(model.prog_opt.simulation_type == 0)
        {
            print(model.prog_opt.verbose, "Forward river simulation type selected.");
            ForwardRiverSimulation river(&model, &triangle, &sim);
            river.non_linear_solver();
        }
        //Backward simulation
        else if(model.prog_opt.simulation_type == 1)
        {
            print(model.prog_opt.verbose, "Bacward river simulation type selected.");
            ForwardRiverSimulation river(&model, &triangle, &sim);
            river.backward_solver();
        }
        //test simulation
        else if(model.prog_opt.simulation_type == 2)
        {   
            print(model.prog_opt.verbose, "Test river simulation type selected.");

            //reinitialize geometry
            model.sources = model.border.MakeRectangular();

            model.tree.Initialize(model.border.GetSourcesIdsPointsAndAngles(model.sources));

            auto source_branch_id = 1;//todo
            model.tree.at(source_branch_id).AddPoint(Polar{0.1, 0});

            EvaluateSeriesParams(model, triangle, sim, model.prog_opt.output_file_name);
            Save(model, model.prog_opt.output_file_name);
        }
        //unhandled case
        else 
            throw Exception("Invalid simulation type selected: " + to_string(model.prog_opt.simulation_type));

        print(model.prog_opt.verbose, "End of main loop...");
        print(model.prog_opt.verbose, "Done.");
    }
    catch(const River::Exception& caught)
    {
        cout << "river excetpion" << endl;
        cout << caught.what() << endl;
        return 1;
    }
    catch(const dealii::ExceptionBase caught)
    {
        cout << "dealii excetpion" << endl;
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