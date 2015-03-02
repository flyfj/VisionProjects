
function [ db_feats, db_fns, db_obj_cates, db_fn_cate_ids, db_obj_names, db_fn_obj_ids] = prepare_depth_db( totrain, db_dir )
%PREPARE_DATA Summary of this function goes here
%   prepare synthesized depth data
new_sz = [64 64];

disp('preparing db depth...');

if totrain == 1
    % extract image and feature
    imgfns = dir([db_dir '*.png']);
    [m,n] = size(imgfns);
    db_feats = zeros(m, 8*8*31);
    db_fns = cell(m, 1);
    parfor i=1:m
        curfn = [db_dir imgfns(i).name];
        db_fns{i,1} = imgfns(i).name;
        curimg = imread(curfn);
        curimg = mat2gray(curimg);
        curimg = imresize(curimg, new_sz);
        % compute hog
        cellSize = 8;
        hog = vl_hog(single(curimg), cellSize, 'verbose');
        db_feats(i, :) = hog(:)';

        disp([num2str(i) '|' num2str(m)]);
    end
    
    disp('extracting db object names...');
    db_obj_cates = cell(0);
    db_fn_cate_ids = zeros(length(db_fns), 1);
    db_obj_names = cell(0); % corresponding db obj name
    db_fn_obj_ids = zeros(length(db_fns), 1);
    
    cate_map = containers.Map();
    obj_map = containers.Map();
    for i=1:length(db_fns)
        % get db object name
        db_fns{i} = [db_dir db_fns{i}];
        parts = strsplit(db_fns{i}, '\');
        tmp = strsplit(parts{5}, '__');
        
        % object category
        obj_cate = tmp{1};
        if isKey(cate_map, obj_cate) == 0
            db_obj_cates = [db_obj_cates; obj_cate];
            cate_map(obj_cate) = length(db_obj_cates);
            disp(['added category: ' obj_cate]);
        end
        db_fn_cate_ids(i) = cate_map(obj_cate);
        
        % object id
        obj_id = [tmp{1} '__' tmp{2}];
        if isKey(obj_map, obj_id) == 0
            db_obj_names = [db_obj_names; obj_id];
            obj_map(obj_id) = length(db_obj_names);
            disp(['added object: ' obj_id]);
        end
        db_fn_obj_ids(i) = obj_map(obj_id);
        
    end

    save('depth_db_data.mat', 'db_feats', 'db_fns', 'db_obj_cates', 'db_fn_cate_ids', 'db_obj_names', 'db_fn_obj_ids', '-v7.3');
    disp('done.');

else
    tmp = load('depth_db_data.mat');
    db_feats = tmp.db_feats;
    db_fns = tmp.db_fns;
    db_obj_cates = tmp.db_obj_cates;
    db_fn_cate_ids = tmp.db_fn_cate_ids;
    db_obj_names = tmp.db_obj_names;
    db_fn_obj_ids = tmp.db_fn_obj_ids;
    
end

disp('database depth ready.');


