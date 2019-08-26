function grow_mesh_streamline_implicit(river_graph, params)
    
    fprintf('Growing: Royal society streamline approximation... \n');
    
    %Numeric values for boundary conditions
    plotYRange = params.dt;

    %main simulation loop
    while (plotYRange < params.maxY)
        disp("Height - " + string(plotYRange));
        
        DrawRiver(river_graph);
        
        ForwardEvolution(river_graph, params);
        
        %getting the highest tip coord
        plotYRange = GetMaxY(river_graph.get_tips_coords());
        save('river_workspace.mat', "river_graph");
    end
end
