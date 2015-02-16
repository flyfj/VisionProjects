
function [ db_feats, db_fns ] = prepare_depth_db( totrain, db_dir )
%PREPARE_DATA Summary of this function goes here
%   prepare synthesized depth data
new_sz = [64 64];

disp('preparing db depth...');

if totrain == 1
    %% extract image and feature
    imgfns = dir([db_dir '*.png']);
    [m,n] = size(imgfns);
    db_feats = zeros(m, 1984);
    db_fns = cell(m, 1);
    parfor i=1:m
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

else
    db_feats = load('db_hog.mat');
    db_feats = db_feats.db_feats;
    disp('loaded db features.');

    db_fns = load('db_fns.mat');
    db_fns = db_fns.db_fns;
    disp('loaded db files.');
  
end

disp('database depth ready.');


