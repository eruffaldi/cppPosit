#
# Properties of summation table
#
# Given only points on lattice (a,b) >= 1
#
# lattice size n --> unum 3+n tbis
#
# (a,-a,1/a,-1/a) + (b,-b,1/b,-1/b) 
#
# [   a + b,     a - b,   a + 1/b,     a - 1/b]
# [   b - a,   - a - b,   1/b - a,   - a - 1/b]
# [ b + 1/a,   1/a - b, 1/a + 1/b,   1/a - 1/b]
# [ b - 1/a, - b - 1/a, 1/b - 1/a, - 1/a - 1/b]
#
# [ 1, 2, 3, 4; -2, -1, -4, -3; 5, 6, 7, 8; -6, -5, -8, -7]
#
# a+b
# a-b
# a+1/b = (ab+1)/b
# a-1/b = (ab-1)/b
# b+1/a = (ab+1)/a
# b-1/a = (ab-1)/a
# 1/a+1/b = (a+b)/ab deived
# 1/a-1/b = (b-a)/ab derived
#
# But then we can consider our magic properties so tat
#
# 
#
# Taking all pairs (a,b) with a <= b we have n(n+1)/2 pairs each of 8 cases => 8n (n+1)/2
#
# e.g. 16bit as n=13 we have 13*8 (13+1)/2 instead of 
#
# Emanuele Ruffaldi 2017


import punum
import argparse
import fractions
import operator
import tabulate


def main():
    t1 =["half","twice","square","exp2","float"]
    t2 =["times","plus"]
    parser = argparse.ArgumentParser(description='Table generator')
    parser.add_argument('--points',nargs="+",type=int,help="lattice points")
    parser.add_argument('--op1',choices=t1,nargs="+",default=t1)
    parser.add_argument('--op2',choices=t2,nargs="+",default=t2)
    parser.add_argument('--p3',action="store_true")
    parser.add_argument('--p4',action="store_true")
    parser.add_argument('--p5',action="store_true")
    parser.add_argument('--p8',action="store_true")
    parser.add_argument('--p8a',action="store_true")
    parser.add_argument('--p16a',action="store_true")
    parser.add_argument('--verbose',action="store_true")
    parser.add_argument('--sparsetab')
    parser.add_argument('--pickle')
    args = parser.parse_args()
    if args.points is not None:
        if args.points[0] != 1:
            args.points = [1] + args.points
        args.points.sort()
        alpha = punum.Alphabet(args.points)
    else:
        if args.p3:
            alpha = punum.Alphabet.p3()
        elif args.p4:
            alpha = punum.Alphabet.p4()
        elif args.p5:
            alpha = punum.Alphabet.p5()
        elif args.p8:
            alpha = punum.Alphabet.p8()
        elif args.p8a:
            alpha = punum.Alphabet.p8a()
        elif args.p16a:
            alpha = punum.Alphabet.p16a()


    print ("with",len(alpha.eexacts),"obtain",alpha.n," and ",alpha.n2)
    print (alpha.eexacts)


    # from arguments to operation groups
    ops1=[]
    ops2=[]
    asfloat=False
    for aop in args.op2:
        if aop == "plus":
            op = operator.add
        elif aop == "times":
            op = operator.mul
        else:
            print ("skipping op",aop)
            continue
        ops2.append((aop,op))

    two = alpha.convert(2)
    for aop in args.op1:
        if aop == "half":
            op = lambda x: x/two
        elif aop == "twice":
            op = lambda x: x*two
        elif aop == "square":
            op = lambda x: x*x
        elif aop == "exp2":
            op = lambda x: math.exp2(x)
        elif aop == "float":
            asfloat=True
            continue
        else:
            print ("unknown op",aop)
            continue
        ops1.append((aop,op))

    # prepare the outputs with flag and array
    outputs = {}
    for aop,op in ops2:
        outputs[aop] = (2,[])
    for aop,op in ops1:
        outputs[aop] = (1,[])
    if asfloat:
        outputs["float"] = (-1,[])
    # also listing
    outputs["exacts"] = (0,alpha.eexacts)

    # enumerate all the exacts (i is correctly orderer 1/4)
    for i,ae in enumerate(alpha.eexacts):
        a = alpha.fromexactsindex(i) # fraction
        ia = ~a

        # direct conversion table
        if asfloat:
            outputs["float"][1].append(dict(a=a,ai=a.v,y=a.exactvalue()))
            outputs["float"][1].append(dict(a=ia,ai=ia.v,y=ia.exactvalue()))

        # unaries for the 4 cases: +- 1/x -1/x
        for aop,op in ops1:
            q = outputs[aop][1]
            ma =-a;
            mia=-ia;
            y = op(a)
            iy = op(ia)
            my = op(ma)
            miy = op(mia)
            # 4 variants
            q.append(dict(a=a,ai=a.v,yi=y.v,y=y.exactvalue()))
            q.append(dict(a=ma,ai=ma.v,yi=my.v,y=my.exactvalue()))
            q.append(dict(a=ia,ai=ia.v,yi=iy.v,y=iy.exactvalue()))
            q.append(dict(a=mia,ai=mia.v,yi=miy.v,y=miy.exactvalue()))

        for j,be in enumerate(alpha.eexacts):
            if j < i:
                continue
            b = alpha.fromexactsindex(j)
            # 8 cases 
            ib = ~b
            if i == 0:
                if j == 0:
                    # exactly 1+1 or 1-1 
                    # keep first v < second
                    whats = [(a,b),(-a,b)]
                else:
                    # a is 1, j > 1
                    # keep first v < second
                    whats = [(a,b),(a,-b),(-a,-ib),(a,-ib)]
            elif i == j:
                whats = [(a,a),(a,-a),(ia,a),(ia,ia),(a,-ia)]
            else:
                # both > 1
                whats = [(a,b),(a,-b),(ia,b),(-a,-ib),(-ia,-ib),(ia,-ib),(ia,-b),(a,-ib)]
            for k,(xa,xb) in enumerate(whats):
                print (xa.v,xb.v,ae,be,k,len(whats))
                x1 = xa.exactvalue()
                x2 = xb.exactvalue()
                for aop,op in ops2:
                    rr = outputs[aop][1]
                    y = op(x1,x2)
                    uy = alpha.convert(y)
                    rr.append(dict(ai=xa.v,bi=xb.v,a=x1,b=x2,y=y,yi=uy.v))
    if args.pickle:
        outputs["two"] = (-2,[two.v,inv(two).v])
        pickle.dump(outputs,open(args.pickle,"wb"),protocol=pickle.HIGHEST_PROTOCOL)
    elif args.sparsetab:
        for x,y in outputs.items():
            o = open(args.sparsetab+x,"w",encoding="utf8")
            mode,content = y
            if mode == 2:
                # binop
                for x in content:
                    o.write("%d %d %d\n" % (x["ai"],x["bi"],x["yi"]))
            elif mode == 1:
                # unaryop
                for x in content:
                    o.write("%d %d\n" % (x["ai"],x["yi"]))
            elif mode == -1:
                # float
                for x in content:
                    o.write("%d %f\n" % (x["ai"],x["y"]))
            elif mode == 0:
                # exact list
                for x in y[1]:
                    o.write("%d\n" % (x["ai"]))
    else:
        for x,y in outputs.items():
            mode,content = y
            print ("\n",x,mode,"\n")
            print (tabulate.tabulate(content))




if __name__ == '__main__':
    main()