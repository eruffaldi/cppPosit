function [u,h] = halfinrange(mi,ma)

z =uint16(0:65535);
y =halfprecision(z,'double');

ii = find(y >= mi & y <=ma);
u =z(ii);
h = y(ii);

[h,hi]= sort(h);
u =u(hi);