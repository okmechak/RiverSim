function maxY = GetMaxY( tips )
%GETMAXY Summary of this function goes here
%   Detailed explanation goes here
    coords = cell2mat(tips.values());
    maxY = max(coords(2, :));

end

