% extract hog descriptors from depth maps

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



%% compute pca manifold

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



%% vis 230 229
visualize_clusters(db_manifolds{410}.data);

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

