% GIST Parameters:
clear param
param.imageSize = [256 256]; % set a normalized image size
param.orientationsPerScale = [8 8 8 8]; % number of orientations per scale (from HF to LF)
param.numberBlocks = 4;
param.fc_prefilt = 4;

imgdir = 'F:\Datasets\MobileProductSearch\Whole_Dataset\AmazonProductSet\DemoDataSet_resized\';
resdir = 'F:\Results\MobileVisualSearch\gist\';

% category list
cate_list_file = 'F:\Datasets\MobileProductSearch\Whole_Dataset\AmazonProductSet\test_categories.txt';
fid = fopen(cate_list_file);
cate_list = textscan(fid, '%s');
fclose(fid);
cate_list = cate_list{1,1};

for id=1:size(cate_list,1)
    
    savefile = [resdir cate_list{id,1} '.mat'];
    if(exist(savefile, 'file'))
        continue;
    end
    
    disp(cate_list{id,1});
    
    % file directory
    curdir = [imgdir cate_list{id,1} '\'];
    gist_resdir = [resdir cate_list{id,1} '\'];
    mkdir(gist_resdir);
    readpath = [curdir '*.jpg'];
    readlist = dir(readpath);   % save readlist as image files
    [m,n] = size(readlist);
    
    if(m==0)
        continue;
    end

    % Pre-allocate gist:
    Nfeatures = sum(param.orientationsPerScale)*param.numberBlocks^2;
    gist = zeros([m Nfeatures]); 

    % Load first image and compute gist:
    img = imread( [curdir readlist(1,1).name] );
    [gist(1, :), param] = LMgist(img, '', param); % first call
    % Loop:
    for i = 2:m
       img = imread( [curdir readlist(i,1).name] );
       gist(i, :) = LMgist(img, '', param); % the next calls will be faster
       disp([num2str(i) 'th img done.']);
    end
    
    
    save(savefile, 'gist');
    
    disp(['Save gist to ' savefile]);
    
end


disp('Finish.');

