%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

addpath('E:\Projects\Github\VisionToolbox\Common\Matlab\');

db_dir = 'F:\3D\ModelNet\test_db_depth2\';
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
new_sz = [64 64];

%% prepare query
% each category uses 10 queries
query_cates = {'banana'; 'coffee_mug'; 'bowl'; 'keyboard'; 'food_box'};
query_num_per_cate = 20;
if ~exist('query_fns', 'var') || ~exist('query_hog', 'var')
    query_fns = cell(length(query_cates)*query_num_per_cate, 1);
    query_hog = zeros(length(query_fns), 1984);
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
                query_hog(cnt, :) = hog(:)';
                % increase
                cnt = cnt + 1;
            end

            break;  % only use one sub category
        end

    end

    save('query_fns.mat', 'query_fns');
    save('query_hog.mat', 'query_hog');
end

%% load db

if ~exist('db_hog', 'var')
    db_hog = load('db_hog.mat');
    db_hog = db_hog.img_hogs;
end
if ~exist('db_fns', 'var')
    db_fns = load('db_fns.mat');
    db_fns = db_fns.img_files;
    db_fns = db_fns.';
    % append root dir
    for i=1:length(db_fns)
        db_fns{i} = [db_dir db_fns{i}];
    end
end

%% hog l2

% result data
ranked_res_fns = cell(length(query_fns), size(db_fns,2));
ranked_res_names = cell(length(query_fns), size(db_fns,2));
rank_scores = zeros(length(query_fns), length(db_fns));

for i=1:size(query_hog, 1)
    qhog = query_hog(i,:);
    qhog_mat = repmat(qhog, size(db_hog,1), 1);
    dists = abs(qhog_mat - db_hog).^2;
    dists = sum(dists, 2);
    dists = sqrt(dists);
    [Y,I] = sort(dists, 1);
    rank_scores(i,:) = Y;
    ranked_res_fns(i,:) = db_fns(1,I);
    disp(num2str(i));
end

%% hog manifold

% load
if ~exist('db_manifolds', 'var')
    db_manifolds = load('db_manifolds.mat');
    db_manifolds = db_manifolds.db_manifolds;
end

% result data
ranked_res_names = cell(length(query_fns), length(db_manifolds));
rank_scores = zeros(length(query_fns), length(db_manifolds));

% match
for i=1:size(query_hog, 1)
    qhog = query_hog(i,:);
    % match each object manifold
    dists = zeros(length(db_manifolds), 1);
    names = cell(length(db_manifolds), 1);
    for j=1:length(db_manifolds)
        dists(j) = match_obj_manifold(qhog, 5, db_manifolds{j}.data);
        names{j} = db_manifolds{j}.name;
    end
    [Y,I] = sort(dists, 1);
    rank_scores(i,:) = Y;
    ranked_res_names(i,:) = names(I)';
    
    disp(num2str(i));
end


%% qualitative results
visualize_search_res('res_bruteforce.html', query_fns, ranked_res_fns, 50);


%% compute top K precision / accuracy
comp_accu(query_fns, ranked_res_names, 5)

