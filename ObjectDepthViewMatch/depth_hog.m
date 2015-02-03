% extract hog descriptors from depth maps


%% init
addpath('C:\vlfeat-0.9.20-bin\toolbox\');
vl_setup

img_dir = 'F:\3D\ModelNet\test_db_depth2\';
db_dir = 'F:\3D\ModelNet\depth_hog\';
query_dir = 'E:\Datasets\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\coffee_mug\coffee_mug_7\';
new_sz = [64 64];

%% visualize hog
cellSize = 8;
imgfn = 'E:\frame_320.png';
img = imread(imgfn);
img = imresize(img, new_sz);
figure
subplot(1, 2, 1)
imagesc(img)
hog = vl_hog(single(img), cellSize, 'verbose');
% render hog
imhog = vl_hog('render', hog, 'verbose');
subplot(1, 2, 2)
imagesc(imhog);
colormap gray ;

%% extract image and feature

imgfns = dir([img_dir '*.png']);
[m,n] = size(imgfns);
img_hogs = zeros(m, 1984);
img_files = cell(m, 1);
parfor i=1:m
    curfn = [img_dir imgfns(i).name];
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


