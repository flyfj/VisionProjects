% extract hog descriptors from depth maps


%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

db_dir = 'F:\3D\ModelNet\test_db_depth2\';
query_dir = 'E:\Datasets\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\coffee_mug\coffee_mug_7\';
new_sz = [64 64];

%% visualize hog
cellSize = 8;
tmp_dir = 'E:\Images\';
imgfns = dir([tmp_dir '*.jpg']);
tmp_hogs = zeros(100, 1984);
for i=1:100
    imgfn = [tmp_dir imgfns(i).name];
    img = imread(imgfn);
    img = imresize(img, new_sz);
    hog = vl_hog(single(img), cellSize, 'verbose');
    % render hog
%     imhog = vl_hog('render', hog, 'verbose');
%     subplot(1, 2, 2)
%     imagesc(imhog);
%     colormap gray ;
    tmp_hogs(i, :) = hog(:)';
end

imagesc(tmp_hogs)
colorbar

%% extract image and feature

imgfns = dir([db_dir '*.png']);
[m,n] = size(imgfns);
img_hogs = zeros(m, 1984);
img_files = cell(m, 1);
parfor i=1:m
    curfn = [db_dir imgfns(i).name];
    img_files{i,1} = imgfns(i).name;
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
    img_hogs(i, :) = hog(:)';
    
    disp([num2str(i) '|' num2str(m)]);
end

% save
save('db_fns.mat', 'img_files', '-v7.3');
% save('img_data.mat', 'img_pixel', '-v7.3');
save('db_hog.mat', 'img_hogs', '-v7.3');

%% compute pca manifold

if ~exist('db_hog', 'var')
    db_hog = load('db_hog.mat');
    db_hog = db_hog.img_hogs;
    disp('loaded db hogs.');
end
if ~exist('db_fns', 'var')
    db_fns = load('db_fns.mat');
    db_fns = db_fns.img_files;
    db_fns = db_fns.';
    for i=1:length(db_fns)
        db_fns{i} = [db_dir db_fns{i}];
    end
    disp('loaded db files.');
end

% append root dir
if ~exist('obj_map', 'var')
    obj_map = containers.Map();
    for i=1:length(db_fns)
        % divide into categories
        parts = strsplit(db_fns{i}, '\');
        tmp = strsplit(parts{5}, '__');
        obj_id = [tmp{1} '__' tmp{2}];
        if isKey(obj_map, obj_id) == 0
            obj_map(obj_id) =[];
        end
        obj_map(obj_id) = [obj_map(obj_id), i];
    end
    disp('organized object images.');
end

% for each object, compute manifold
% manifold structure: name: object name; data: cluster data, mean, eigvectors and
% member files
obj_names = keys(obj_map);
db_manifolds = cell(length(obj_names), 1);
for i=1:length(obj_names)
    feat_ids = obj_map(obj_names{i});
    db_manifolds{i}.name = obj_names{i};
    db_manifolds{i}.data = comp_pca_manifold(db_hog(feat_ids, :), db_fns(feat_ids));
    disp([num2str(i) '/' num2str(length(obj_names)) ' manifold learned.']);
end

save('db_manifolds.mat', 'db_manifolds', '-v7.3');

%% vis 230 229
visualize_imgs(db_manifolds{230}.data{4}.fns);

%% simple test for hog performance
% pick one query and do bruteforce matching
qhog = db_hog(25042, :);
tmp_ids = 25001:25100;
tmp_hog = db_hog(tmp_ids, :);
qhog_mat = repmat(qhog, size(tmp_hog,1), 1);
dists = abs(qhog_mat - tmp_hog).^2;
dists = sum(dists, 2);
dists = sqrt(dists);
[Y,I] = sort(dists, 1);
visualize_imgs(db_fns(1, tmp_ids(I(1:20))));

