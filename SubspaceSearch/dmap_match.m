


%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

img_dir = 'D:\imgs\';

%% extract image and feature

imgfns = dir([img_dir '*.png']);
img_pixel = [];
img_hogs = [];
img_files = cell(size(imgfns,1), 1);
for i=1:size(imgfns,1)
    curfn = [img_dir imgfns(i).name];
    img_files{i,1} = curfn;
    curimg = imread(curfn);
    curimg = rgb2gray(curimg);
    curimg = imresize(curimg, [128,128]);
    img_pixel = [img_pixel; curimg(:)'];
    % compute hog
    cellSize = 8 ;
    hog = vl_hog(single(curimg), cellSize, 'verbose');
    % render hog
%     imhog = vl_hog('render', hog, 'verbose') ;
%     clf ; imagesc(imhog) ; colormap gray ;
%     pause
    img_hogs = [img_hogs; hog(:)'];
end

% save
save('img_fns.mat', 'img_files');
save('img_data.mat', 'img_pixel');
save('depth_hog.mat', 'img_hogs');


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
    curimg = imresize(curimg, [128,128]);
    query_pixel = [query_pixel; curimg(:)'];
    % compute hog
    cellSize = 8 ;
    hog = vl_hog(single(curimg), cellSize, 'verbose');
    query_hogs = [query_hogs; hog(:)'];
end

%% raw pixel L2
img_pixel = load('img_data.mat');

for i=1:size(query_pixel)
    
end


%% HOG L2


%% 