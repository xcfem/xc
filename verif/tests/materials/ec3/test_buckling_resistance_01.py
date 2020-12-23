# -*- coding: utf-8 -*-
''' Buckling resistance of of steel columns.
   Example 1 page 12 from publication:
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
from materials.sections.structural_shapes import arcelor_metric_shapes

S355JR= EC3_materials.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
HEB340= arcelor_metric_shapes.HEShape(S355JR,'HE_340_B')



#Check results pages 34 and 35
Leq= 4.335 # Buckling length
Nd= 3326e3 

XY= HEB340.getBucklingReductionFactorY(Leq,'c',1)
XZ= HEB340.getBucklingReductionFactorZ(Leq,'b',1)
XYTeor= 0.69
ratio1= abs(XY-XYTeor)/XY
NbRd= HEB340.getBucklingResistance(Leq,Leq,'c','b',1)
NbRdTeor= 4186.2e3
ratio2= abs(NbRd-NbRdTeor)/NbRd

# print('XY= ', XY)
# print('XZ= ', XZ)
# print('ratio1= ', ratio1)
# print('NbRd= ', NbRd/1e3, ' kN')
# print('ratio2= ', ratio2)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<0.01) and (ratio2<0.01)):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
