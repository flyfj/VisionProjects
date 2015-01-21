


%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

img_dir = 'F:\3D\DepthMaps_new\';

%% extract image and feature

imgfns = dir([img_dir '*.png']);
[m,n] = size(imgfns);
img_pixel = zeros(m, 64*64);
img_hogs = zeros(m, 1984);
img_files = cell(m, 1);
parfor i=1:m
    curfn = [img_dir imgfns(i).name];
    img_files{i,1} = curfn;
    curimg = imread(curfn);
%     curimg = imresize(curimg, [128,128]);
    img_pixel(i, :) = curimg(:)';
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
save('img_fns.mat', 'img_files', '-v7.3');
save('img_data.mat', 'img_pixel', '-v7.3');
save('depth_hog.mat', 'img_hogs', '-v7.3');


%% match object depth map

% load query
query_dir = '';
qfns = dir([query_dir '*.png']);
query_files = cell(size(qfns,1), 1);
query_pixel = [];
query_hogs = [];
for i=1:size(qfns,1)
    curfn = [query_dir qfns(i).name];
    query_files{i,1} = curfn;
    curimg = imread(curfn);
    curimg = rgb2gray(curimg);
    query_pixel = [query_pixel; curimg(:)'];
    % compute hog
    cellSize = 8;
    hog = vl_hog(single(curimg), cellSize, 'verbose');
    query_hogs = [query_hogs; hog(:)'];
end

%% raw pixel L2
img_pixel = load('img_data.mat');
img_fns = load('img_fns.mat');
img_fns = img_fns.img_files;


for i=1:size(img_fns)
    
end


%% HOG L2

depth_hog = load('depth_hog.mat');
depth_hog = depth_hog.img_hogs;
img_fns = load('img_fns.mat');
img_fns = img_fns.img_files;
img_fns = img_fns.';

%%
% randomly select query
ids = randperm(size(depth_hog,1), size(depth_hog,1));
ids = ids(1:100);
%%
query_fns = cell(length(ids), 1);
ranked_res_fns = cell(length(ids), size(img_fns,2));
for i=1:length(ids)
    qhog = depth_hog(ids(i),:);
    qhog_mat = repmat(qhog, size(depth_hog,1), 1);
    dists = abs(qhog_mat - depth_hog).^2;
    dists = sum(dists, 2);
    dists = sqrt(dists);
    [Y,I] = sort(dists, 1);
    query_fns(i,1) = img_fns(ids(i));
    ranked_res_fns(i,:) = img_fns(1,I);
    disp(num2str(i));
end
%%
 visualize_search_res('res.html', query_fns, ranked_res_fns, 100);


%% 