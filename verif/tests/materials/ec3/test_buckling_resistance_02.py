# -*- coding: utf-8 -*-
''' Buckling resistance of of steel columns.
   Example taken from the web page:

   https://sdcverifier.com/benchmarks/eurocode-3-buckling-resistance-of-a-compression-member/
   Eurocode 3. Example 6.7: Buckling resistance of a compression member.
'''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec3 import EC3_materials

# Material
S355JR= EC3_materials.S355JR
gammaM0= 1.00
S355JR.gammaM= gammaM0 
steelShape= EC3_materials.CHSShape(S355JR,'CHS_244.5_10.0')

# Buckling curves according to table 6.2 of EC3-1-1.
bucklingCurve= steelShape.getBucklingCurve()
if(bucklingCurve=='a'):
    ratio0= 0
else:
    ratio0= 1

# Section class.
sectionClass= steelShape.getClassInCompression()
ratio1= abs(sectionClass-1)

# Compression strength of the section.
NcRd= steelShape.getNcRd()
refNcRd= 7370*355/1.0
ratio2= abs(NcRd-refNcRd)/refNcRd

# Buckling reduction factors.
Leq= 4.0
XY= steelShape.getBucklingReductionFactorY(Leq, sectionClass= sectionClass)
XZ= steelShape.getBucklingReductionFactorZ(Leq, sectionClass= sectionClass)
ratio3= math.sqrt((XY-0.877783874726401)**2+(XZ-0.877783874726401)**2)

# Buckling resistance.
NbRd= steelShape.getBucklingResistance(Leq,Leq,1)
refNbRd= 0.8777838747264016*refNcRd
ratio4= abs(NbRd-refNbRd)/refNbRd

'''
print('ratio0= ', ratio0)
print('section class: ', sectionClass)
print('ratio1: ', ratio1)
print('buckling curve: ', bucklingCurve)
print('cross-section NcRd= ',NcRd/1e3, 'kN')
print('cross-section reference NcRd= ',refNcRd/1e3, 'kN')
print('ratio2: ', ratio2)
print('XY= ', XY)
print('XZ= ', XZ)
print('ratio3: ', ratio3)
print('NbRd= ', NbRd/1e3, ' kN')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio0==0) and (ratio1<1e-5) and (ratio2<1e-5)and (ratio3<1e-5) and (ratio4<1e-5)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
