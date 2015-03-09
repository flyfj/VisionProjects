
function [ db_feats, db_cate_ids] = prepare_depth_db( totrain, feat_type )
%PREPARE_DATA Summary of this function goes here
%   prepare synthesized depth data
new_sz = [64 64];
db_dir = 'F:\3D\ModelNet\test_db_depth\';

disp('preparing db depth...');

if totrain == 1
    objdirs = dir([db_dir '*.*']);
    objdirs(1:2) = [];
    db_feats = cell(length(objdirs), 1);
    db_cate_ids = zeros(length(objdirs), 1);
    db_cate_names = containers.Map();
    % each object
    for i=1:length(objdirs)
        
        disp(['processing ' objdirs(i).name]);
        
        % assign cate id
        tmp = strsplit(objdirs(i).name, '__');
        cate_name = tmp{1};
        if ~isKey(db_cate_names, cate_name)
            db_cate_names(cate_name) = i;
        end
        db_cate_ids(i) = db_cate_names(cate_name);
        
        % extract feature
        obj_feats = [];
        obj_root = [db_dir '\' objdirs(i).name '\'];
        imgfns = dir([obj_root '*.png']);
        m = length(imgfns);
        parfor j=1:m
            curfn = [obj_root imgfns(j).name];
            curimg = imread(curfn);
            curimg = mat2gray(curimg);
            curimg = imresize(curimg, new_sz);
            switch feat_type
                case 'hog'
                    cellSize = 8;
                    hog = vl_hog(single(curimg), cellSize);
                    obj_feats = [obj_feats; hog(:)'];
                case 'pixel'
                    obj_feats = [obj_feats; curimg(:)' ./ 255];
            end
            disp([num2str(i) ' : ' num2str(j) '/' num2str(m)]);
        end
        disp('feature extracted.');
        
        db_feats{i} = obj_feats;
        
    end

    % save
    db_cate_names = db_cate_names.keys';
    save('depth_db_data.mat', 'db_feats', 'db_cate_ids', 'db_cate_names', '-v7.3');
    disp('done.');

else
    tmp = load('depth_db_data.mat');
    db_feats = tmp.db_feats;
    db_cate_ids = tmp.db_cate_ids;
    db_cate_names = tmp.db_cate_names;
    
end

disp('database depth ready.');


