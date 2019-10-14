function [ box_height, channelmouths] = GetInitialCondition( initial_condition )
    %GETINITIALCONDITION returns heigh and sources coords initial geometry constants
    %   Detailed explanation goes here
    box_height = 50;
    channelmouths = [];
    switch initial_condition
        case 0
            box_height = 50;
            channelmouths = [0, 0];
        case 1
            box_height = 50;
            channelmouths = [0.2, 0];
        case 2
            box_height = 50;
            channelmouths = [0, 0];
        case 3
            box_height = 50;
            channelmouths = [ [-0.25, 0]; [0.25, 0] ];
        case 4
            box_height = 50;
            channelmouths = [ [-0.75, 0]; [-0.25, 0]; [0.25,0]; [0.75,0] ];
        case 5
            box_height = 1;
            channelmouths = [0.25, 0];
        case 6
            box_height = 2;
            channelmouths = [-0.6, 0];
        otherwise
            disp('inproper value');
    end


end

