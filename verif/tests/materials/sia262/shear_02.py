# -*- coding: utf-8 -*-
# Home made test: check shear for very small sollicitations.

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.sections.fiber_section import def_simple_RC_section
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from postprocess import element_section_map
from postprocess import limit_state_data as lsd

concrete= SIA262_materials.c30_37
reinfSteel= SIA262_materials.B500B

areaFi14= SIA262_materials.section_barres_courantes[14e-3]
fi14s150r40= def_simple_RC_section.ReinfRow(rebarsDiam=14e-3,areaRebar= areaFi14,rebarsSpacing= 0.150,width=1.0,nominalCover= 0.040)
fi14s150r50=def_simple_RC_section.ReinfRow(rebarsDiam=14e-3,areaRebar= areaFi14,rebarsSpacing= 0.150,width=1.0,nominalCover= 0.050)

slab30RCSect= element_section_map.RCSlabBeamSection(name='slab30RCSect',sectionDescr="foundation slab thickness 30 cm.",concrType=concrete, reinfSteelType=reinfSteel,depth=0.3)
slab30RCSect.dir1PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi14s150r40]) #Longitudinal
slab30RCSect.dir1NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi14s150r40]) #
slab30RCSect.dir2PositvRebarRows= def_simple_RC_section.LongReinfLayers([fi14s150r50]) #Transverse
slab30RCSect.dir2NegatvRebarRows= def_simple_RC_section.LongReinfLayers([fi14s150r50]) #

slab30RCSect.createSections()

shearController= SIA262_limit_state_checking.ShearController(lsd.shearResistance.label)

#rcSection= slab30RCSect.lstRCSects[0]
rcSection= slab30RCSect.lstRCSects[1]
shearController.setSection(rcSection)

VuTmp= rcSection.getRoughVcuEstimation()
VyTmp= -1768.49921973
FCtmp= abs(VyTmp)/VuTmp
ratio1= abs(FCtmp-0.0117899947982)/0.0117899947982

'''
print('VuTmp= ', VuTmp/1e3, 'kN')
print('VyTmp= ', VyTmp/1e3, 'kN')
print('FCtmp= ', FCtmp)
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-4):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
