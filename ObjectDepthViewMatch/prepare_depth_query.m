function [query_feats, query_fns] = prepare_depth_query(totrain, query_dir)
% prepare query
% each category uses 10 queries

new_sz = [64 64];
query_cates = {'banana'; 'coffee_mug'; 'bowl'; 'keyboard'; 'food_box'};
query_num_per_cate = 20;

disp('preparing query depth...');

if totrain == 1
    query_fns = cell(length(query_cates)*query_num_per_cate, 1);
    query_feats = zeros(length(query_fns), 1984);
    cnt = 1;
    for j=1:length(query_cates)
        cur_cate_root = [query_dir query_cates{j} '\'];
        cur_cate_subs = dir([cur_cate_root '*']);
        start = 0;
        for k=1:length(cur_cate_subs)
            if cur_cate_subs(k).isdir == 0 || ...
                strcmp(cur_cate_subs(k).name, '.') == 1 || ...
                strcmp(cur_cate_subs(k).name, '..') == 1 
                continue;
            end
            if start==0
                start = 1;
                continue;
            end
            % query files
            cur_cate_sub_dir = [cur_cate_root cur_cate_subs(k).name '\'];
            fns = dir([cur_cate_sub_dir '*_depthcrop.png']);
            ids = randperm(length(fns));
            fns = fns(ids(1:query_num_per_cate));
            for i=1:length(fns)
                cur_qfn = [cur_cate_sub_dir fns(i).name];
                sep_loc = strfind(fns(i).name, '_depthcrop');
                % use color image for results
                query_fns{cnt,1} = [cur_cate_sub_dir fns(i).name(1:sep_loc-1) '_crop.png'];
                maskfn = [cur_cate_sub_dir fns(i).name(1:sep_loc-1) '_maskcrop.png'];
                maskimg = imread(maskfn);
                maskimg = maskimg > 0;
                curimg = imread(cur_qfn);
                curimg = mat2gray(curimg);
                % apply mask
                curimg = curimg .* maskimg;
                curimg = uint8(curimg .* 255);
                % crop
                stats = regionprops(maskimg, 'Area', 'BoundingBox');
                stats(1).BoundingBox
                curimg = imcrop(curimg, stats(1).BoundingBox);
                curimg = imresize(curimg, new_sz);
                % compute hog
                cellSize = 8;
                hog = vl_hog(single(curimg), cellSize, 'verbose');
                query_feats(cnt, :) = hog(:)';
                % increase
                cnt = cnt + 1;
            end

            break;  % only use one sub category
        end

    end

    save('query_fns.mat', 'query_fns');
    save('query_hog.mat', 'query_feats');
else
    query_fns = load('query_fns.mat');
    query_fns = query_fns.query_fns;
    query_feats = load('query_hog.mat');
    query_feats = query_feats.query_feats;
  
end

disp('query depth ready.');

