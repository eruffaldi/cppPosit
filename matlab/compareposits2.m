addpath halfprecision/
n8 = arrayfun(@(x) {8,x,sprintf('positint8_t,8,%d,uint16_t,false.bin',x)},0:3,'UniformOutput',false);
p8 = cellvcat(cellfun(@(x) loadpositdump(['../build/' x{3}],x{1},x{2}),n8,'UniformOutput',false));

%%
n10 = arrayfun(@(x) {10,x,sprintf('positint16_t,10,%d,uint16_t,false.bin',x)},0:3,'UniformOutput',false);
 p10 = cellvcat(cellfun(@(x) loadpositdump(['../build/' x{3}],x{1},x{2}),n10,'UniformOutput',false));

%%
n12 = arrayfun(@(x) {12,x,sprintf('positint16_t,12,%d,uint16_t,false.bin',x)},0:4,'UniformOutput',false);
p12 = cellvcat(cellfun(@(x) loadpositdump(['../build/' x{3}],x{1},x{2}),n12,'UniformOutput',false));

%%  
[u,h] = halfinrange(-10,10);      
tf16=dump2table([-length(u)/2:(length(u)/2-1);cast(u,'double');h]');
tf16.bits=16;
tf16.es=0;
tf16.what=categorical({'float'},{'posit','float','valid'})
f16=tf16;
%%
p12e=[f16;p12];
p10e=[f16;p10];

%%
qss1={[p8;p10;p12;f16]};
qss1=limittab(qss1,-2,2,'exclusive');
qss={p8,p10,p12,f16};
qss=limittab(qss,-2,2,'exclusive');
%%
p81=qss{1};
p8z =p81(1,:);
%%
    colors =[[0 0.4470 0.7410];[0.8500 0.3250 0.0980];[0.9290 0.6940 0.1250];[0.4940 0.1840 0.5560];[0,1,1]];

%%
for K=1:length(qss)
    qs=qss{K};
    figure;
    s=[];
    h=[];
    for I=1:height(qs)
        h(I)=plot(qs.signed{I},qs.float{I},'Color',colors(mod(I,length(colors))+1,:));
        hold on
        plot(qs.signed{I},qs.float{I},['*'],'Color',colors(mod(I,length(colors))+1,:));
        if qs.what(I)=='float'
         s{I} =sprintf('float16');
        else
         s{end+1} =sprintf('posit%d es=%d',qs.bits(I),qs.es(I));
        end
    ylim([-1,1]);
    end
    yl =ylim;

    hold off
    legend(h,s);
end

%%

% 
% size(u)
% plot(h)
% hold on
% plot(h,'*');
% yl =ylim;
% title(sprintf('float16 in range %f %f',yl(1),yl(2)));

%%
qss={p10e,f16};
qss=limittab(qss,-1,1,'exclusive');

%%
for K=1:length(qss)
    qs=qss{K};
    figure;

    s={};
    h=[];
    for I=1:height(qs)
        bits =qs.bits(I);
        isfloatx = qs.what(I) == 'float';
        es=qs.es(I);
%        if es >0
 %           continue
  %      end
        usenan=false;
        h(I)=plot(qs.float{I},qs.res{I},'Color',colors(I,:));
        hold on
        plot(qs.float{I},qs.res{I},['.'],'Color',colors(I,:));
        if qs.what(I) =='float'
            s{I} =sprintf('float%d',bits);
        else
            s{I} = sprintf('posit%d es %d',bits,es);
        end
    end
    xlabel('Value');
    ylabel('Resolution');
    legend(h,s);
    hold off
    xlim([-1,1]);
end
