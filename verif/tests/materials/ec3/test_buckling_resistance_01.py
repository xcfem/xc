# -*- coding: utf-8 -*-
''' Buckling resistance of steel columns.
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

# Material
S355JR= EC3_materials.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
steelShape= EC3_materials.HEShape(S355JR,'HE_340_B')
steelShape.sectionClass= 1

#Check results pages 34 and 35
Leq= 4.335 # Buckling length
Nd= 3326e3

# Buckling curves according to table 6.2 of EC3-1-1.
yBucklingCurve= steelShape.getBucklingCurve(majorAxis= False)
zBucklingCurve= steelShape.getBucklingCurve(majorAxis= True)
if((yBucklingCurve=='c') and (zBucklingCurve=='b')):
    ratio0= 0
else:
    ratio0= 1
    
# Buckling reduction factors.
XY= steelShape.getBucklingReductionFactorY(Leq)
XZ= steelShape.getBucklingReductionFactorZ(Leq)
XYTeor= 0.69
ratio1= abs(XY-XYTeor)/XY

# Buckling resistance.
NbRd= steelShape.getBucklingResistance(Leq,Leq)
NbRdTeor= 4186.2e3
ratio2= abs(NbRd-NbRdTeor)/NbRd

# print('ratio0= ', ratio0)
# print('XY= ', XY)
# print('XZ= ', XZ)
# print('ratio1= ', ratio1)
# print('NbRd= ', NbRd/1e3, ' kN')
# print('ratio2= ', ratio2)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0==0) and (ratio1<0.01) and (ratio2<0.01)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
