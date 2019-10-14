function status = RunFreeFem(river_graph)
%RUNFREEFEM Make mesh and export data to FreeFem and run simulation
%   Detailed explanation goes here

    meshFileName = 'Mesh.msh';
    tipFileName = 'freefem_input.dat';
    
    Triangulator_more_regular(river_graph, meshFileName);
    writetippos(river_graph.get_tips_segments(), tipFileName); %write to freefem_input.dat

    %run of freeFem simulation
    [status, cmdout] = system('FreeFem++ load_and_solve_laplace.edp');
    if (status ~= 0)
        disp(cmdout);
    end
end