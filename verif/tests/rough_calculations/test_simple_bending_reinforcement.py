# -*- coding: utf-8 -*-
from rough_calculations import ng_simple_bending_reinforcement

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

M=27*9810
fcd= 21e6
fsd= 420e6
b= 0.3
d= 0.44

As= ng_simple_bending_reinforcement.AsSimpleBending(M,fcd,fsd,b,d)
T= As*fsd
xpl= T/0.85/fcd/b
z= d-xpl/2.0
C= 0.85*fcd*b*xpl
ratio1= (T-C)/T
ratio2= (M-T*z)/M


# print "As= ", As*1e4," cm2"
# print "T= ", T/1e3," kN"
# print "xpl= ", xpl," m"
# print "z= ", z," m"
# print "C= ", C/1e3," kN"
# print "ratio1= ",ratio1
# print "ratio2= ",ratio2


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio1)<1e-10 and abs(ratio2)<1e-10):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

