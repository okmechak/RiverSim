function dt = Triangulator_more_regular(river_graph, filename)
%dt=DelaunayTri(graph.P(:,1),graph.P(:,2), graph.C);

    net_size = 0.1;
    minx = min(river_graph.P(:, 1)) + net_size / 2;
    maxx = max(river_graph.P(:, 1)) - net_size / 2;
    miny = min(river_graph.P(:, 2)) + net_size * sqrt(3);
    maxy = max(river_graph.P(:, 2)) - net_size * sqrt(3);
    nx = round((maxx - minx) / net_size) + 1;
    ny = round((maxy - miny) / (net_size * sqrt(3))) + 1;

    if mod(ny, 2) == 0
        net1 = repmat(linspace(minx + net_size/2, maxx - net_size/2, nx - 1),...
            ny/2, 1);
        net1 = net1(:);
        net1 = [net1 repmat(linspace(miny + (net_size*sqrt(3)), maxy, ny/2)',...
            nx - 1, 1)];
        net2 = repmat(linspace(minx, maxx, nx), ny/2, 1);
        net2 = net2(:);
        net2 = [net2 repmat(linspace(miny, maxy - (net_size*sqrt(3)), ny/2)',...
            nx, 1)];
        net = [net1; net2];
    else
        net1 = repmat(linspace(minx, maxx, nx),(ny + 1)/2, 1);
        net1 = net1(:);
        net1 = [net1 repmat(linspace(miny, maxy,(ny + 1)/2)', nx, 1)];
        net2 = repmat(linspace(minx + net_size/2, maxx - net_size/2, nx - 1), (ny-1)/2,1);
        net2 = net2(:);
        net2 = [net2 repmat(linspace(miny+(net_size*sqrt(3)), ...
            maxy - (net_size*sqrt(3)), (ny - 1)/2)', nx - 1, 1)];
        net = [net1; net2];
    end
    
    regular_graph = plgs(river_graph);

    for i = 1:5
        line = abs(regular_graph.P(regular_graph.C(1,2), :) -...
            regular_graph.P(regular_graph.C(1,1), :));

        if (line(1) < line(2))
            n = round(line(2)/net_size/sqrt(3)) + 1;
        else
            n = round(line(1)/net_size) + 1;
        end

        if (n > 2)
            x = linspace(regular_graph.P(regular_graph.C(1, 1), 1),...
                regular_graph.P(regular_graph.C(1, 2), 1), n);
            y = linspace(regular_graph.P(regular_graph.C(1, 1), 2),...
                regular_graph.P(regular_graph.C(1, 2), 2), n);
            x(1) = [];
            x(end) = [];
            y(1) = [];
            y(end) = [];
            points = size(regular_graph.P, 1);
            regular_graph.P = [regular_graph.P; [x' y']];
            regular_graph.BC = [regular_graph.BC; repmat(regular_graph.BC(1), n - 1, 1)];
            regular_graph.C = [regular_graph.C; [[regular_graph.C(1, 1) (points + 1):(points + n - 2)]; ...
                [(points + 1):(points + n - 2) regular_graph.C(1, 2)]]'];
            regular_graph.C(1, :) = [];
            regular_graph.BC(1) = [];
        end
    end

    dt = delaunayTriangulation([regular_graph.P; net], regular_graph.C);
    fid = fopen(filename, 'w');
    % number of Vertices, number of triangles, Number of edges
    %InsideTriangles=dt(dt.inOutStatus(),:);
    %nvc=size(dt.X,1);
    nvc = size(dt.Points, 1);
    nt = size(dt.ConnectivityList, 1);
    nb = size(dt.Constraints, 1); % boundary edges

    % write the mesh file
    fprintf(fid, '%s\n', [int2str(nvc), ' ', int2str(nt), ' ', int2str(nb)]);
    fclose(fid);
    dlmwrite(filename, [dt.Points, zeros(size(dt.Points,1), 1)], '-append', 'delimiter', '\t', 'precision', 6);
    dlmwrite(filename, [dt.ConnectivityList, zeros(size(dt.ConnectivityList, 1), 1)],...
               '-append', 'delimiter', '\t');
    dlmwrite(filename, [regular_graph.C, regular_graph.BC], '-append', 'delimiter', '\t');
end