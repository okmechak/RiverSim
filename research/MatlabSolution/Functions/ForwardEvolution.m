function ForwardEvolution(river_graph, params)
%FORWARDEVOLUTION Summary of this function goes here
%   Detailed explanation goes here
    for k = 1:params.steps
        %Run FreeFem
        if RunFreeFem(river_graph)
            fprintf('FreeFem++ FAILED\n');
            return;
        end
        
        [distance, bif_cond] = GetRiverTipsdX(river_graph.get_tips_segments(),... 
            params.theta, params.bif_type, params.bif_treshold, params.dt,...
            'freefem_output.dat');
        
        if ~params.allow_bif
            bif_cond = logical(bif_cond * 0);
        end
        
        [newtips, newbiffs] = GenerateSegments( river_graph.get_tips_segments(),...
            distance, bif_cond );
        
        %adding new tips
        river_graph.add_segments(newtips);
        for bif_tip = newbiffs
            river_graph.add_biffurcation(bif_tip(1), bif_tip(2));
        end
        
    end
end

