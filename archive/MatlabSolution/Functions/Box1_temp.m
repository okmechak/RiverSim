function B = Box1_temp(P1,P2)
    %description of river growth field 
    B.LLC = P1;
    B.URC = P2;
    B.LRC = point_m(B.URC.x, B.LLC.y);
    B.ULC = point_m(B.LLC.x, B.URC.y);
    B.Width = B.LRC - B.LLC;
    B.Height = B.ULC - B.LLC;
%     end
% end
%   