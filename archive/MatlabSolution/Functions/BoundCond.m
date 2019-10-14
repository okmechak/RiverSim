
classdef BoundCond
    %BOUNDARYCOND boundary condition constant values
    %   used as struct to use as names for boundary condition
    
    properties
        none = 0;
        dirichlet = 1;
        neumann = 2;
        constantflux = 3;
    end
end