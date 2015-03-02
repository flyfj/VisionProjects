
% if manifold cluster has only one member
for i=1:length(db_manifolds)
    cls = db_manifolds{i}.data;
    for j=1:length(cls)
        if length(cls{j}.fns) == 1
            disp([num2str(i) ',' num2str(j)]);
            disp(cls{j}.fns{1});
        end
    end
end