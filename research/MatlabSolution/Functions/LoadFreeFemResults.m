function [dr_table, series_params] = LoadFreeFemResults( file_name, dt, theta )
%LOADFREEFEMRESULTS Summary of this function goes here
%   Detailed explanation goes here
    tip_growth_matrix = load(file_name);
    dr_table = dt * tip_growth_matrix(:, 1).^theta;
    dr_table = [dr_table, tip_growth_matrix(:, 5)]';
    series_params = tip_growth_matrix(:, [1,2,3]);
end