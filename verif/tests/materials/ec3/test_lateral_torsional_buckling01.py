# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams.
   Example 1 from:
   Calculating the C1 factor for lateral torsional buckling.
   Modern steel construction. Nov/Dec 2013
   David Brown
'''
from __future__ import division
import math
import xc_base
import geom
import xc
import scipy.interpolate
from materials.ec3 import lateral_torsional_buckling as ltb

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


supportCoefs= ltb.SupportCoefficients(k1= 1.0,k2= 1.0)
x= [0.0,0.25,0.5,0.75,1.0]
M= [50,122.5,105,-2.5,-200]
mgf1= ltb.MomentGradientFactorC1(x,M)
C11= mgf1.getC1(supportCoefs)
C11Teor= math.sqrt(35*200**2/(200**2+9*122.5**2+16*105**2+9*2.5**2))
x= [0.0,0.25,0.4,0.5,0.75,1.0]
M= [150,260,283.5,280,210,50]
mgf2= ltb.MomentGradientFactorC1(x,M)
C12= mgf2.getC1(supportCoefs)
C12Teor= math.sqrt(35*283.5**2/(283.5**2+9*260**2+16*280**2+9*210**2))

ratio1= abs(C11-C11Teor)/C11Teor
ratio2= abs(C12-C12Teor)/C12Teor

# print 'C1= ', C11
# print 'C1Teor= ', C11Teor
# print 'ratio1= ', ratio1
# print 'C1= ', C12
# print 'C1Teor= ', C12Teor
# print 'ratio2= ', ratio2

import os
fname= os.path.basename(__file__)
if(ratio1<0.05 and ratio2<0.05):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
