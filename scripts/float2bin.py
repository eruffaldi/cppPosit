import numpy as np
import binascii


types = (np.float16,np.float32,np.float64,np.float128);

for t in types:
	inf = np.array(np.inf,dtype=t)
	nan = np.array(np.nan,dtype=t)
	zero = np.zeros(1,dtype=t)
	one = np.ones(1,dtype=t)
	for x,y in zip((-inf,inf,nan,one,2*one),("-inf","inf","nan","one","two")):
		bb = x.byteswap().tobytes()
		print "%30s %10s" % (t,y),"0x" + binascii.hexlify(bb)