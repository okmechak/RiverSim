classdef segment
    
    properties
        Start;
        End;
        BoundaryCondition;
        Name = -1;
    end
    
    methods
        function S = segment(V1, V2, B, Name)
            if (nargin == 4 && isfloat(V1) == 1 && isfloat(V2) == 1 && ...
                    length(V1) == 2 && length(V2) == 2) 
                S.Start.x = V1(1);
                S.Start.y = V1(2);
                S.End.x = V2(1);
                S.End.y = V2(2);
                S.BoundaryCondition = B;
                S.Name = Name;
                
            elseif(nargin == 3 && isfloat(V1) == 1 && isfloat(V2) == 1 && ...
                    length(V1) == 2 && length(V2) == 2) 
                S.Start.x = V1(1);
                S.Start.y = V1(2);
                S.End.x = V2(1);
                S.End.y = V2(2);
                S.BoundaryCondition = B;
                
            elseif(nargin == 4 && isa(V1, 'point') && isa(V2, 'point'))
                S.Start = V1; 
                S.End = V2; 
                S.BoundaryCondition = B;
                S.Name = Name;
                
            elseif(nargin == 3 && isa(V1, 'point') && isa(V2, 'point'))
                S.Start = V1; 
                S.End = V2; 
                S.BoundaryCondition = B;
                
            elseif (nargin == 2 && isfloat(V1) == 1 && isfloat(V2) == 1 && ...
                    length(V1) == 2  && length(V2) == 2) 
                S.Start.x = V1(1); 
                S.Start.y = V1(2);
                S.End.x = V2(1);
                S.End.y = V2(2);
                S.BoundaryCondition = 0;
                
            elseif (nargin == 4 && isa(V1, 'point_m') && isa(V2, 'point_m'))
                S.Start.x = V1.x;
                S.Start.y = V1.y;
                S.End.x = V2.x;
                S.End.y = V2.y;
                S.BoundaryCondition = B;
                S.Name = Name;
                
            elseif (nargin == 3 && isa(V1, 'point_m') && isa(V2, 'point_m'))
                S.Start.x = V1.x;
                S.Start.y = V1.y;
                S.End.x = V2.x;
                S.End.y = V2.y;
                S.BoundaryCondition = B;
                
            else
                S.Start = point; 
                S.End = point;
                S.BoundaryCondition = 0;
            end        
        end
        
        function l = labs(S)
            l = norm([S.End.x - S.Start.x, S.End.y - S.Start.y]);
        end
        
        function v = direction(S)
            v = [S.End.x - S.Start.x, S.End.y - S.Start.y];
        end
        
    end
end
