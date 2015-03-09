function [query_feats, query_cate_ids] = prepare_depth_query( totrain, feat_type )
% prepare query
% each category uses 10 queries

query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
new_sz = [64 64];
query_cates = {'banana'; 'coffee_mug'; 'bowl'; 'keyboard'; 'food_box'};
cate_ids = [6, 21, 11, 41, 16];
query_num_per_cate = 20;

disp('preparing query depth...');

if totrain == 1
%     query_fns = cell(length(query_cates)*query_num_per_cate, 1);
    query_feats = cell(0);
    query_cate_ids = [];
    
    cnt = 1;
    for j=1:length(query_cates)
        cur_cate_root = [query_dir query_cates{j} '\'];
        cur_cate_subs = dir([cur_cate_root '*']);
        cur_cate_subs(1:2) = [];
        
        % list all images under subfolders
        all_fns = [];
        tmp = randperm(length(cur_cate_subs));
        for k=tmp(1):tmp(1)
            % query files
            cur_cate_sub_dir = [cur_cate_root cur_cate_subs(k).name '\'];
            fns = dir([cur_cate_sub_dir '*_depthcrop.png']);
            all_fns = [all_fns; fns];
        end
        
        % randomly select queries
        if strcmp(feat_type, 'hog')
            obj_feats = zeros(query_num_per_cate, 8*8*31);
        elseif strcmp(feat_type, 'pixel')
            obj_feats = zeros(query_num_per_cate, newsz(1)*newsz(2));
        end
        ids = randperm(length(all_fns));
        sel_fns = all_fns(ids(1:query_num_per_cate));
        for i=1:query_num_per_cate
            cur_qfn = [cur_cate_sub_dir sel_fns(i).name];
            sep_loc = strfind(sel_fns(i).name, '_depthcrop');
            % use color image for results
            %query_fns{cnt,1} = [cur_cate_sub_dir sel_fns(i).name(1:sep_loc-1) '_crop.png'];
            maskfn = [cur_cate_sub_dir sel_fns(i).name(1:sep_loc-1) '_maskcrop.png'];
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
            
            if strcmp(feat_type, 'hog')
                % compute hog
                cellSize = 8;
                hog = vl_hog(single(curimg), cellSize);
                obj_feats(i, :) = hog(:)';
            elseif strcmp(feat_type, 'pixel')
                obj_feats(i,:) = curimg(:) ./ 255;
            end
            
            query_feats = [query_feats; obj_feats(i,:)];
            query_cate_ids = [query_cate_ids; cate_ids(j)];
            
        end

    end

    save('query_data.mat', 'query_feats', 'query_cate_ids', '-v7.3');
else
    tmp = load('query_data.mat');
    query_feats = tmp.query_feats;
    query_cate_ids = tmp.query_cate_ids;
    
end

disp('query depth ready.');

