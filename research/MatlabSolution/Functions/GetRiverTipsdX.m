function [distance, bif_cond] = GetRiverTipsdX(tips, eta, bif_type,...
    bif_threshold, dt, fileName )
%GETRIVERTIPDX Summary of this function goes here
%   Detailed explanation goes here
    
    %loading output from FreeFem++
    %'freefem_output.dat'
    tip_growth_matrix = load(fileName);
    
    %rotation matrix of tips in global(not sure) coord system
    R = GetRotTipsMatrix( tips );
    
    n = size(tip_growth_matrix, 1);
    distance = zeros(n, 2);
    dr_table = dt * tip_growth_matrix(:, 1).^eta;
    bif_cond = zeros(n, 1); 
    moves = zeros(n, 1);
    %stream line method of growth direction
    for k = 1:n
        disp("")
        a1 = tip_growth_matrix(k, 1);
        a2 = tip_growth_matrix(k, 2);
        a3 = tip_growth_matrix(k, 3);
        disp("some params")
        disp(tip_growth_matrix)
        disp([a1,a2,a3])
        beta = a1 / a2;
        dr = dr_table(k);
        %dr = dt * dr / max(dr_table); %??? why is it?? -> %variable time step
        y = (beta^2/9) * (( 27*dr/(2* beta^2) + 1)^(2/3) - 1 );
        x = sign(a2)*2*sqrt( (y^3 / beta^2) + (y^4 / beta^4));
        dX = [x; y];

        %finding distance in global coordinate system
        dX = R{k}*dX;
        distance(k,:) = dX';%last vector of river growth

        %bif_type calculation
        %growth_cond = dr > dt/20;%TODO make it more general
        bifurcate = BifConditionCheck(bif_type, bif_threshold, a1, a3);

        bif_cond(k) = bifurcate;
    end
end

