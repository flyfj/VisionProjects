function [ output_args ] = visualize_search_res( query_fns, ranked_res_fns )
%VISUALIZE_SEARCH_RES Summary of this function goes here
%   Detailed explanation goes here

html_src = '<table>';
for i=1:size(query_fns,1)
    html_src = [html_src, '<tr><td><img src=''' query_fns{i,1} '''></td>'];
    
end



end

