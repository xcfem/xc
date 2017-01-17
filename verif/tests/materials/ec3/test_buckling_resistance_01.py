# -*- coding: utf-8 -*-
''' Buckling resistance of of steel columns.
   Example 1 page 12 from publication:
   Eurocodes ‐ Design of steel buildings with worked examples
   Brussels, 16 - 17 October 2014
'''
from __future__ import division
import math
import xc_base
import geom
import xc
import scipy.interpolate


from materials import aceros_estructurales as steel
from materials import steelProfile as sp
from materials.perfiles_metalicos.arcelor import perfiles_he_arcelor as he

S355JR= steel.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
HEB340= he.HEProfile(S355JR,'HE_340_B')



#Check results pages 34 and 35
Leq= 4.335 # Buckling length
Nd= 3326e3 

XY= HEB340.getBucklingReductionFactorY(Leq,'c',1)
XZ= HEB340.getBucklingReductionFactorZ(Leq,'b',1)
XYTeor= 0.69
ratio1= abs(XY-XYTeor)/XY
NbRd= HEB340.getBucklingResistance(Leq,'c','b',1)
NbRdTeor= 4186.2e3
ratio2= abs(NbRd-NbRdTeor)/NbRd

# print 'XY= ', XY
# print 'XZ= ', XZ
# print 'ratio1= ', ratio1
# print 'NbRd= ', NbRd/1e3, ' kN'
# print 'ratio2= ', ratio2


import os
fname= os.path.basename(__file__)
if((ratio1<0.01) and (ratio2<0.01)):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
