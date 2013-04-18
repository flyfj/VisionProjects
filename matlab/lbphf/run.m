
imgdir = 'E:\GalaxyZoo\assets\zoo2\';
outdir = 'I:\Zoo2_lbp\';

imglist = dir([imgdir '*.jpg']);
for i=1:size(imglist,1)
    
    I=imread([imgdir imglist(i,1).name]);
    mapping=getmaplbphf(8);
    h=lbp(I,1,8,mapping,'h');
    h=h/sum(h);
    lbp_hf_features=constructhf(h,mapping);
    
    outfile = [outdir imglist(i,1).name '.lbp'];
    fid = fopen(outfile, 'w');
    fprintf(fid, '%f ', lbp_hf_features);
    fclose(fid);
    
    disp(['Finish ' imglist(i,1).name]);
end

