# -*- coding: utf-8 -*-
''' SHS subject to combined compression and bi-axial bending.
   
   Test based on the example 5 of the publication: SCI PUBLICATION P374 «Steel Building Design: Worked Examples - Hollow Sections. In accordance with Eurocodes and the UK National Annexes» M E Brettle. 2008. SCI Steel Knowledge. ISBN 979-1-85942-161-1
'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec3 import EC3_materials

steelMaterial= EC3_materials.S355JR
# Partial factors for resistance
gammaM0= 1.0
gammaM1= 1.0
steelMaterial.gammaM= gammaM0
# Steel shape.
steelShape= EC3_materials.HFSHSShape(steel= steelMaterial, name='HFSHS150x150x6.3')
# Cross section classification (in compression: conservative criteria).
steelShape.sectionClass= steelShape.getClassInternalPartInCompression()
sectionClassOK= (steelShape.sectionClass==1)

# Bending and axial forces.
Nd= -600e3 # Design compression force.
Mzd= 20e3 # Design Moment about the z-z axis (major axis)
Myd= 5e3 # Design Moment about the y-y axis (minor axis)

## Design resistance of the cross-section for uniform compression
NcRd= steelShape.getNcRd()
refNcRd= 1271e3
ratio1= abs(NcRd-refNcRd)/refNcRd

# Design bending moment resistance of the cross section
McRdy= steelShape.getMcRdy()
refMcRdy= 68e3
ratio2= abs(McRdy-refMcRdy)/refMcRdy

McRdz= steelShape.getMcRdz()
refMcRdz= McRdy
ratio3= abs(McRdz-refMcRdz)/refMcRdz

bendingFactorY, bendingFactorZ= steelShape.getBendingFactors(nCF= abs(Nd/NcRd))
ratio4= abs(bendingFactorY-0.69)/0.69
ratio5= abs(bendingFactorZ-bendingFactorY)/bendingFactorY

efficiency= steelShape.getBiaxialBendingEfficiency(Nd= Nd, Myd= Myd, Mzd= Mzd)
ratio6= abs(efficiency[0]-0.47767666284071286)/0.47767666284071286

'''
print(sectionClassOK)
print('NcRd= ', NcRd/1e3, 'kN')
print('ratio1= ', ratio1)
print('McRdy= ', McRdy/1e3, 'kN')
print('ratio2= ', ratio2)
print('McRdz= ', McRdz/1e3, 'kN')
print('ratio3= ', ratio3)
print('bendingFactorY= ', bendingFactorY)
print('ratio4= ', ratio4)
print('bendingFactorZ= ', bendingFactorZ)
print('ratio5= ', ratio5)
print(efficiency)
print('ratio6= ', ratio6)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(sectionClassOK and ratio1<1e-3 and ratio2<1e-2 and ratio3<1e-5 and ratio4<1e-2 and ratio5<1e-5 and ratio6<1e-4):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
