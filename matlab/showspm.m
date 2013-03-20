
grid_num = 0;
feat_dim = 0;

fid = fopen('E:\spm.txt');
A = fscanf(fid, ['%d' '%d'], 2);
grid_num = A(1,1);
feat_dim = A(2,1);

spm = zeros(grid_num, feat_dim);

for i=1:grid_num
    for j=1:feat_dim
        spm(i,j) = fscanf(fid, '%f', 1);
    end
end

fclose(fid);


% show histogram
figure(1);
for i=1:16
    subplot(4,4,i)
    plot(1:feat_dim, spm(i,:), 'r-')
end
