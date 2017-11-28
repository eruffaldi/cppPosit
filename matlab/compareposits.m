n8 = {'positint8_t,8,0,uint16_t,false.bin','positint8_t,8,1,uint16_t,false.bin','positint8_t,8,2,uint16_t,false.bin','positint8_t,8,3,uint16_t,false.bin'};
p8 = cellfun(@(x) loadpositdump(['../build/' x]),n8,'UniformOutput',false);

%%
n10= {'positint16_t,10,0,uint16_t,false.bin','positint16_t,10,1,uint16_t,false.bin','positint16_t,10,2,uint16_t,false.bin'};
p10 = cellfun(@(x) loadpositdump(['../build/' x]),n10,'UniformOutput',false);

%%
qs=p10;
%%
c='rgbym';
s=[];
h=[];
for I=1:length(qs)
    p=qs{I};
    es=I-1;
    usenan=false;
    h(I)=plot(p(:,1),p(:,3),c(I));
    hold on
    plot(p(:,1),p(:,3),['*',c(I)]);
    s{end+1} =sprintf('positX es=%d',es);
    ylim([-10,10]);
end

hold off
legend(h,s);
%%
J=1;
qsr = qs{J}(abs(qs{J}(:,3))<4,:);
%hist(diff(qsr(:,3)));
hist(qsr(:,3));
%hist(diff(qs{1}(:,3)),50);

%%

%%
[u,h] = halfinrange(-4,4);                                                                                                                                                                                                
size(u)
plot(h)
hold on
plot(h,'*');