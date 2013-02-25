% GIST Parameters:
clear param
param.imageSize = [256 256]; % set a normalized image size
param.orientationsPerScale = [8 8 8 8]; % number of orientations per scale (from HF to LF)
param.numberBlocks = 4;
param.fc_prefilt = 4;

% file directory
imgdir = 'H:\AmazonProductData\DemoDataSet_resized\Apple_Desktops\';
gist_resdir = 'H:\MobileVisualSearch\gist\Apple_Desktops\';
readpath = [imgdir '*.jpg'];
readlist = dir(readpath);   % save readlist as image files
[m,n] = size(readlist);

% Pre-allocate gist:
Nfeatures = sum(param.orientationsPerScale)*param.numberBlocks^2;
gist = zeros([m Nfeatures]); 

% Load first image and compute gist:
img = imread( [imgdir readlist(1,1).name] );
[gist(1, :), param] = LMgist(img, '', param); % first call
% Loop:
for i = 2:m
   img = imread( [imgdir readlist(i,1).name] );
   gist(i, :) = LMgist(img, '', param); % the next calls will be faster
   disp([num2str(i) 'th img done.']);
end

disp('Finish.');

