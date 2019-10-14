function R = GetRotTipsMatrix( tips )
%GETROTTIPSMATRIX Returns rotation matrixes for each tip in global system
%   none
   
    for k = 1:size(tips, 2)
        angle = pi/2 -...
            cart2pol(tips(k).End.x - tips(k).Start.x, tips(k).End.y - tips(k).Start.y);
        R{k} = [cos(angle), sin(angle); ...
               -sin(angle), cos(angle)];
    end
end

