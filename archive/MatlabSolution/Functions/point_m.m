% Object for 2d Points 
classdef point_m
    properties
        x=0;
        y=0;
    end
    
    methods
        function P = point_m(x,y)
            if (nargin == 2)
                P.x = x;  
                P.y = y;
            else
                P.x = 0;
                P.y = 0;
            end
        end
	    
        function d = minus(P1,P2)   % calculates the distance
            d = [P1.x - P2.x, P1.y - P2.y];
        end
	    
        function P = plus(P1, v)     % translates P1 by v
            P = point_m(P1.x + v(1), P1.y + v(2));
        end
	    
        function C = coordinates(P) % returns the [x y] coordinates of a point 
            C = [P.x P.y];
        end
        
        function b = eq(P1, P2)      % checks if two points are equal
            if(P1.x == P2.x && P1.y == P2.y)
              b=1;
            else
              b=0;
            end
        end
    end
end