# -*- coding: utf-8 -*-
''' Anchorage of tendons for the ultimate limit state. Home made test.'''

__author__= "Luis Claudio Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials

# Materials.
concrete= EC2_materials.C40 # concrete.
prestressingSteel= EC2_materials.Y1860S7 # prestressig steel.

# Strand
strandDiameter= 15.2e-3 # diameter of the strand.
strandArea= 141e-6 # area of the strand.
#strandStress= 317.01e3/strandArea
strandStress= 255e3/strandArea

fbpd= prestressingSteel.getUltimateBondStress(concrete= concrete, bondConditionsCoeff= 1.0, tendonTypeCoeff= 1.2)
L= prestressingSteel.getAnchorageLength(transmissionLength= 0.0, concrete= concrete, tendonDiameter= strandDiameter, sigma_pd= strandStress, sigma_pminf= 0.0, bondConditionsCoeff= 1.0, tendonTypeCoeff= 1.2, alpha2= 0.19)
Lref= 2.6580857104242384
ratio1= abs(L-Lref)/Lref

'''
print('Ultimate bond stress: fbpd= ', fbpd/1e6, 'MPa')
print('Anchorate length: L= ', L, ' m')
'''

import os
fname= os.path.basename(__file__)
if(ratio1<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
