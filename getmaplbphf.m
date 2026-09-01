function mapping = getmaplbphf(samples)
%GETMAPLBPHF  Build the rotation-invariant mapping table for LBP-HF.
%   mapping(i+1) is the smallest value among all cyclic rotations of the
%   samples-bit binary number i.
%   Fixed: bitshift takes only (value, amount); the extra third argument
%   caused "ASSUMEDTYPE must be an integer type name."

    num = 2^samples;
    mapping = zeros(num,1);

    for i = 0:num-1
        minval = i;
        ii = i;
        for k = 1:samples
            j = bitset(bitshift(i,1),1,bitget(i,samples)); %rotate left
            if j < minval
                minval = j;
            end
            i = j;
        end
        mapping(ii+1) = minval;
    end
end
