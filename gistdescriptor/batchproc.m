% GIST Parameters:
clear param
param.imageSize = [256 256]; % set a normalized image size
param.orientationsPerScale = [8 8 8 8]; % number of orientations per scale (from HF to LF)
param.numberBlocks = 4;
param.fc_prefilt = 4;

imgdir = 'F:\Datasets\Recognition\CIFAR-10\cifar-10-binary\test_imgs\';
%'F:\Datasets\MobileProductSearch\Whole_Dataset\AmazonProductSet\DemoDataSet_resized\';
resdir = 'F:\Datasets\Recognition\CIFAR-10\cifar-10-binary\';
%'F:\Results\MobileVisualSearch\gist\';

% category list
cate_list_file = 'F:\Datasets\MobileProductSearch\Whole_Dataset\AmazonProductSet\test_categories.txt';
fid = fopen(cate_list_file);
cate_list = textscan(fid, '%s');
fclose(fid);
cate_list = cate_list{1,1};

% not using category list, loop over all image files

savefile = [resdir 'test_gist.txt'];
    
m = 10000;
% Pre-allocate gist:
Nfeatures = sum(param.orientationsPerScale)*param.numberBlocks^2;
gist = zeros([m Nfeatures]); 

for id=0:m-1

    % Load first image and compute gist:
    img = imread( [imgdir num2str(id) '.jpg'] );
    [gist(id+1, :), param] = LMgist(img, '', param); % first call
    
    disp([num2str(id) 'th img done.']);

end

save(savefile, 'gist', '-ASCII')
disp(['Save gist to ' savefile]);
disp('Finish.');

