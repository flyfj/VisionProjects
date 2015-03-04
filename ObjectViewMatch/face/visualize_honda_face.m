
% draw face images in honda dataset
data_fn = 'F:\Recognition\Honda_processed.mat';
data = load(data_fn);
imgs = data.ImgData;

for i=1:length(imgs)
    % randomly select 10 samples from each image set
    sel_ids = randperm(size(imgs{i}, 2), 10);
    sel_faces = imgs{i}(:, sel_ids);
    figure
    for j=1:size(sel_faces, 2)
        subplot(1, size(sel_faces,2), j)
        imshow(reshape(mat2gray(sel_faces(:,j)), [20,20]))
        hold on
    end
    pause
end