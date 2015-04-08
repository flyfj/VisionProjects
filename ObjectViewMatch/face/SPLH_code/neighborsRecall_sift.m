function [R,P,AP] = neighborsRecall_sift(trueRank, rank)
%
% compute P(i in Saprox(M) | i in S(N)) as a function of M
% we fix M = 50;
%
% Input:
%   - trueRank: indices from closest neighbors to far
%   - rank: output ranking of the algorithm
% Output:
%   - h: plot handle 
%   - P: vector of dimension [600 1] = P(i in Saprox(M) | i in S(N)), for N=1:600

[nimagestest, sizedatabase] = size(rank);
N=size(trueRank,2);

M_set=[10, 50 100, 200, 300, 400, 500, 1000,2000,10000];

jj = zeros(size(rank));
    
test_num=20000;

for n = 1:nimagestest
    jj(n,:) = ismember(rank(n,:), trueRank(n,1:test_num));    
end

cumsum_rank=cumsum(jj,2);

Ri = single(cumsum_rank/test_num);
R = mean(Ri,1);

for i_M=1:length(M_set)
    M=M_set(i_M);
    Pi=sum(jj(:,1:M),2)/M;
    P(i_M)=mean(Pi,1);
end


Pi = single(cumsum_rank./repmat([1:sizedatabase],nimagestest,1));
%Pc=mean(Pi,1);
AP = sum(sum(Pi.*jj))/sum(sum(jj));



%%% evaluate single query
% N = 50;   
% j = trueRank(:,1:N);
% jj = ismember(rank, j);    
% R = single(cumsum(jj)/N);
% P = single(cumsum(jj)./[1:length(rank)]);
