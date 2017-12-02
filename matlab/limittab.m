function t =limittab(t,mi,ma,mode)

if iscell(t)
    for I=1:length(t)
        t{I} = limittab(t{I},mi,ma,mode);
    end
else
    
    for I=1:height(t)
        v = t.float{I};
        if strcmp(mode,'exclusive')
            ba =v>mi & v<ma;
        else
            ba =v>=mi & v<=ma;
        end
        t.signed{I} =t.signed{I}(ba);
        t.float{I} =t.float{I}(ba);
        t.unsigned{I} =t.unsigned{I}(ba);
        t.res{I} =t.res{I}(ba);
    end
t =describetab(t);

end
