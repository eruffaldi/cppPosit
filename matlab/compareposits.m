n8 = {'positint8_t,8,0,uint16_t,false.bin','positint8_t,8,1,uint16_t,false.bin','positint8_t,8,2,uint16_t,false.bin','positint8_t,8,3,uint16_t,false.bin'};
p8 = cellfun(@(x) loadpositdump(['../build/' x]),n8,'UniformOutput',false);

%%
n10= {'positint16_t,10,0,uint16_t,false.bin','positint16_t,10,1,uint16_t,false.bin','positint16_t,10,2,uint16_t,false.bin','positint16_t,10,3,uint16_t,false.bin'};
p10 = cellfun(@(x) loadpositdump(['../build/' x]),n10,'UniformOutput',false);

%%
n12= {'positint16_t,12,0,uint16_t,false.bin','positint16_t,12,1,uint16_t,false.bin','positint16_t,12,2,uint16_t,false.bin','positint16_t,12,3,uint16_t,false.bin'};
p12 = cellfun(@(x) loadpositdump(['../build/' x]),n12,'UniformOutput',false);

%%
[u,h] = halfinrange(-10,10);      
f16=zeros(length(u),4);
f16(:,1)=-length(u)/2:(length(u)/2-1);
f16(:,2)=u;
f16(:,3)=h;
f16(:,4)=meandiff(h);
f16={f16};

%%
qss={p8,p10,p12,f16};
%%
    colors =[[0 0.4470 0.7410];[0.8500 0.3250 0.0980];[0.9290 0.6940 0.1250];[0.4940 0.1840 0.5560]];

%%
for K=1:length(qss)
    qs=qss{K};
    figure;
    bits=log2(length(p));
    s=[];
    h=[];
    for I=1:length(qs)
        p=qs{I};
        bits=log2(length(p));
        es=I-1;
        if floor(bits) ~= bits
            isfloat16=1;
        else
            isfloat16=0;
        end
        usenan=false;
        h(I)=plot(p(:,1),p(:,3),'Color',colors(I,:));
        hold on
        plot(p(:,1),p(:,3),['*'],'Color',colors(I,:));
        if isfloat16
         s{I} =sprintf('float16');
        else
         s{end+1} =sprintf('posit%d es=%d',bits,es);
        end
    ylim([-10,10]);
    end
    yl =ylim;
    title(sprintf('Posits %d bits with Y limit %f %f',bits,yl(1),yl(2)));

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
for K=1:length(qss)
    qs=qss{K};
    figure;

    s={};
    h=[];
    for I=1:length(qs)
        p=qs{I};
        bits=log2(length(p));
        if floor(bits) ~= bits
            isfloat16=1;
        else
            isfloat16=0;
        end
        es=I-1;
        usenan=false;
        qs{I} = p;
        h(I)=plot(p(:,3),p(:,4),'Color',colors(I,:));
        hold on
        plot(p(:,3),p(:,4),['*'],'Color',colors(I,:));
        if isfloat16
        s{I} =sprintf('float16');
        else
        s{I} =sprintf('posit%d es=%d',bits,es);
        end
    end
    xlabel('Value');
    ylabel('Resolution'); 
    legend(h,s);
    hold off
    xlim([-10,10]);
end


%%

