
% pick subset from youtube
unique_train_ids = unique(traingnd);
unique_test_ids = unique(testgnd);
hist(traingnd, unique_train_ids)
% hist(testgnd, unique_test_ids)

% randomly select subjects
subject_num = 50;
sel_ids = randsample(unique_train_ids, 50);
train_data = [];
train_ids = [];
test_data = [];
test_ids = [];
for i=1:length(sel_ids)
    curid = sel_ids(i);
    train_sel = find(traingnd==curid);
    train_data = [train_data; traindata(train_sel,:)];
    train_ids = [train_ids; traingnd(train_sel)];
    test_sel = find(testgnd==curid);
    test_data = [test_data; testdata(test_sel,:)];
    test_ids = [test_ids; testgnd(test_sel)];
end

save('youtube_face_subset50.mat', 'train_data', 'train_ids', 'test_data', 'test_ids', '-v7.3');

