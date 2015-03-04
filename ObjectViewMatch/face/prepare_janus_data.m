function [ gal_feats, gal_ids, probe_feats, probe_ids ] = prepare_janus_data( totrain, protocol, split_id )
%   each set is a cell entry

janus_root = 'F:\Recognition\JANUS\CS0\';
save_fn = sprintf('janus_split%d_%s.mat', split_id, protocol);

if totrain == 1
        
    % process gallery
    gal_csv_fn = sprintf('%sprotocol/split%d/test_%d_%s_gal.csv', janus_root, split_id, split_id, protocol);
    [gal_feats, gal_ids] = enroll_faces(janus_root, gal_csv_fn);

    % probe
    probe_csv_fn = sprintf('%sprotocol/split%d/test_%d_%s_probe.csv', janus_root, split_id, split_id, protocol);
    [probe_feats, probe_ids] = enroll_faces(janus_root, probe_csv_fn);

    % save
    save(save_fn, 'gal_feats', 'gal_ids', 'probe_feats', 'probe_ids');
    
    disp('janus data trained.');
else
    % load
    if ~exist(save_fn, 'file')
        disp([save_fn ' not exists.']);
        return;
    end
    tmp = load(save_fn);
    gal_feats = tmp.gal_feats;
    gal_ids = tmp.gal_ids;
    probe_feats = tmp.probe_feats;
    probe_ids = tmp.probe_ids;
    
    disp('janus data loaded.');
end

end


%% tool function to process one csv file
function [feats, ids] = enroll_faces(janus_root, csv_fn)
    
gal_csv = importdata(csv_fn);
% parse template ids as instance ids
template_ids = zeros(size(gal_csv.textdata, 1)-1, 1);
for i=1:length(template_ids)
    template_ids(i) = str2num(gal_csv.textdata{i+1,1});
end

% compute feature and form matrix
unique_template_ids = unique(template_ids);
set_num = length(unique_template_ids);
feats = cell(set_num,1);
ids = zeros(set_num, 1);
for i=1:set_num
   disp(sprintf('loading template %d/%d...', i, set_num));
   
   set_row_ids = find(template_ids == unique_template_ids(i));
   cur_set_faces = zeros(length(set_row_ids), 400);
   for j=1:length(set_row_ids)
       % file name
        cur_fn = [janus_root 'faces\' gal_csv.textdata{set_row_ids(j)+1, 3}];
        if ~exist(cur_fn, 'file')
            continue;
        end
        face_img = imread(cur_fn);
        if length(size(face_img)) == 3
            face_img = rgb2gray(face_img);
        end
       face_img = imresize(face_img, [20, 20]);
       %-- TODO: replace to any feature extraction code --%
       cur_set_faces(j, :) = face_img(:);
   end
   feats{i} = cur_set_faces;
   ids(i) = str2num(gal_csv.textdata{set_row_ids(1)+1, 2});

end
    
ids = int32(ids);

end

