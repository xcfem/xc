# -*- coding: utf-8 -*-
''' Reinforced concrete section. Crack control verification test.
   results are compared with those of the «Prontuario informático del hormigón armado». '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc

from materials.sections.fiber_section import def_simple_RC_section

from materials.ehe import EHE_materials

nominalCover= .025
rcSection= def_simple_RC_section.RCRectangularSection(width= 0.25, depth= 0.5)
rcSection.sectionName= "Test"
rcSection.sectionDescr= "Test reinforce concrete section."
rcSection.fiberSectionParameters.concrType= EHE_materials.HA25
rcSection.fiberSectionParameters.reinfSteelType= EHE_materials.B500S
rebNeg= def_simple_RC_section.ReinfRow(nRebars= 2, areaRebar= EHE_materials.Fi20,nominalCover= nominalCover+0.01,width= rcSection.b-nominalCover)
rcSection.negatvRebarRows= def_simple_RC_section.LongReinfLayers([rebNeg])
rebPos= def_simple_RC_section.ReinfRow(nRebars= 5, areaRebar= EHE_materials.Fi20,nominalCover=nominalCover,width= rcSection.b-nominalCover)
rcSection.positvRebarRows= def_simple_RC_section.LongReinfLayers([rebPos])

# Mass properties of gross section

err= 0.0
grossSectionArea= rcSection.A()
err+= (grossSectionArea-0.125)**2
grossSectionIy= rcSection.Iy()
err+= (grossSectionIy-0.000651041666667)**2
grossSectionIz= rcSection.Iz()
err+= (grossSectionIz-0.00260416666667)**2
grossSectioniy= rcSection.iy()
err+= (grossSectioniy-0.0721687836487)**2
grossSectioniz= rcSection.iz()
err+= (grossSectioniz-0.144337567297)**2
grossSectionCOGy= rcSection.hCOG()
err+= (grossSectionCOGy-0.25)**2
grossSectionCOGz= rcSection.bCOG()
err+= (grossSectionCOGz-0.125)**2


# Homogenized section
n= rcSection.getHomogenizationCoefficient()
homogenizedSectionArea= rcSection.getAreaHomogenizedSection()
err+= (homogenizedSectionArea-0.141123801568)**2
# homogenizedSectionIy= rcSection.Iy()
# err+= (homogenizedSectionIy-0.000651041666667)**2
homogenizedSectionIz= rcSection.getIzHomogenizedSection()
err+= (homogenizedSectionIz-0.00352774842486)**2
# homogenizedSectioniy= rcSection.iy()
# err+= (homogenizedSectioniy-0.0721687836487)**2
homogenizedSectioniz= rcSection.izHomogenizedSection()
err+= (homogenizedSectioniz-0.158106115944)**2
homogenizedSectionCOGy= rcSection.hCOGHomogenizedSection()
err+= (homogenizedSectionCOGy-0.232290926852)**2
# homogenizedSectionCOGz= rcSection.bCOG()

err= math.sqrt(err)

'''
print('gross section area: ', grossSectionArea, ' m2')
print('gross section Iy= ', grossSectionIy, ' m4')
print('gross section Iz= ', grossSectionIz, ' m4')
print('gross section iy= ', grossSectioniy, ' m')
print('gross section iz= ', grossSectioniz, ' m')
print('gross section center of gravity y= ', grossSectionCOGy, ' m')
print('gross section center of gravity z= ', grossSectionCOGz, ' m')
print('homogenization coefficient n= ', n)
print('homogenized section area: ', homogenizedSectionArea, ' m2')
# print('homogenized section Iy= ', homogenizedSectionIy, ' m4')
print('homogenized section Iz= ', homogenizedSectionIz, ' m4')
# print('homogenized section iy= ', homogenizedSectioniy, ' m')
print('homogenized section iz= ', homogenizedSectioniz, ' m')
print('homogenized section center of gravity y= ', homogenizedSectionCOGy, ' m')
# print('homogenized section center of gravity z= ', homogenizedSectionCOGz, ' m')
print('error= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
