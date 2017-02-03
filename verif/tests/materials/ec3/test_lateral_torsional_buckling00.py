# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams.
   Elaboración propia. '''
from __future__ import division
import math
import xc_base
import geom
import xc
import scipy.interpolate


from materials.ec3 import lateral_torsional_buckling as ltb


def caseASampleMoments(L,M,FF):
  nDiv= 20
  step= L/nDiv
  xx=(0,L)
  yy=(M,FF*M)
  f=scipy.interpolate.interp1d(xx, yy)
  x=list()
  M=list()
  xi= 0.0
  Mi= 0.0
  for i in range(1,nDiv+2):
    Mi= f(xi)
    x.append(xi)
    M.append(float(Mi))
    xi+= step
  return [x,M]


supportCoefs= ltb.SupportCoefficients(k1= 1.0,k2= 1.0)
nDiv= 20
step= 2/nDiv
psi=list()
c1=list()
psii= -1.0
C1i= 0.0
for i in range(1,nDiv+2): 
  mD= caseASampleMoments(5.0,10.0,psii)
  mgf= ltb.MomentGradientFactorC1(mD[0],mD[1])
  C1i= mgf.getC1(supportCoefs)
  psi.append(psii)
  c1.append(C1i)
  psii+= step

C1Teor=  [2.4494897427831783, 2.5202394341199024, 2.551789158017712, 2.539514968727602, 2.485250608738542, 2.3965787580611124, 2.284160962880643, 2.158748288883464, 2.0291986247835694, 1.9017865941611045, 1.7803963942770489, 1.6670801802983037, 1.5626453420774271, 1.4671286132146033, 1.3801311186847085, 1.301036207838119, 1.229142533193964, 1.1637407889826492, 1.1041549711386875, 1.0497621901901266, 1.0]
ratio1= 0
sz= len(C1Teor)
for i in range(0,sz):
  ratio1+= (c1[i]-C1Teor[i])**2

ratio1= math.sqrt(ratio1)

# print 'psi= ', psi
# print 'C1= ', c1
# print 'sz= ', sz
# print 'ratio1= ', ratio1

# import matplotlib.pyplot as plt
# plt.title('Case A')
# plt.xlim(-1.0, 1.0)
# plt.xlabel(r'Coefficient $\psi$')
# plt.ylim(0, 3)
# plt.ylabel(r'Moment gradient factor $C_1$')
# plt.plot(psi, c1)
# plt.show()

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
