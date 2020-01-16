classdef plgs < handle
    
    properties (Access = public)
      P = [];  % points
      C = [];  % constraints between point i and point j
      B = [];  % is bifurcation point
      Names = []; %give names to points. This should help with navigation.
      BC;    % boundarycondition on constraint k
      
      nameTrigger = 0; %0 - left branch, 1 - righ
    end
    
    methods (Access = public)  
        
        %creation add and remove functionality
        
        %creation
        function obj = plgs(P, C, BC, B, Names)
            if(nargin == 5)
                obj.P = P;
                obj.C = C;
                obj.BC = BC;
                obj.B = B;
                obj.Names = Names;
            elseif (nargin == 1)
                obj.P = P.P;
                obj.C = P.C;
                obj.BC = P.BC;
                obj.B = P.B;
                obj.Names = P.Names;
            end    
        end
        
        %add
        function add_segments(obj, segs)
            for seg = segs
               obj.add_segment(seg); 
            end
        end
        
        function add_segment(obj, seg)
            obj.add_constraint(seg.Start, seg.End, seg.BoundaryCondition, seg.Name)
        end
        
        function add_biffurcation(obj, seg_left, seg_right)
            if (seg_left.Start.x ~= seg_right.Start.x && ...
                    seg_left.Start.y ~= seg_right.Start.y)
                ME = MException('MyComponent:noSuchVariable', ...
                    'Segments has different Start positions');
                throw(ME);
            end 
            bif_index = obj.get_index_by_coord(seg_left.Start);
            %now start point is a bifurcation point
            obj.B(bif_index) = 1;
            obj.add_segment(seg_left);
            obj.add_segment(seg_right);
            
            
        end
        
        %remove
        function remove_segment(obj, seg)
            %TODO
            obj.remove_constraint(obj, seg.End, seg.Name)
        end
        
        function remove_segments(obj, segs)
           %TODO 
           for seg = segs
               obj.remove_segment(obj, seg)
           end
        end
            
        %get some properties
        
        function tips_and_names = get_tips_coords(obj)
            beginnings = obj.C(:, 1);
            endings = obj.C(:, 2);
            tipsIndexes = setdiff(endings, beginnings);
            tipsNames = obj.Names(tipsIndexes);
            if size(tipsIndexes, 1) == 1
                tips_and_names = containers.Map(tipsNames, ...
                    obj.P(tipsIndexes, :)');
            else
                tips_and_names = containers.Map(tipsNames, ...
                    num2cell(obj.P(tipsIndexes, :)',1));
            end
        end
        
        function segs = get_tips_segments(obj)
            tips_constr = obj.get_tips_constraints();
            segs = [];
            for constr = tips_constr
                segs = [segs, obj.constr_to_seg(constr)];
            end 
        end
        
        function segs = get_bif_segments(obj)
            bif_coords = obj.P(obj.B);
            segs = [];
            for bif_coord = bif_coords
                index = obj.get_index_by_coord(bif_coord);
                constr = obj.get_constr_by_index(index);
                segs = [segs; obj.constr_to_seg(constr)];
            end
        end
        
        function shrink_all_branches(obj, lenghts_and_indexes)
            
            n = size(lenghts_and_indexes, 2);
            for k = 1:n
                branch_index = lenghts_and_indexes(2, k);
                len = lenghts_and_indexes(1, k);
                constr = obj.get_constr_by_name(branch_index);
                if ~isempty(constr)
                    obj.shrink_branch(constr, len);
                end
            end
        end    
    end
    
    
    methods (Access = public)
        
        function seg = constr_to_seg(obj, constr)
            bound_cond = BoundCond;
            startCoord = obj.P(constr(1), :);
            endCoord = obj.P(constr(2), :);

            seg = segment(startCoord, endCoord, ...
                bound_cond.dirichlet, obj.Names(constr(2)));
        end
        
        function add_constraint(obj, P1a, P2a, BC, Name)
             
            if isstruct(P1a)
                P1 = [P1a.x, P1a.y];
            else
                P1 = P1a;
            end
            clear P1a;
            if isstruct(P2a)
                P2 = [P2a.x, P2a.y];
            else
                P2 = P2a;
            end
            clear P2a;  
            if(~isempty(obj.P))
                IndexP1 = find(obj.P(:, 1) == P1(1) & obj.P(:, 2) == P1(2));
                IndexP2 = find(obj.P(:, 1) == P2(1) & obj.P(:, 2) == P2(2));
                % add only new points
                if(size(IndexP1, 1) == 0) 
                    obj.P = [obj.P; P1];
                    IndexP1 = size(obj.P, 1);
                    constr = obj.get_constr_by_index(IndexP1);
                    obj.Names = [obj.Names; obj.Names(constr(2))];
                    obj.B = [obj.B; 0];
                end
                if(size(IndexP2, 1) == 0)
                    obj.P = [obj.P; P2];
                    IndexP2 = size(obj.P, 1);
                    %functionality to add name
                    if obj.B(IndexP1)
                        name = obj.GenerateNewName(obj.Names(IndexP1));
                    else
                        name = obj.Names(IndexP1);
                    end
                    if (Name ~= -1)
                       name = Name;
                    end
                    obj.Names = [obj.Names; name];
                    obj.B = [obj.B; 0];
                end 
                % add only new edges
                if(size(IndexP1, 1) ~= 0 && size(IndexP2, 1) ~= 0)
                    %then  both points are in the list, so check if the
                    %constraint is already there
                    edgeij = find(obj.C(:, 1) == IndexP1 & obj.C(:, 2) == IndexP2);
                    edgeji = find(obj.C(:, 1) == IndexP2 & obj.C(:, 2) == IndexP1);
                    if(size(edgeij, 1) == 0 && size(edgeji, 1) == 0)
                        obj.C = [obj.C; [IndexP1, IndexP2]];
                        obj.BC = [obj.BC; BC];
                    end
                else
                    obj.C = [obj.C; [IndexP1, IndexP2]];
                    obj.BC = [obj.BC; BC];
                end
            else
                IndexP1 = 1;
                IndexP2 = 2;
                obj.P = [P1; P2];    
                obj.C = [IndexP1, IndexP2];	 
                obj.BC = BC;
                if (Name ~= -1)
                    obj.Names = [Name; Name];
                else
                    obj.Names = [1; 1];
                end
                obj.B = [0; 0];
            end 
        end
        
        function remove_constraint(obj, point_coord, name)
            
            if isstruct(P1a)
                point_coord = [point_coord.x, point_coord.y];
            end
            
            if(~isempty(obj.P))
                %finding of index in point array
                IndexP = obj.get_index_by_coordfind(point_coord);
                
                if (obj.Names(IndexP) ~= name)
                    ME = MException('MyComponent:noSuchVariable', ...
                        'name of target and current segments are different');
                    throw(ME);
                end
                
                if(size(IndexP, 1) ~= 0)
                    obj.remove_constraint_by_index(IndexP)
                end
            end 
        end
        
        function remove_constraint_by_index(obj, IndexP)
            % remove from poin array
            obj.P(IndexP, :) = [];


            %removing from bif array
            if(~isempty(obj.B))
                obj.B(IndexP) = [];
            end
            
            %removing from Name array
            if(~isempty(obj.Names))
                obj.Names(IndexP) = [];
            end
            
            %removing from boundary condition array
            if(~isempty(obj.BC))
                obj.BC(IndexP) = [];
            end

            if(~isempty(obj.C))
                %finding and removing from constrain array
                IndexC = find(obj.C(:, 2) == IndexP);
                obj.C(IndexC, :) = [];

                %shifting all indexes in BC array by -1 cos we removed
                %one element
                shiftMatrix = -1*(obj.C > IndexP);
                obj.C = obj.C + shiftMatrix;
            end
        end
        
        function tip = shrink_tip(obj, constr, alpha)
           seg = obj.constr_to_seg(constr);
           obj.P(constr(2), :) = obj.P(constr(1), :) + seg.direction() * alpha;
           tip = obj.P(constr(2), :);
        end
        
        function is_end_of_branch = shrink_branch(obj, constr, len)
            is_end_of_branch = 0;
            while (len > 0)
                seg = obj.constr_to_seg(constr);
                seg_len = seg.labs();
                %if lenght is less than constroaint lenght than we
                %shrink this segment by this lenght stop
                if (seg_len > len)
                    alpha = (seg_len - len) / seg_len;
                    obj.shrink_tip(constr, alpha);
                    len = 0;
                
                %if reached bifurcation point then we just
                %remove rest of brunch and stop    
                elseif obj.B(constr(1))
                    
                    obj.remove_constraint_by_index(constr(2))
                    len = 0;
                    %this point is no more bifurcation point
                    obj.B(constr(1)) = 0;
                    is_end_of_branch = 1;
                
                %remove rest of segment and continue of
                %shrinking next segment
                else
                    obj.remove_constraint_by_index(constr(2))
                    len = len - seg_len;
                    constr = obj.get_prev_constr(constr);
                end 
            end
        end
        
         function prev_constr = get_prev_constr(obj, constr)
            index = obj.C(:, 2) == constr(1);
            prev_constr = obj.C(index, :);       
        end
        
        function tips_constr_and_name = get_tips_constraints(obj)
            beginnings = obj.C(:, 1);
            endings = obj.C(:, 2);
            tipsIndexes = transpose(setdiff(endings, beginnings));
            
            tips_constr_and_name = zeros(3, size(tipsIndexes, 2));
            
            for k = 1:size(tipsIndexes, 2)
                tip = tipsIndexes(k);
                index = (obj.C == tip);
                newIndex = logical(fliplr(index) + index);
                
                tips_constr_and_name([1, 2], k) = obj.C(newIndex);
                
            end 
            
            tips_constr_and_name(3, :) = obj.Names(tips_constr_and_name(2, :));
        end
        
        function val = is_bif_point(obj, index)
            if index <= size(obj.B)
                val = obj.B(index);
            else
                val = (sum(obj.C == index) == 2);
            end
        end
        
        function index = get_index_by_coord(obj, point_coord)
            index = find(obj.P(:, 1) == point_coord.x &...
                obj.P(:, 2) == point_coord.y);
        end
        
        function constr = get_constr_by_index(obj, index)
            constr_index = obj.C(:, 2) == index;
            constr = obj.C(constr_index, :);
        end
        
        function constr = get_constr_by_name(obj, name)
            tips_constr_and_name = obj.get_tips_constraints();
            pos = tips_constr_and_name == name;
            constr = transpose(tips_constr_and_name([1, 2], pos(3, :)));
        end
        
        function name_number = GenerateNewName(obj, base_name)
            name_number = 2 * base_name + obj.nameTrigger;
            if obj.nameTrigger == 0
                obj.nameTrigger = 1;
            else
                obj.nameTrigger = 0;
            end
        end
    end
end