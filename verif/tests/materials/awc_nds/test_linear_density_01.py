# -*- coding: utf-8 -*-
''' Test linearRho method.'''

# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import xc

from misc_utils import log_messages as lmsg
from materials.awc_nds import structural_panels

# Finite element problem.
FEcase= xc.FEProblem()
FEcase.title= 'Second floor joists and LVL beams.'
preprocessor= FEcase.getPreprocessor

# Materials
beamPanel= structural_panels.LVLHeaderSections['1.75x18']
beamSection= beamPanel.defElasticShearSection3d(preprocessor)
ratio1= abs(beamSection.linearRho-beamPanel.getLinearDensity())/beamPanel.getLinearDensity()
beamRefValue= 9*0.4535924/.3048 # 9 lb/ft
ratio2= abs(beamSection.linearRho-beamRefValue)/beamRefValue
joistPanel= structural_panels.LVLHeaderSections['1.75x7-1/4']
joistSection= joistPanel.defElasticShearSection3d(preprocessor)
ratio3= abs(joistSection.linearRho-joistPanel.getLinearDensity())/joistPanel.getLinearDensity()
joistRefValue= 3.6*0.4535924/.3048 # 3.6 lb/ft
ratio4= abs(joistSection.linearRho-joistRefValue)/joistRefValue

'''
print(beamPanel.getLinearDensity())
print(beamSection.linearRho)
print('ratio1= ', ratio1)
print(beamRefValue)
print('ratio2=', ratio2)
print(joistPanel.getLinearDensity())
print(joistSection.linearRho)
print('ratio3=', ratio3)
print(joistRefValue)
print('ratio4=', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-12) and (ratio2<1e-6) and (ratio3<1e-12) and(ratio3<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
