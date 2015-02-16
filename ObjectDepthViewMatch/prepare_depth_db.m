
function [ db_feats, db_fns, db_obj_names, db_fn_obj_ids] = prepare_depth_db( totrain, db_dir )
%PREPARE_DATA Summary of this function goes here
%   prepare synthesized depth data
new_sz = [64 64];

disp('preparing db depth...');

if totrain == 1
    %% extract image and feature
    imgfns = dir([db_dir '*.png']);
    [m,n] = size(imgfns);
    db_feats = zeros(m, 8*8*31);
    db_fns = cell(m, 1);
    for i=1:m
        curfn = [db_dir imgfns(i).name];
        db_fns{i,1} = imgfns(i).name;
        curimg = imread(curfn);
        curimg = mat2gray(curimg);
        curimg = imresize(curimg, new_sz);
        % compute hog
        cellSize = 8;
        hog = vl_hog(single(curimg), cellSize, 'verbose');
        % render hog
    %     imhog = vl_hog('render', hog, 'verbose') ;
    %     clf ; imagesc(imhog) ; colormap gray ;
    %     pause
        db_feats(i, :) = hog(:)';

        disp([num2str(i) '|' num2str(m)]);
    end

    % save
    save('db_fns.mat', 'db_fns', '-v7.3');
    save('db_hog.mat', 'db_feats', '-v7.3');
    
    disp('extracting db object names...');
    db_obj_names = cell(0); % corresponding db obj name
    db_fn_obj_ids = zeros(length(db_fns), 1);
    obj_map = containers.Map();
    for i=1:length(db_fns)
        % get db object name
        db_fns{i} = [db_dir db_fns{i}];
        parts = strsplit(db_fns{i}, '\');
        tmp = strsplit(parts{5}, '__');
        obj_id = [tmp{1} '__' tmp{2}];
        if isKey(obj_map, obj_id) == 0
            % add new object name
            db_obj_names = [db_obj_names; obj_id];
            obj_map(obj_id) = length(db_obj_names);
            disp(['added object: ' obj_id]);
        end
        db_fn_obj_ids(i) = obj_map(obj_id);
    end
    save('db_obj_names.mat', 'db_obj_names', '-v7.3');
    save('db_fn_obj_ids.mat', 'db_fn_obj_ids', '-v7.3');
    disp('done.');

else
    tmp = load('db_hog.mat');
    db_feats = tmp.db_feats;
    disp('loaded db features.');

    tmp = load('db_fns.mat');
    db_fns = tmp.db_fns;
    disp('loaded db files.');
    
    tmp = load('db_obj_names.mat');
    db_obj_names = tmp.db_obj_names;
    
    tmp = load('db_obj_fn_ids.mat');
    db_fn_obj_ids = tmp.db_fn_obj_ids;
  
    disp('loaded db object names and ids.');
end

disp('database depth ready.');


