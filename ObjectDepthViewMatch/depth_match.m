%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup



db_dir = 'F:\3D\ModelNet\test_db_depth2\';
query_dir = 'F:\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
new_sz = [64 64];

%% prepare query
% each category uses 10 queries


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
for i=1:1%size(query_hog, 1)
    qhog = query_hog(i,:);
    % match each object manifold
    for j=1:length(db_manifolds)
        obj_match_scores(i, j) = match_obj_manifold(qhog, 100, db_manifolds{j}.data);
%         if obj_match_scores(i,j) == 0
%             obj_match_scores(i,j) = 100;
%         end
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

