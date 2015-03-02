function [ db_feats, db_ids, query_feats, query_ids ] = prepare_janus_face( totrain, protocol, split_id )
%PREPARE_JANUS_FACE Summary of this function goes here
%   each set is a cell entry

janus_root = 'F:\Recognition\JANUS\CS0\';

if totrain == 1
% this version will parse csv and get only the face pixel as feature

% process gallery
gal_csv_fn = sprintf('%sprotocol/split%d/test_%d_%s_gal.csv', janus_root, split_id, split_id, protocol);
gal_csv = importdata(gal_csv_fn);

unique_media_ids = unique(gal_csv.data(:,1));
set_num = length(unique_media_ids);
gal_feats = cell(set_num,1);
gal_subject_ids = zeros(set_num, 1);
for i=1:set_num
   % get media id
   ids = find(gal_csv.data(:,1) == unique_media_ids(i));
   cur_set_faces = zeros(length(ids), 400);
   for j=1:length(ids)
       % file name
        cur_fn = [janus_root 'faces\' gal_csv.textdata{ids(j)+1, 3}];
        if ~exist(cur_fn, 'file')
            continue;
        end
        face_img = imread(cur_fn);
        if length(size(face_img)) == 3
            face_img = rgb2gray(face_img);
        end
       face_img = imresize(face_img, [20, 20]);
       cur_set_faces(j, :) = face_img(:);
   end
   gal_feats{i} = cur_set_faces;
   gal_subject_ids(i) = str2num(gal_csv.textdata{ids(1)+1, 2});
  
end


% probe
probe_csv_fn = sprintf('%sprotocol/split%d/test_%d_%s_probe.csv', janus_root, split_id, split_id, protocol);
probe_csv = importdata(probe_csv_fn);

unique_media_ids = unique(probe_csv.data(:,1));
set_num = length(unique_media_ids);
probe_feats = cell(set_num,1);
probe_subject_ids = zeros(set_num, 1);
for i=1:set_num
   % get media id
   ids = find(probe_csv.data(:,1) == unique_media_ids(i));
   cur_set_faces = zeros(length(ids), 400);
   for j=1:length(ids)
       % file name
        cur_fn = [janus_root 'faces\' probe_csv.textdata{ids(j)+1, 3}];
        face_img = imread(cur_fn);
        if length(size(face_img)) == 3
            face_img = rgb2gray(face_img);
        end
       face_img = imresize(face_img, [20, 20]);
       cur_set_faces(j, :) = face_img(:);
   end
   probe_feats{i} = cur_set_faces;
   probe_subject_ids(i) = str2num(probe_csv.textdata{ids(1)+1, 2});
   
end

save_fn = sprintf('janus_split%d_%s.mat', split_id, protocol);
save(save_fn, 'gal_feats', 'gal_subject_ids', 'probe_feats', 'probe_subject_ids'); 


else
end




end

