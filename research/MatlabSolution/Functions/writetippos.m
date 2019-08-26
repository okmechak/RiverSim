function writetippos(tips, TipFileName)
    if(~isempty(tips))
        for i = 1:size(tips, 2)  
         A(i,:) = [tips(i).End.x, tips(i).End.y,...
             cart2pol(tips(i).End.x - tips(i).Start.x, tips(i).End.y - tips(i).Start.y),...
             tips(i).Name];
        end
        dlmwrite(TipFileName, size(tips, 2), 'delimiter', '\t');
        dlmwrite(TipFileName, A, 'delimiter', '\t', 'precision', 12, '-append');
    end