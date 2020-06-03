# -*- coding: utf-8 -*-

''' Verification test based on the EXAMPLE ACI 318-14 RC-BM-001 of
the program SAFE form Computers and Structures Inc.'''

 
from __future__ import division
from __future__ import print_function
from materials.aci import ACI_materials
from materials.aci import ACI_limit_state_checking as lsc
import os
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data as lsd

__author__= "Luis Claudio Pérez Tato (LCPT"
__copyright__= "Copyright 2015, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

concrete= ACI_materials.c4000 # Concrete strength 4000 psi
concrete.gmmC= 1.0/0.75
reinfSteel= ACI_materials.A615G60
reinfSteel.gammaS= 1.0/0.75

lSqrtFck= concrete.getLambdaSqrtFck()
lSqrtFckRef= 63.246*ACI_materials.toPascal
ratio1= abs(lSqrtFck-lSqrtFckRef)/lSqrtFckRef

bw= 12*0.0254 # 12 in
d= 15*0.0254 # 15 in
section= def_simple_RC_section.RCRectangularSection(name='test',concrType=concrete, reinfSteelType=reinfSteel,width= bw, depth= d/0.9)
section.shReinfY= def_simple_RC_section.ShearReinforcement(nShReinfBranches= 1.0,areaShReinfBranch= 0.592*0.0254**2,shReinfSpacing= 1.0*0.3048)
shearController= lsc.ShearController(lsd.shearResistance.label)
shearController.setSection(section)

Vc= shearController.getVcNoShearRebars(Nd= 0.0)
VcRef= 17.076*4.4482216e3/0.75 # kip->N
ratio2= abs(Vc-VcRef)/VcRef

Vmax= shearController.getV_max(Nd= 0.0)
VmaxRef= 85.381*4.4482216e3
ratio3= abs(Vmax-VmaxRef)/VmaxRef

ratio4= abs(0.0004774184-section.shReinfY.getAs()*shearController.effectiveDepth)/0.0004774184

Vu= shearController.getVu(Nd= 0.0)
VuRef= 50.4*4.4482216e3
ratio5= abs(Vu-VuRef)/VuRef

'''
print('lSqrtFck= ', lSqrtFck/1e6, ' MPa')
print('lSqrtFckRef= ', lSqrtFckRef/1e6, ' MPa')
print('ratio1= ', ratio1)
print('Vc= ', Vc/1e3, ' kN')
print('VcRef= ', VcRef/1e3, ' kN')
print('ratio2= ', ratio2)
print('Vmax= ', Vmax/1e3, ' kN')
print('VmaxRef= ', VmaxRef/1e3, ' kN')
print('ratio3= ', ratio3)
print('ratio4= ', ratio4)
print('Vu= ', Vu/1e3, ' kN')
print('VuRef= ', VuRef/1e3, ' kN')
print('ratio5= ', ratio5)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-5) and (ratio2<1e-4) and (ratio3<1e-5) and (ratio4<1e-4) and (ratio5<5e-3)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

