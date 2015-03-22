

% map names to id
db_dir = 'F:\3D\ModelNet\test_db_depth\';
objdirs = dir([db_dir '*.*']);
objdirs(1:2) = [];
db_cate_names = containers.Map();
cnt = 1;
names = [];
for i=1:length(objdirs)
        
    % assign cate id
    tmp = strsplit(objdirs(i).name, '__');
    cate_name = tmp{1};
    if ~isKey(db_cate_names, cate_name)
        db_cate_names(cate_name) = cnt;
        curname.name = cate_name;
        curname.id = cnt;
        names = [names; curname];
        cnt = cnt + 1;
    end
    
end

save('names.mat', 'names', '-v7.3');