function series_params_container = BackwardEvolution( river_graph, params)
%BACKEVOLUTION Summary of this function goes here
%Detailed explanation goes here
    
    series_params_container = [];
    for k = 1:params.steps
        if RunFreeFem(river_graph)
            fprintf('FreeFem++ FAILED\n');
            return;
        end

        %TODO create some structure with names and pass it to
        %shrink_all_branches
        [dr_table, series_params] = LoadFreeFemResults(...
            params.freefem_output_name, params.dt, params.theta );
        
        river_graph.shrink_all_branches(dr_table); 
        
        series_params_container = [series_params_container; series_params];
    end
end

