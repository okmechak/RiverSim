function bif_val = GetBifValue( bif_type, a1, a3)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    switch bif_type
        case 1
            bif_val = a1; 
        case 2
            bif_val = -a1/(9*a3);
        otherwise
            ME = MException('MyComponent:noSuchVariable', ...
                'Unvalid type of bifurcation: %s ', string(bif_type));
            throw(ME);
    end
end

