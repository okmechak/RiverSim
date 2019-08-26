addpath('Functions');

params = struct(); 

params.params_file_name = "params_file_name.mat";
%load previous results
%0 - no
%1 - yes
params.load_prev_res = 0;

%power of growth from 0 to Infinity
params.theta = 1;

% initial_condition can have 5 values:
% 0 - rectangle 2x50 with one centered tip
% 1 - rectangle 2x50 with one not centered tip
% 2 - rectangle 2x50 with one bifurcated tip
% 3 - rectangle 2x50 with one two symmetrical tips
% 4 - rectangle 2x50 with one four symmetrical tips
% 5 - square 2x2 with one centered tip
% 6 - square 2x2 with one not centered tip
params.init_cond = 5;

% bifurcation can have 3 values:
% 0 - no bifurcation
% 1 - bifurcation after reaching certain a1 value (bifurcation_treshold)
% 2 - bifurcation after reaching certain a3/a1 value (bifurcation_treshold)
params.bif_type = 1;

% 0.8 recommended for bifurcation = 1 
% 0.23 recommended for Poisson
% 0.03 recommended for bifurcation = 2
params.bif_treshold = 0.8;
params.allow_bif = false;

% method can have 2 values:
% 0 - solving Laplace equation
% 1 - solving Poisson equation
params.equation_type = 1;

% adaptation can have 2 values:
% 0 - without field adaptation
% 1 - with field adaptation
params.mesh_adaptation = 1;

% method can have 2 values:
% 0 - Royal growth
% 1 - maximal flow direction
params.growth_method = 0;

%num of steps between draws
params.steps = 3;


%model numeraical params
params.length = 0.1; %initial length of rivers
params.t_i = 0;    % <- this variable is not used in current version of code...
params.dt = 0.01;
params.maxY = 0.5;
params.saveplots = true;




%%%%%%%%%%%%%%%%
%%MAIN PROGRAM%%
%%%%%%%%%%%%%%%%
disp("Running of simluation with next configuration");
disp(params);
save(params.params_file_name, "params");

%sending some parameters data to freeFem
fid = fopen('FEM_settings.txt', 'w');
fprintf(fid, '%d\n%d', params.equation_type, params.mesh_adaptation);
fclose(fid);

%generation of object of intital graph
if (~params.load_prev_res)
    river_graph = GenerateInitialGraphAndTips(params.length, params.init_cond);
else
    load('river_workspace.mat', "river_graph");
    load(params.params_file_name, "params");
end

disp("initial river graph:");
disp(river_graph);
DrawRiver(river_graph);
pause(3);

%Main loop of simulation
grow_mesh_streamline_implicit(river_graph, params);