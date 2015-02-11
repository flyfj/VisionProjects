function accu = comp_accu( query_fns, res_fns, topK, opt )
%COMP_ACCU Summary of this function goes here
%   compute matching accuracy
 
accu = 0;
for i=1:length(query_fns)
    parts = strsplit(query_fns{i}, '\');
    query_cate = parts{6};
    for k=1:topK
        if opt == 1
            % for result names
            tmp = strsplit(res_fns{i,k}, '__');
            res_cate = tmp{1};
        else
            % for result files
            parts = strsplit(res_fns{i,k}, '\');
            tmp = strsplit(parts{5}, '__');
            res_cate = tmp{1};
        end

        if strcmp(res_cate, 'coffee_pot') || strcmp(res_cate, 'mug') || strcmp(res_cate, 'cup')
            res_cate = 'coffee_mug';
        end
        if strcmp(res_cate, 'box')
            res_cate = 'food_box';
        end
        if strcmp(query_cate, res_cate) == 1
            accu = accu + 1;
            break;
        end
    end
end

accu = accu / length(query_fns);

end

