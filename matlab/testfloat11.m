function r = testfloatt11(zp,mode,a,b,omode)
r = [];

ia = toindex(zp,mode,a);
if isempty(ia)
    return;
end
r.a=struct('index',ia,'signed',zp.signed{1}(ia),'unsigned',zp.unsigned{1}(ia),'float',zp.float{1}(ia));

ib = toindex(zp,mode,b);
if isempty(ib)
    return;
end
r.b=struct('index',ib,'signed',zp.signed{1}(ib),'unsigned',zp.unsigned{1}(ib),'float',zp.float{1}(ib));
r.s =fromindex(zp,'float',ia)+fromindex(zp,'float',ib);
iy = toindex(zp,'float',r.s);
if isempty(iy)==0
r.y=struct('index',iy,'signed',zp.signed{1}(iy),'unsigned',zp.unsigned{1}(iy),'float',zp.float{1}(iy));
r.diff =r.s-r.y.float;
end
