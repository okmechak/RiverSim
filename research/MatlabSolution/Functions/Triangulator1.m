function dt=Triangulator1(graph, filename)
%dt=DelaunayTri(graph.P(:,1),graph.P(:,2), graph.C);
dt = delaunayTriangulation(graph.P, graph.C);
fid = fopen(filename, 'w');
% number of Vertices, number of triangles, Number of edges
%InsideTriangles=dt(dt.inOutStatus(),:);
%nvc=size(dt.X,1);
nvc = size(dt.Points,1);
nt = size(dt.ConnectivityList,1);
nb = size(dt.Constraints,1); % boundary edges

% write the mesh file
fprintf(fid,'%s\n',[int2str(nvc),' ',int2str(nt),' ',int2str(nb)]);
fclose(fid);
dlmwrite(filename,[dt.Points,zeros(size(dt.Points,1),1)],'-append','delimiter','\t','precision', 6);
dlmwrite(filename,[dt.ConnectivityList,zeros(size(dt.ConnectivityList,1),1)],...
		   '-append','delimiter','\t');
dlmwrite(filename,[graph.C,graph.BC], '-append','delimiter','\t');
