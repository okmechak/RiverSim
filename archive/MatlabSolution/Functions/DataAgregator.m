classdef DataAgregator < handle
    %DATAAGREGATOR Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        file_name = "simulation_data.mat";
        series_params = {};
        geometry_params = {};
        power_param = [];    
        cur_index = 1;
    end
    
    methods
        
        function SetCurrentPower(obj, power)
            %checking is current power unique or not
            obj.cur_index = find(obj.power_param == power);
            if isempty(obj.cur_index)
                obj.power_param = [obj.power_param; power];
                obj.cur_index = size(obj.power_param, 1);
            end
        end
        
        function AddSeriesParams(obj, new_series_params)
            if size(obj.series_params, 2) < obj.cur_index
                %initialization of data structure
                obj.series_params{obj.cur_index} = new_series_params;
            else
                obj.series_params{obj.cur_index} = ...
                    [obj.series_params{obj.cur_index}; new_series_params];
            end
        end
        
        function AddGeometryParams(obj, A, B, C)
            %TODO
            n = size(A, 1);
            temp = [];
            for key = cell2mat(A.keys())
                %new keys should exist and new points should be different
                if isKey(B, key) && isKey(C, key)
                    if any(B(key) ~= C(key)) && any(A(key) ~= B(key))
                        temp = [temp; obj.ProcessGeometry(A(key), B(key), C(key))];
                    end
                end
            end
            
            if size(obj.geometry_params, 2) < obj.cur_index
                %initialization of data structure
                obj.geometry_params{obj.cur_index} = temp;
            else
                obj.geometry_params{obj.cur_index} = ...
                    [obj.geometry_params{obj.cur_index}; temp];
            end
        end
        
        function res = ProcessGeometry(obj, A, B, C)
            %TODO
            V1 = [A(1) - B(1); A(2) - B(2)];
            V2 = [C(1) - C(1); A(2) - B(2)];
            V3 = [A(1) - C(1); A(2) - C(2)];
            alpha = subspace(V1, V2);
            ds = abs(norm(V1) - norm(V2));
            dl = norm(V3);
            res = [ds, dl, alpha];
        end
        
        function PlotStatistic(obj)
            [x, firs_quantile, mean_quantile, last_quantile] = ...
                obj.GetGeomPlotData("ds");
            plot(x, firs_quantile, x, mean_quantile, x, last_quantile);
            pause(3);
            [x, firs_quantile, mean_quantile, last_quantile] = ...
                obj.GetGeomPlotData("dl");
            plot(x, firs_quantile, x, mean_quantile, x, last_quantile);
            pause(3);
            [x, firs_quantile, mean_quantile, last_quantile] =...
                obj.GetGeomPlotData("alpha");
            plot(x, firs_quantile, x, mean_quantile, x, last_quantile);
        end
        
        function [x, firs_quantile, mean_quantile, last_quantile] = GetGeomPlotData(obj, index)
            switch index
                case "ds"
                    index = 1;
                case "dl"
                    index = 2;
                case "alpha"
                    index = 3;
            end
           x = obj.power_param;
           firs_quantile = [];
           mean_quantile = [];
           last_quantile = [];
           for k = 1:size(x, 1)
               geomtry_params = obj.geometry_params{k};
               ds = geomtry_params(:, index);
               
               firs_quantile = [firs_quantile, quantile(ds, 0.25)];
               mean_quantile = [mean_quantile, mean(ds)];
               last_quantile = [last_quantile, quantile(ds, 0.75)];
           end
        end
        
        function Save(obj)
           series_params = obj.series_params;
           geometry_params = obj.geometry_params;
           power_param = obj.power_param;
           save(obj.file_name, 'series_params', 'geometry_params',...
               'power_param'); 
        end
        
        function Load(obj)
           load(obj.file_name, 'series_params', 'geometry_params',...
               'power_param'); 
           obj.series_params = series_params;
           obj.geometry_params = geometry_params;
           obj.power_param = power_param; 
        end
        
    end
    
end

