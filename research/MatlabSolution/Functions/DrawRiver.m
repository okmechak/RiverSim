function DrawRiver( river_graph )
%INITIALDRAW Some initial
%   Detailed explanation goes here

    tips = river_graph.get_tips_coords();
    plotYRange = GetMaxY(tips);
    
    dims = [0 1 0 max([plotYRange + 0.2, 1])];
    
    plot(river_graph.P(river_graph.C'), river_graph.P(river_graph.C' +...
        size(river_graph.P, 1)), '.-', 'Color', 'b', 'LineWidth', 2);
    axis(dims);
    
   coords = cell2mat(tips.values());
   keys = cell2mat(tips.keys());
    for k = 1:size(coords, 2)
        text(coords(1, k), coords(2, k), char(string(keys(k))));
    end
    drawnow;
    
end

