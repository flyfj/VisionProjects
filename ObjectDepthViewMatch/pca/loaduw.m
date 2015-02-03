
%% load uw rgbd object view dataset
root_dir = 'E:\Datasets\RGBD_Dataset\UW\rgbd-obj-dataset\rgbd-dataset\';
cate_dirs = dir([root_dir '*.*']);

for i=1:length(cate_dirs)
    
    cate_name = cate_dirs(i).name;
    if( strcmp(cate_name, '.') || strcmp(cate_name, '..'))
        continue;
    end
    
    obj_dirs = dir([root_dir cate_name '\*.*']);
    for j=1:length(obj_dirs)
        
        cur_obj_dir = obj_dirs(j).name;
        if(strcmp(cur_obj_dir, '.') || strcmp(cur_obj_dir, '..'))
            continue;
        end
        % get object images
        obj_imgfns = dir([root_dir cate_name '\' cur_obj_dir '\*_crop.png']);
        
        all_imgs = zeros(length(obj_imgfns), 50*50);
        for k=1:length(obj_imgfns)
            cur_img = imread([root_dir cate_name '\' cur_obj_dir '\' obj_imgfns(k).name]);
            gray_img = rgb2gray(cur_img);
            gray_img = imresize(gray_img, [50 50])';
            all_imgs(k, :) = gray_img(:);
            disp([num2str(k) ' img read.']);
        end
        [eig_vecs, obj_mean] = comp_obj_subspace(all_imgs);
        % save to data
        db_obj.eig_vecs = eig_vecs;
        db_obj.obj_mean = obj_mean;
        db_obj.dir = [cate_name '\' cur_obj_dir];
        save([cate_name '-' cur_obj_dir], 'db_obj');
%         %show mean
%         imshow(reshape(obj_mean, 50, 50)', []);
%         %show eigenvectors
%         close all
%         for m=1:20
%             subplot(1, 20, m)
%             eig_img = reshape(eig_vecs(m,:), 50, 50)';
%             imshow(eig_img, [])
%             hold on
%             pause
%         end
        disp(['finished ' cate_name '-' cur_obj_dir]);
    end
    
end