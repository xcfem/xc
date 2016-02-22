# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams.
   pages 32-34 Example 2 from:
   Eurocodes ‐ Design of steel buildings with worked examples
   Brussels, 16 - 17 October 2014
'''
from __future__ import division
import math
import xc_base
import geom
import xc
import scipy.interpolate


from materials.ec3 import lateral_torsional_buckling as ltb
from materials import aceros_estructurales as steel
from materials.perfiles_metalicos.arcelor import perfiles_ipe_arcelor as IPE_profiles
from rough_calculations import ng_simple_beam as sb

S355JR= steel.S355JR
gammaM0= 1.05
S355JR.gammaM= gammaM0 
IPE400= IPE_profiles.IPEProfile(S355JR,"IPE_400")


# Geometry
k1= 1.0; k2= 1.0

#Check results pages 34 and 35
L= 3 # Bar length (m)
x= [0.0,0.25*L,0.5*L,0.75*L,1.0*L]
M= [-93.7e3,-93.7e3/2.0,0.0,114.3e3/2.0,114.3e3]
mgf= ltb.MomentGradientFactorC1(x,M)
overlineLambdaLT= IPE400.getNonDimensionalBeamSlenderness(1,x,M)
alphaLT= IPE400.getImperfectionFactor()
# phiLT= IPE400.getIntermediateFactor(1,x,M)
# chiLT= IPE400.getReductionFactor(1,x,M)
# chiLT= IPE400.getReductionFactor(1,x,M)
MbRd= IPE400.getLateralTorsionalBuclingResistance(1,x,M)
MbRdTeor= 412.9e3
ratio1= abs(MbRd-MbRdTeor)/MbRdTeor
ratio2= abs(overlineLambdaLT-0.51)/0.51

# print 'overlineLambdaLT= ', overlineLambdaLT
# print 'alphaLT= ', alphaLT
# print 'phiLT= ', phiLT
# print 'chiLT= ', chiLT
# print 'MbRd= ', MbRd/1e3, 'kN m'
# print 'ratio1= ', ratio1
# print 'ratio2= ', ratio2

import os
fname= os.path.basename(__file__)
if(ratio1<0.05 and ratio2<0.01):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
