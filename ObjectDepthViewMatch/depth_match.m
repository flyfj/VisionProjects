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
    db_names = cell(size(db_fns));
    % append root dir
    for i=1:length(db_fns)
        db_fns{i} = [db_dir db_fns{i}];
        parts = strsplit(db_fns{i}, '\');
        tmp = strsplit(parts{5}, '__');
        db_names{i} = [tmp{1} '__' tmp{2}];
    end
    disp('loaded db files.');
end

%% hog l2

% result data
ranked_res_fns = cell(length(query_fns), size(db_fns,2));
% distance to all database view for each query view
view_match_scores = zeros(length(query_fns), length(db_fns));
% distance to all database object for each query view
obj_match_scores = [];
db_obj_names = [];  % non-duplicate object names for all db view

% profile on
for i=1:size(query_hog, 1)
    qhog = query_hog(i,:);
    qhog_mat = repmat(qhog, size(db_hog,1), 1);
    dists = abs(qhog_mat - db_hog).^2;
    dists = sum(dists, 2);
    view_match_scores(i, :) = sqrt(dists);
    
    [db_obj_names, tmp] = view_score_to_obj_score(db_names, view_match_scores(i,:));
    obj_match_scores = [obj_match_scores; tmp];
    
%     profile viewer
    disp(num2str(i));
end


%% hog manifold

% load
if ~exist('db_manifolds', 'var')
    db_manifolds = load('db_manifolds.mat');
    db_manifolds = db_manifolds.db_manifolds;
end

% result data
db_obj_names = cell(1, length(db_manifolds));
obj_match_scores = zeros(length(query_fns), length(db_manifolds));

% match
for i=1:size(query_hog, 1)
    qhog = query_hog(i,:);
    % match each object manifold
    for j=1:length(db_manifolds)
        obj_match_scores(i, j) = match_obj_manifold(qhog, 100, db_manifolds{j}.data);
        if i>1
            continue;
        end
        db_obj_names{1, j} = db_manifolds{j}.name;
    end
    
    disp(num2str(i));
end

%%
query_fns_comb = query_fns;
obj_match_score_comb = obj_match_scores;

%% multiview
view_step = 5;
query_num = length(query_fns) / view_step;
query_fns_comb = cell(query_num, 1);
obj_match_score_comb = zeros(query_num, size(obj_match_scores,2));
for i=1:query_num
    startid = (i-1)*view_step+1;
    query_fns_comb{i} = query_fns{startid};
    obj_match_score_comb(i,:) = mean(obj_match_scores(startid:startid+view_step-1, :), 1);
    disp(num2str(i));
end

%% qualitative results
visualize_search_res('res_bruteforce.html', query_fns, ranked_res_fns, 50);

%% compute top K precision / accuracy

% form result files
ranked_scores = zeros(length(query_fns_comb), length(db_obj_names));
ranked_res_names = cell(length(query_fns_comb), length(db_obj_names));
for i=1:size(obj_match_score_comb,1)
    [Y,I] = sort(obj_match_score_comb(i,:), 2);
    ranked_scores(i,:) = Y;
    ranked_res_names(i,:) = db_obj_names(1,I);
end

%%
comp_accu(query_fns_comb, ranked_res_names, 5, 1)

