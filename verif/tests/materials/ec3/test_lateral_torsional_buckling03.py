# -*- coding: utf-8 -*-
''' Lateral torsional buckling of steel beams.
   pages 32-34 Example 2 from:
   Eurocodes ‐ Design of steel buildings with worked examples
   Brussels, 16 - 17 October 2014
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials
from rough_calculations import ng_simple_beam as sb
from geom_utils import interpolation as intp

S355JR= EC3_materials.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
IPE400= EC3_materials.IPEShape(S355JR,"IPE_400")


# Geometry
k1= 1.0; k2= 1.0

#Check results pages 34 and 35
L= 3 # Bar length (m)
x= [0.0,0.25*L,0.5*L,0.75*L,1.0*L]
M= [-93.7e3,-93.7e3/2.0,0.0,114.3e3/2.0,114.3e3]
Mi=intp.interpEquidistPoints(xi=x,yi=M,nDiv=4)
overlineLambdaLT= IPE400.getLateralBucklingNonDimensionalBeamSlenderness(1,L,Mi)
alphaLT= IPE400.getLateralBucklingImperfectionFactor()
# phiLT= IPE400.getLateralBucklingIntermediateFactor(1,x,M)
# chiLT= IPE400.getLateralBucklingReductionFactor(1,x,M)
# chiLT= IPE400.getLateralBucklingReductionFactor(1,x,M)
MbRd= IPE400.getLateralTorsionalBucklingResistance(1,L,Mi)
MbRdTeor= 412.9e3
ratio1= abs(MbRd-MbRdTeor)/MbRdTeor
ratio2= abs(overlineLambdaLT-0.51)/0.51

# print('overlineLambdaLT= ', overlineLambdaLT)
# print('alphaLT= ', alphaLT)
# print('phiLT= ', phiLT)
# print('chiLT= ', chiLT)
# print('MbRd= ', MbRd/1e3, 'kN m')
# print('ratio1= ', ratio1)
# print('ratio2= ', ratio2)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<0.05 and ratio2<0.01):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
