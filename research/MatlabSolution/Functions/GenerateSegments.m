function [newtips, newbiffs] = GenerateSegments( tips, distance, bif_cond)
%GENERATESEGMENTS Summary of this function goes here
%   Detailed explanation goes here

    %Numeric values for boundary conditions
    bound_cond = BoundCond;

    angle = pi/5;
    matrixLeft = [ cos(angle) -sin(angle); ...
                    sin(angle) cos(angle) ];
    matrixRight = [ cos(angle) sin(angle);...
                   -sin(angle) cos(angle) ];

    newtips = [];
    newbiffs = [];
    for k = 1:size(tips, 2)
        dX = distance(k, :);
        dX = dX';
        if bif_cond(k)
            left_seg = segment([tips(k).End.x, tips(k).End.y],...
                       [tips(k).End.x, tips(k).End.y] + (matrixLeft*dX)',...
                       bound_cond.dirichlet);

            right_seg = segment([tips(k).End.x, tips(k).End.y],...
                       [tips(k).End.x, tips(k).End.y] + (matrixRight*dX)',...
                       bound_cond.dirichlet);

            newbiffs = [newbiffs, [left_seg; right_seg]];
        else
            tipsegment = segment([tips(k).End.x, tips(k).End.y],...
                       [tips(k).End.x, tips(k).End.y]+dX',...
                       bound_cond.dirichlet);
            newtips = [newtips tipsegment];
        end
    end
end
