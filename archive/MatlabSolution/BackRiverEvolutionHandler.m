addpath('Functions');

load("params_file_name.mat", "params");

%previous results names
params.river_graph_file_name = "river_workspace.mat";
params.river_graph_var_name = "river_graph";
params.freefem_output_name = "freefem_output.dat";
params.steps = 3;
params.allow_bif = false;

%model numeraical params
%params.length = 0.01; % initial length of rivers
%params.dt = 0.2;

%TODO add continue functionality

%%%%%%%%%%%%%%%%
%%MAIN PROGRAM%%
%%%%%%%%%%%%%%%%
disp("Running of simluation with next configuration");
disp(params);

%sending some parameters data to freeFem
fid = fopen('FEM_settings.txt', 'w');
fprintf(fid, '%d\n%d', params.equation_type, params.mesh_adaptation);
fclose(fid);

%generation of object of intital graph
load(params.river_graph_file_name, params.river_graph_var_name);

disp("initial river graph:");
disp(river_graph);
orig_river_graph = plgs(river_graph);
DrawRiver(river_graph);
pause(3);

DA = DataAgregator();

%Main loop of simulation
stop_condition = true;
for theta = 0:0.1:2
    params.theta = theta;
    river_graph = plgs(orig_river_graph);
    %while stop_condition
    disp("theta = " + string(theta));
    DA.SetCurrentPower(theta);
    for k = 1:30
        disp("k = " + string(k));
        A = river_graph.get_tips_coords();
        series_params = BackwardEvolution(river_graph, params);
        B = river_graph.get_tips_coords();

        forward_river_graph = plgs(river_graph);
        ForwardEvolution(forward_river_graph, params);
        C = forward_river_graph.get_tips_coords();

        DA.AddSeriesParams(series_params);
        DA.AddGeometryParams(A, B, C);

        DrawRiver(forward_river_graph);

        %stop condition
        %when exist only one branch == only one biff point
        %stop_condition = sum(river_graph.B) >= 1;
    end
    DA.Save();
end
