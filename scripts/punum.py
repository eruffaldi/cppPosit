# Emanuele Ruffaldi 2017

from functools import singledispatch
import fractions
import cmath
import numbers
from typing import Union,Optional
import math

@singledispatch
def exp(arg):
    pass

@singledispatch
def sqrt(arg):
    pass

@singledispatch
def pow(arg,n):
    pass

@singledispatch
def neg(arg):
    return -arg

@singledispatch
def inv(arg):
    return 1/arg

verbose = False

"""
class Alphabet3:
    def __init__(self):
        self.storagetype = "uint8"
        self.n = 3
        self.n2 = 8
    def exacts(self):
        return (1)
    def one(self):
        pass
    def zero(self):
        pass
    def inf(self):
        raise "implement inf of alphabet"

class Alphabet4:
    def __init__(self):
        self.storagetype = "uint16"
        self.n = 4
        self.n2 = 16
    def exacts(self):
        return (1,2)
    def one(self):
        pass
    def zero(self):
        pass
    def inf(self):
        raise "implement inf of alphabet"
"""

# This is generic
class Alphabet:
    def __init__(self,eexacts):
        if eexacts[0] != 1:
            raise Exception("Alphabet should start with 1")
        elif bin(len(eexacts)).count("1") != 1:
            raise Exception("Alphabet should be power of two and not %d" % len(eexacts))
        last = 0
        for x in eexacts:
            if x < 1 or x <= last:
                raise Exception("Alphabet should be of monotonic >= 1 got %s before %s" % (last,x))
            last = x
        self.eexacts = eexacts
        self.n = len(eexacts) 
        self.n2 = (len(eexacts)<<3)  # number of points
        # e.g. for having 32bits we need 2^29 exact points (!) 
        #   float has: 6 decimal digits exacts integers
        #   float has al 2^n exact
        #   how many?
        #   from here to 2^29 there is a long way
        # table cost is prohibitive
        self.mask = self.n2-1 # mask for 2^n
        self.storagetype = "auto"
        if verbose:
            print ("mask is ",bin(self.mask), " n2 is ",self.n2," n exacts ",self.n," exacts ",eexacts ,"infinity",bin(self.n2>>1))
        print ("mask is ",bin(self.mask), " n2 is ",self.n2," n exacts ",self.n," exacts ",eexacts ,"infinity",bin(self.n2>>1),"one",bin(self.n2>>2))
    def one(self):
        return self.rawpnum(self.n2>>2)
    def zero(self):
        return Pnum(self,0)
    def inf(self):
        return self.rawpnum(self.n2>>1)
    def pnnvalues(self):
        return self.n2
    def pnnmask(self):
        return self.mask
    def pnmod(self,x):
        return x & self.pnnmask()
    def next(self,x):
        return Pnum(self,(x + 1) & self.mask) # 1
    def prev(self,x):
        return Pnum(self,(x - 1) & self.mask) # 1
    def rawpnum(self,x):
        return Pnum(self,x & self.mask) # & self.pnmod(,
    def convert(self,value):
        if isinstance(value,numbers.Rational):
            return self._searchvalue(float(value))
        else:
            if cmath.isinf(value):
                return self.inf()
            elif cmath.isnan(value):
                raise Exception("cannot cast nan to pnum")
            else:
                return self._searchvalue(value)
    def fromexactsindex(self,idx):
        if verbose:
            print ("found at idx",idx,self.eexacts[idx])
        #index(one(T)) + (convert(storagetype(T), i - 1) << 1)
        iidx = (self.n2>>2)+((idx)<<1)
        return self.rawpnum(iidx)
    def _searchvalue(self,x):
        if x == 0:
            return self.zero()
        if x < 0:
            return -self.convert(-x)
        etable = self.eexacts
        lo = 0
        hi = len(etable)
        if verbose:
            print ("lookup table ",len(etable)," starting ",(lo,hi))
        if x < 1:
            while True:
                mid = lo + ((hi - lo) >> 1)
                if (mid == lo or mid == hi):
                    break
                lo, hi = (mid,hi) if (inv(etable[mid]) > x) else (lo, mid)
            if lo >= 0 and inv(etable[lo]) == x:
                return inv(self.fromexactsindex(lo))
            elif hi < len(etable) and inv(etable[hi]) == x:
                return inv(self.fromexactsindex(hi))
            elif lo == 0:
             return self.one().prev()
            elif hi >= len(etable):
                return self.zero().next()
            else:
                return inv((self.fromexactsindex(lo).next()))
        else:
            while True:
              mid = lo + ((hi - lo) >> 1)
              if (mid == lo or mid == hi):
                break
              lo, hi = (mid,hi) if (etable[mid] < x) else (lo, mid)

            if verbose:
                print ("found with",lo,hi)
            if lo >= 0 and etable[lo] == x: # exaxt low
                return self.fromexactsindex(lo)
            elif hi < len(etable) and etable[hi] == x: # exact high
                return self.fromexactsindex(hi)
            elif lo == 0: # low is first, then next of first
                return self.one().next()
            elif hi >= len(etable): # past use prev
                return self.inf().prev()
            else:
                return self.fromexactsindex(lo).next()
    @staticmethod
    def p1():
        return Alphabet((fractions.Fraction(1,1),))
    @staticmethod
    def p2():
        return Alphabet((fractions.Fraction(1,1),fractions.Fraction(2,1)))
    @staticmethod
    def p4():
        return Alphabet((fractions.Fraction(1,1),fractions.Fraction(2,1),fractions.Fraction(4,1),fractions.Fraction(8,1)))
    @staticmethod
    def p8():
        return Alphabet([fractions.Fraction(1<<k,1) for k in range(0,8)])
    @staticmethod
    def ppi8():
        pi = math.pi
        return Alphabet(sorted([1,pi,2*pi,3*pi/2,pi/2,3/4.0*math.pi,math.pi/3.0,math.pi*2/3.0]))
    @staticmethod
    def p8a():
        # use scaled by 4
        return Alphabet([fractions.Fraction(2**n*(4 + m),4) for m in range(0,4) for n in range(0,8)])
    @staticmethod
    def p16a():
        return Alphabet([fractions.Fraction(2**n*(256 + m),256) for m in range(0,256) for n in range(0,32)])

class Pnum:
    def __init__(self,base,v):
        self.base = base
        self.v = v
    def __mul__(self,other):
        return self.slowtimes(other)
    def __add__(self,other):
        return self.slowplus(other)
    def __div__(self,other):
        return self*(~other)
    def __sub__(self,other):
        return self+(-other)
    def __neg__(self):
        return self.base.rawpnum(-self.v)
    def __invert__(self): # -(x + index(inf))
        return self.base.rawpnum(-(self.v+(self.base.n2 >> 1)))
    def one(self):
        return self.base.one()
    def zero(self):
        return self.base.zero()
    def inf(self):
        return self.base.inf()
    def abs(self):
        return -self if self.isstrictlynegative() else self
    def fromindex(self,i):
        # generic index 0..n2
        return self.rawpnum(i)
    def fromexactsindex(self,i):
        # index 0..len(values)
        return self.rawpnum((self.n2 >> 2) +  ((i-1)<<1) )
    def exacts(self):
        return self.eexacts
    def iszero(self):
        return self.v == 0
    def isinf(self):
        return self.v == (self.base.n2 >> 1)
    def isexact(self):
        return (self.v & 1) == 0 # ubit
    def next(self):
        return self.base.next(self.v)
    def prev(self):
        return self.base.prev(self.v)
    def isstrictlynegative(self):
        return self.v > (self.base.n2>>1)# pinf
    def isfractional(self):
        w = self.abs().v
        return (w > 0) and (w < (self.base.n2>>2))
    def _exacttimes(self,other):
        rx = (self.exactvalue())
        ry = (other.exactvalue())
        return self.inf() if self.isinf() or other.isinf() else self.base.convert(rx*ry)
    def _exactplus(self,other):
        rx = (self.exactvalue())
        ry = (other.exactvalue())
        return self.inf() if self.isinf() or other.isinf() else self.base.convert(rx+ry)

    def slowsquare(self,other):
        ai = self.isinf()
        a0 = self.iszero()
        if a1:
            return Pbound.inf(self.base)
        if a0:
            return Pbound.zero(self.base)
        ae = self.isexact()
        be = other.isexact()
        abe = ae and be
        x = self
        y = self
        x1,x2 = (x,x) if ae else (x.prev(),x.next())
        y1,y2 = (y,y) if be else (y.prev(),y.next())
        if x.isstrictlynegative():
            x1,x2 = x2,x1
            y1,y2 = y2,y1
        z1 = x1._exacttimes(y1)
        #z1 = (!bothexact && isexact(z1)) ? nextpnum(z1) : z1
        z1 = z1.next() if (not abe and z1.isexact()) else z1
        z2 = z2.next() if (not abe and z2.isexact()) else z2
        return Pbound(z1,z2)
    # ritorns Pbound
    def slowtimes(self,other):
        ai = self.isinf()
        a0 = self.iszero()
        bi = other.isinf()
        b0 = other.iszero()
        if (ai and b0) or (a0 and bi):
            return Pbound.everything(self.base)
        elif (ai or bi):
            return Pbound.inf(self.base)
        elif (a0 or b0):
            return Pbound.zero(self.base)
        ae = self.isexact()
        be = other.isexact()
        abe = ae and be
        x = self
        y = other
        x1,x2 = (x,x) if ae else (x.prev(),x.next())
        y1,y2 = (y,y) if be else (y.prev(),y.next())
        if x.isstrictlynegative():
            x1,x2 = x2,x1
        if y.isstrictlynegative():
            y1,y2 = y2,y1
        z1 = x1._exacttimes(y1)
        z2 = x2._exacttimes(y2)
        #z1 = (!bothexact && isexact(z1)) ? nextpnum(z1) : z1
        z1 = z1.next() if (not abe and z1.isexact()) else z1
        z2 = z2.next() if (not abe and z2.isexact()) else z2
        return Pbound(z1,z2)
    def slowtwice(self):
        ai = self.isinf()
        if a:
            return Pbound.everything(self.base)
        if self.iszero():
            return self
        ae = self.isexact()
        abe = True
        x = self
        y = self
        x1,x2 = (x,x) if ae else (x.prev(),x.next())
        y1,y2 = (y,y) if be else (y.prev(),y.next())
        z1 = x1._exactplus(y1)
        z2 = x2._exactplus(y2)
        #z1 = (!bothexact && isexact(z1)) ? nextpnum(z1) : z1
        z1 = z1.next() if (not abe and z1.isexact()) else z1
        z2 = z2.next() if (not abe and z2.isexact()) else z2
        return Pbound(z1,z2)
    # ritorns Pbound
    def slowplus(self,other):
        ai = self.isinf()
        bi = other.isinf()
        if (ai and bi):
            return Pbound.everything(self.base)
        if (ai or bi):
            return Pbound.inf(self.base)
        if self.iszero():
            return other
        if other.iszero():
            return self
        ae = self.isexact()
        be = other.isexact()
        abe = ae and be
        x = self
        y = other
        x1,x2 = (x,x) if ae else (x.prev(),x.next()) # x1 x2 EXACT
        y1,y2 = (y,y) if be else (y.prev(),y.next()) # y1 y2 EXACT
        z1 = x1._exactplus(y1)
        z2 = x2._exactplus(y2)
        #z1 = (!bothexact && isexact(z1)) ? nextpnum(z1) : z1
        z1 = z1.next() if (not abe and z1.isexact()) else z1
        z2 = z2.next() if (not abe and z2.isexact()) else z2
        return Pbound(z1,z2)
    def exactvalue(self):
        # zero, infinity are special
        # fractional => integral and flip
        # negative => positive and flp resilts
        if self.isinf():
            return math.inf #fractions.Fraction(1,0)
        elif self.v == 0:
            return 0
        elif self.isstrictlynegative():
            return -((-self).exactvalue())
        elif self.isfractional():
            return inv(inv(self).exactvalue())
        else:
            # POSITIVE and NOT FRACTIONAL one..inf
            i1 = self.base.n2>>2
            return self.base.eexacts[((self.v - i1)>>1)] #index(x) - index(one(x))) >> 1) + 1
    def __str__(self):
        if self.isinf():
            return "pnum(inf)"
        else:
            e = "" if self.isexact() else "?"
            v = self.exactvalue()
            if not isinstance(v,fractions.Fraction):
                return "pnum%s(%s)" % (e,v)
            elif v.denominator == 1:
                return "pnum%s(%s)" % (e,v.numerator)
            elif v.numerator == 1:
                return "pnum%s(1/%s)" % (e,v.denominator)
            else:
                return "pnum%s(%s/%s)" % (e,v.numerator,v.denominator)
    def sqrt(self):
        if self.isstrictlynegative():
            return Pbound(True)
        xe = self.isexact()
        if xe:
            sr = self.base.convert(math.sqrt(self.exactvalue()))
            x1,x2 = x,x
            y1,y2 = sr
        else:
            x1,x2 =  (self.prev(),self.next())
            y1,y2 = self.base.convert(math.sqrt(x1.exactvalue())),self.base.convert(math.sqrt(x2.exactvalue()))

        if y1.isexact():
            if not xe:
                y1 = y1.next()
            elif y1*y1 != x1: #precompute exactness of sqrt
                y1 = y1.prev() 
        if y2.isexact():
            if not xe:
                y2 = y2.prev()
            elif y2*y2 != x2: #precompute exactness of sqrt
                y2 = y2.next()

        return Pbound(y1,y2)

    def pow(self,n):
        """
         T = typeof(x)
          xexact = isexact(x)

          # TODO converting to floats to avoid underflow/overflow problems.
          # Should figure out if there's a decent way to work exactly
          x1, x2 = xexact ? (x, x) : (prevpnum(x), nextpnum(x))
          if xexact
            y1 = y2 = T(convert(Float64, exactvalue(x1))^n)
          else
            y1, y2 = T(convert(Float64, exactvalue(x1))^n), T(convert(Float64, exactvalue(x2))^n)
          end

          if iseven(n) && isstrictlynegative(x)
            y1, y2 = y2, y1
          end

          y1 = !xexact && isexact(y1) ? nextpnum(y1) : y1
          y2 = !xexact && isexact(y2) ? prevpnum(y2) : y2

          Pbound(y1, y2)
          """
        pass

    def exp(self):
        if self.isinf():
            return Pbound(self.base.zero(),self.base.inf())
        xe = self.isexact()
        if xe:
            sr = self.base.convert(math.exp(self.exactvalue()))
            x1,x2 = x,x
            y1,y2 = sr
        else:
            x1,x2 =  (self.prev(),self.next())
            y1,y2 = self.base.convert(math.exp(x1.exactvalue())),self.base.convert(math.sqrt(x2.exactvalue()))

        if y1.isexact():
            if x1.iszero():
                y1 = y1 if xe else y1.next()
            elif y1.isinf():
                y1 = y1 if xe else y1.next()
            else:
                y1 = y1.prev()
        if y2.isexact():
            if x2.iszero():
                y2 = y2 if xe else y2.prev()
            elif y1.isinf():
                y2 = y2 if xe else y2.prev()
            else:
                y2 = y2.next()

        if x1.isinf():
            return Pbound(self.base.zero().next(),y2)
        elif x2.isinf():
            return Pboun(y1,self.base.inf().prev())
        else:
            return Pbound(y1,y2)
# interval using two Pnum
class Pbound:
    def __init__(self,first_or_empty:Union[Pnum,bool],last:Optional[Pnum]=None):
        #print ("Pbound",first_or_empty,last)
        if first_or_empty is True:
            self.empty = True
            self.v = (last,last)
        else:
            if not isinstance(first_or_empty,Pnum):
                raise Exception("expected Pnum")
            if last is not None and not isinstance(first_or_empty,Pnum):
                raise Exception("expected Pnum")
            self.empty = False
            self.v = (first_or_empty,first_or_empty if last is None else last)
    def complement(self):
        if self.empty:
            return Pbound.everything(self.base)
        elif self.iseverything():
            return Pbound.empty(self.base)
        else:
            return Pbound(self.v[1].next(),self.v[0].prev())
    def iter(self):
        if self.empty:
            return
        f,l = self.v
        yield f
        while f.v != l.v:
            f = f.next()
            yield f
    def iseverything(self):
        return self.v[0] == self.v[1].next()
    @property
    def base(self):
        return self.v[0].base
    @staticmethod
    def inf(base):
        return Pbound(base.inf())
    @staticmethod
    def one(base):
        return Pbound(base.one())
    @staticmethod
    def zero(base):
        return Pbound(base.zero())
    @staticmethod
    def empty(base):
        return Pbound(True,base.zero())
    @staticmethod
    def finite(base):
        return Pbound(base.inf().next(),base.inf().prev())
    @staticmethod
    def nonzero(base):
        return Pbound(base.zero().next(),base.zero().prev())
    @staticmethod
    def everything(base):
        return Pbound(base.zero(),base.zero().prev())

    def __str__(self):
        if self.empty:
            return "[]"
        elif self.iseverything():
            return "[all]"
        elif self.v[0].v == self.v[1].v:
            if self.v[0].isexact():
                return "[" + str(self.v[0].exactvalue())+"]"
            else:
                return "(%s,%s)" % (str(self.v[0].prev().exactvalue()),str(self.v[1].next().exactvalue()))
        else:
            if self.v[0].isexact():
                pre = "[" + str(self.v[0].exactvalue())
            else:
                pre = "(" + str(self.v[0].prev().exactvalue())
            if self.v[1].isexact():
                post = str(self.v[1].exactvalue()) + "]"
            else:
                post = str(self.v[1].next().exactvalue()) + ")"
            return "%s,%s" % (pre,post)
    def __neg__(self):
        if self.iseverything():
            return self.everything()
        else:
            return Pbound(~-elf.v[1],-self.v[1])
    def __invert__(self):
        if self.iseverything():
            return self.everything()
        else:
            return Pbound(~self.v[1],~self.v[0])

"""def asfloat(self):
    pass
def exp(self):
    #mapreduce(exp,union,Sopn,self.eachpnum)
    pass
def sqrt(self):
    #mapreduce(sqrt,union,Sopn,self.eachpnum)
    pass
def pow(self,n):
    pass"""

#https://github.com/jwmerrill/Pnums.jl/blob/master/src/ops.jl
"""
# set of Pnums
class Sopn:
    def __init__(self,x=None):
        # copy other Sopn
        # float
        pass
    def __mul__(self,other):
        # mapreduce(*,Sopn, eachpnum(self),eachpnum(other))
        pass
    def __div__(self,other):
        # mapreduce(/,Sopn, eachpnum(self),eachpnum(other))
        pass
    def __add__(self,other):
        # mapreduce(+,Sopn, eachpnum(self),eachpnum(other))
        pass
    def __sub__(self,other):
        # mapreduce(-,Sopn, eachpnum(self),eachpnum(other))
        pass
    def __neg__(self):
        # mapreduce((-),Sopn, eachpnum(self))
        pass
    def __ior_(self,p):
        # inplace or of the pvvoe
        pass
    def eachpnum(self):
        pass
    def asfloat(self):
        pass
    def exp(self):
        #mapreduce(exp,union,Sopn,self.eachpnum)
        pass
    def sqrt(self):
        #mapreduce(sqrt,union,Sopn,self.eachpnum)
        pass
    def pow(self,n):
        pass
    def indexlength(self,other):
        raise "TBD"
        return pnmod(other.v-self.v) # what?
    def next(self):
        raise "TBD"
    def prev(self):
        raise "TBD"
    def everything(self):
        return self == self.next()

@inv.register(Sopn)
def _(arg ):
    return arg.__invert__();

@pow.register(Sopn)
def _(arg,n):
    return arg.pow(n)

@exp.register(Sopn)
def _(arg):
    return self.exp()

@sqrt.register(Sopn)
def _(arg):
    return self.sqrt()

@neg.register(Sopn)
def _(arg):
    return -self
"""
@inv.register(Pnum)
def _(arg ):
    return arg.__invert__();

@pow.register(Pnum)
def _(arg,n):
    return arg.pow(n)

@exp.register(Pnum)
def _(arg):
    return self.exp()

@sqrt.register(Pnum)
def _(arg):
    return self.sqrt()

def main():
    pass

if __name__ == '__main__':
    main()