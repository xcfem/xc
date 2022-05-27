# -*- coding: utf-8 -*-
''' Sofistik Benchmark Example No. 10: Shear between web and flanges 
    of T-sections. Sofistik 2018.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections import section_properties
from rough_calculations import ng_simple_beam as sb

# Materials.
concrete= EC2_materials.C20
reinfSteel= EC2_materials.S500A

# Geometry
## Section.
webWidth= .4
webHeight= .6
flangeThickness= .15
effectiveFlangeWidth= 1.75
crossSectionGeometry= section_properties.TSection(name= '', webWidth= webWidth, webHeight= webHeight, flangeWidth= effectiveFlangeWidth, flangeThickness= flangeThickness, chamferSide= 0.0)
effectiveDepth= 0.68
innerLeverArm= 0.967*effectiveDepth # See "calculating the longitudinal reinforcement" in the example.

flangeStirrupsSpacing= .15
flangeStirrupsSteel= reinfSteel

## Beam.
beam= sb.SimpleBeam(E= concrete.Ecm(), I= crossSectionGeometry.Iz(), l= 10)


# Load.
Pg= 155e3 # 155 kN/m

# Bending moment.
x0= 0.0
x1= 1.0
DeltaX= x1-x0 # length under consideration (see figure 6.7 on EC2:2004).
MEdx0= beam.getBendingMomentUnderUniformLoad(q= -Pg, x= x0)
MEdx1= beam.getBendingMomentUnderUniformLoad(q= -Pg, x= x1)

# Shear between flange and web.
flangeArea= flangeThickness*effectiveFlangeWidth
flangeOverhangArea= flangeThickness*(effectiveFlangeWidth-webWidth)/2.0
areaRatio= flangeOverhangArea/flangeArea
## Compression increment.
flangeCompIncr= (MEdx1-MEdx0)/innerLeverArm
flangeOverhangCompIncr= flangeCompIncr*areaRatio
flangeOverhangCompIncrRef= 409.14245739660913e3 # 409.36 kN in the Sofistik document.
ratio1= abs(flangeOverhangCompIncr-flangeOverhangCompIncrRef)/flangeOverhangCompIncrRef
## Longitudinal shear stress.
longitudinalShearStress= flangeOverhangCompIncr/flangeThickness/DeltaX
longitudinalShearStressRef= 2.727616382644061e6 # 2.72 MPa in the Sofistik document.
## Flange struts crushing
flangeStrutAngle= math.radians(38.66) # 31.75 in the Sofistik document (computed according German national annex).
flangeStrutAngleSftk= math.radians(31.7) # Sofistik document value.
flangeStrutAngleRatio= math.tan(flangeStrutAngle)/math.tan(flangeStrutAngleSftk)
vRdMax= EC2_limit_state_checking.getMaximumShearFlangeStrutCrushingStress(concrete, flangeStrutAngle, nationalAnnex= None)
VRdMax= vRdMax*DeltaX*flangeThickness
VRdc= EC2_limit_state_checking.getConcreteFlangeShearStrength(concrete, hf= flangeThickness, DeltaX= DeltaX, nationalAnnex= None)
VRdcRef= 61.89172917715849e3 # 61.2 kN in the Sofistik document.
ratio2= abs(VRdc-VRdcRef)/VRdcRef
adjustedAsf= 5.79e-4*flangeStirrupsSpacing*flangeStrutAngleRatio
vRds= EC2_limit_state_checking.getFlangeShearResistanceShearReinfStress(concrete, hf= flangeThickness, Asf= adjustedAsf, sf= flangeStirrupsSpacing, shearReinfSteel= flangeStirrupsSteel, flangeStrutAngle= flangeStrutAngle, compressionFlange= True, nationalAnnex= None)
VRds= vRds*DeltaX*flangeThickness
VRdsRef= 407.6003879821235e3 # 409.36 kN in the Sofistik document.
ratio3= abs(VRds-VRdsRef)/VRdsRef

'''
print('flange overhang compression increment: ', flangeOverhangCompIncr/1e3, 'kN')
print('ratio1= ', ratio1)
print('longitudinal shear stress: vEd= ', longitudinalShearStress/1e6, 'MPa')
print('maximum shear stress due to diagonal compression in the flange: vRdMax=', vRdMax/1e6, 'MPa')
print('maximum shear due to diagonal compression in the flange: VRdMax=', VRdMax/1e3, 'kN')
print('maximum shear resisted by the concrete: VRdc=', VRdc/1e3, 'kN')
print('ratio2= ', ratio2)
print('maximum shear stress due to the strength of the transverse reinforcement: vRds=', vRds/1e6, 'MPa')
print('maximum shear due to the strength of the transverse reinforcement: VRds=', VRds/1e3, 'kN')
print('ratio3= ', ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-12) and (ratio3<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# crossSectionGeometry.draw()
