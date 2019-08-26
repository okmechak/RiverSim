function river_graph = GenerateInitialGraphAndTips(length, init_cond)
    
    
    [box_height, channelmouths] = GetInitialCondition( init_cond );
    
    %Numeric values for boundary conditions
    bound_cond = BoundCond;
    
    edge_name = 0;
    
    %lenght of wall to left and right
    wall_l = 0; 
    wall_r = 1;

    URC = point_m(wall_r, box_height);%Upper right corner
    LLC = point_m(wall_l, 0);%lower left corner
    Box = Box1_temp(LLC, URC);%simulation region data structure
    
    river_graph = plgs();
    Bottompoints = [
        [Box.LLC.x, Box.LLC.y];
        channelmouths;
        [Box.LRC.x, Box.LRC.y]
        ];
    
    
    %joining and adding edge condition at bottom
    for i = 1:size(Bottompoints, 1) - 1
        P1 = Bottompoints(i, :);
        P2 = Bottompoints(i + 1, :);
        seg = segment(P1, P2, bound_cond.dirichlet, edge_name);
        river_graph.add_segment(seg);
    end
        
    %rest of outer edges
    right_seg = segment(Box.LRC, Box.URC, bound_cond.neumann, edge_name);
    top_seg = segment(Box.URC, Box.ULC, bound_cond.constantflux, edge_name);
    left_seg = segment(Box.ULC, Box.LLC, bound_cond.neumann, edge_name);
    river_graph.add_segments([right_seg, top_seg, left_seg]);

    %adding some lenght to initial sources
    start_segment_name = 1;
    switch init_cond
        case 2
            %bifurcated initial condition
            
            %this is initial segment
            
            river_graph.add_segment(segment([0, 0], [0, 0.5], ...
                bound_cond.dirichlet, start_segment_name));
            
            left_seg = segment(channelmouths + [0, 0.5], channelmouths + ...
                [0, 0.5] + [sin(pi/5), cos(pi/5)] * length, bound_cond.dirichlet);
            right_seg = segment(channelmouths + [0, 0.5], channelmouths +...
                [0, 0.5] + [-sin(pi/5), cos(pi/5)] * length, bound_cond.dirichlet);
            
            river_graph.add_biffurcation(left_seg, right_seg);
        otherwise
            for n = 1:size(channelmouths, 1)
                seg = segment(channelmouths(n, :), channelmouths(n,:) +...
                    [0, length], bound_cond.dirichlet, start_segment_name);
                river_graph.add_segment(seg);
            end
    end
end