function [p, r, apM, ahd,ap, ph2] = prcal_MNIST(score,label,labelvalue)
% 
% Precision and Recall calculation for approximate nearest neighbor search 
% input: 
%  score - 1xn continuse real value 
%  label - 1xn discrete real value
%  labelvalue - value of true positives in the 'label' vector
%  p is the precision  
%  r is the recall
%  apM 
%%% Last update 
%%% Jan. 20, 2010
%%% Jun Wang (jwang@ee.columbia.edu)
if length(score)~=length(label)
    error('score and label must be equal length\n');
    pause;
end

%%% number of true samples
num_truesamples=sum(label==labelvalue);
%%% number of samples
numds=length(label);

%%% score is the computed hamming distance
[sorted_val, sorted_ind]=sort(score); 
sorted_label=label(sorted_ind);
sorted_truefalse=(sorted_label==labelvalue);

M=[5 10 50 100 200 300 400 500 1000];
for i_M=1:length(M)
    ahd(i_M)=mean(sorted_val(:,1:M(i_M)));
end
    
truepositive=cumsum(sorted_truefalse);
for i_M=1:length(M)
    apM(i_M)=truepositive(M(i_M))/M(i_M);
end

hd2_ind=find(score<=2);
if isempty(hd2_ind)
    ph2=0;
else
    ph2=sum(label(hd2_ind)==labelvalue)/length(hd2_ind);
end

r=truepositive/num_truesamples;
p=truepositive./[1:numds];
ap = apcal(score,label,labelvalue);