function t =describetab(t)

for I=1:height(t)
    v=t.float{I};
    W=v(~(isnan(v)|isinf(v)));
    v=t.res{I};
    Q=v(~(isnan(v)|isinf(v)));
    t.max(I) = max(W);
    t.min(I) = min(W);
    t.smallest(I) = min(abs(W(W ~= 0)));
    t.minres(I) = min(Q);
    t.maxres(I) = max(Q);
end
