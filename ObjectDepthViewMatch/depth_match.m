%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

img_dir = 'F:\3D\ModelNet\test_db_depth2\';
db_dir = 'F:\3D\ModelNet\depth_hog\';
query_dir = 'E:\Datasets\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
query_list = 'uw_query.txt';
new_sz = [64 64];

%% prepare query
% each category uses 10 queries
query_cates = {'banana'; 'coffee_mug'; 'bowl'; 'keyboard';};
query_num_per_cate = 10;
query_fns = cell(length(query_cates)*query_num_per_cate, 1);
query_hog = zeros(length(query_fns), 1984);
cnt = 1;
for j=1:length(query_cates)
    cur_cate_root = [query_dir query_cates{j} '\'];
    cur_cate_subs = dir([cur_cate_root '*']);
    for k=1:length(cur_cate_subs)
        if cur_cate_subs(k).isdir == 0 || ...
            strcmp(cur_cate_subs(k).name, '.') == 1 || ...
            strcmp(cur_cate_subs(k).name, '..') == 1 
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
            query_hog(cnt, :) = hog(:)';
            % increase
            cnt = cnt + 1;
        end
        
        break;  % only use one sub category
    end
    
end


%% HOG L2

db_hog = load('db_hog.mat');
db_hog = db_hog.img_hogs;
img_fns = load('db_fns.mat');
img_fns = img_fns.img_files;
img_fns = img_fns.';
% append root dir
for i=1:length(img_fns)
    img_fns{i} = [img_dir img_fns{i}];
end

%% test

ranked_res_fns = cell(length(query_fns), size(img_fns,2));
rank_scores = zeros(length(query_fns), length(img_fns));
for i=1:length(query_fns)
    qhog = query_hog(i,:);
    qhog_mat = repmat(qhog, size(db_hog,1), 1);
    dists = abs(qhog_mat - db_hog).^2;
    dists = sum(dists, 2);
    dists = sqrt(dists);
    [Y,I] = sort(dists, 1);
    rank_scores(i,:) = Y;
    ranked_res_fns(i,:) = img_fns(1,I);
    disp(num2str(i));
end
%%
 visualize_search_res('res_bruteforce.html', query_fns, ranked_res_fns, 50);


%% 