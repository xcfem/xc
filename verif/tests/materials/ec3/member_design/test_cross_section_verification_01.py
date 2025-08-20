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

steelMaterial= EC3_materials.S355JR
gammaM0= 1.0
steelMaterial.gammaM= gammaM0 
steelShape= EC3_materials.IPEShape(steelMaterial,"IPE_400")
steelShape.sectionClass= 1

# Geometry
k1= 1.0; k2= 1.0

#Check results page 31
Md= 114.3e3
McRd= steelShape.getMcRdz()
McRdTeor= 464.0e3
ratio1= abs(McRd-McRdTeor)/McRdTeor
resCheckBending= steelShape.getBiaxialBendingEfficiency(Nd= 0.0,Myd= 0.0,Mzd= Md)
bendingEfficiency= resCheckBending[0]
bendingEfficiencyTeor= (Md/McRdTeor)**2
ratio2= abs(bendingEfficiency-bendingEfficiencyTeor)/bendingEfficiencyTeor
Vd= 75.9e3
VcRd= steelShape.getVcRdy()
VcRdTeor= 875e3
ratio3= abs(VcRd-VcRdTeor)/VcRdTeor
MvRd= steelShape.getMvRdz(Vd= Vd)
MvRdTeor= 464.0e3
ratio4= abs(MvRd-MvRdTeor)/MvRdTeor

'''
print('McRd= ', McRd/1e3, 'kN m')
print('McRdTeor= ', McRdTeor/1e3, 'kN m')
print('ratio1= ', ratio1)
print('bendingEfficiency= ', bendingEfficiency)
print('bendingEfficiencyTeor= ', bendingEfficiencyTeor)
print('ratio2= ', ratio2)
print('VcRd= ', VcRd/1e3, 'kN m')
print('VcRdTeor= ', VcRdTeor/1e3, 'kN m')
print('ratio3= ', ratio3)
print('MvRd= ', MvRd/1e3, 'kN m')
print('MvRdTeor= ', MvRdTeor/1e3, 'kN m')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-4 and ratio2<1e-4 and ratio3<1e-4 and ratio4<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
