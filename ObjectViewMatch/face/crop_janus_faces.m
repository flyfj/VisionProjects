
% crop out faces from janus images

janus_root = 'F:\Recognition\JANUS\CS0\';

save_dir = [janus_root 'img_faces/'];
mkdir(save_dir);

% load metadata
fns = cell(1, 4);
fns{1} = [janus_root 'protocol/galleryA.csv'];
fns{2} = [janus_root 'protocol/galleryB.csv'];
fns{3} = [janus_root 'protocol/probeA.csv'];
fns{4} = [janus_root 'protocol/probeB.csv'];

for i=1:length(fns)
    disp(['processing ' fns{i}]);
    
    csv = importdata(fns{i});
    tnum = size(csv.textdata, 1);
    for j=2:tnum
        cur_fn = [janus_root csv.textdata{j,3}];
        face_box = csv.data(j-1, 4:7);
        cur_img = imread(cur_fn);
        face_roi = imcrop(cur_img, face_box);
        save_fn = [janus_root 'faces/' csv.textdata{j,3}];
        imwrite(face_roi, save_fn);
        
        disp([num2str(j-1) '/' num2str(tnum-1)]);
    end
    
end



