function X = normr(Y)

X = Y./repmat(sqrt(sum(Y.*Y, 2)),1, size(Y, 2));