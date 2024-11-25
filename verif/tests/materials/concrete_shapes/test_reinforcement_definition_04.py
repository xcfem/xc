# -*- coding: utf-8 -*-
''' Test positions of reinforcement layer in circular section.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"

import math
import xc
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section

# Materials definition
concr= EHE_materials.HA30
concr.alfacc=1.0
steel= EHE_materials.B500S

radius= 0.65/2.0 # Cross-section radius expressed in meters.
nCover= 0.025+12e-3

rebarDiam= 20e-3 # Bar diameter expressed in meters.
cover= nCover+rebarDiam/2.0 # Concrete cover expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.
section1= def_column_RC_section.RCCircularSection(name='Section1',Rext= radius, concrType=concr, reinfSteelType= steel)

# Longitudinal reinforcement
nRebars= 12
section1.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars= nRebars, width= 2*math.pi*(radius-cover), nominalCover= nCover)])

# Define dummy FE problem.
dummyFEProblem= xc.FEProblem()
preprocessor=  dummyFEProblem.getPreprocessor
# Compute the positionso of the bars (among other things).
section1.defRCSection(preprocessor= preprocessor, matDiagType= 'd')

# Access the geometry of the section.
geomSection= section1.getSectionGeometry()
## Access the reinforcement.
reinforcementLayer= list(geomSection.getReinfLayers)[0]
rebars= list(reinforcementLayer.getReinfBars)
ratio1= abs(nRebars-len(rebars))
distances= list()
p0= rebars[0].getPos2d
for b in rebars[1:]:
    p1= b.getPos2d
    distances.append(p1.dist(p0))
    p0= p1
# And the last one.
distances.append(p1.dist(rebars[0].getPos2d))
ratio2= len(distances)-nRebars
avg= sum(distances)/len(distances)
refAvg= 0.14390338907700156
ratio3= abs(avg-refAvg)/refAvg
refDistances= [0.1439033890770015, 0.1439033890770016, 0.14390338907700156, 0.1439033890770015, 0.1439033890770015, 0.14390338907700162, 0.14390338907700148, 0.1439033890770015, 0.14390338907700162, 0.1439033890770014, 0.14390338907700165, 0.14390338907700168]

err= 0.0
for d, dRef in zip(distances, refDistances):
    err+= (d-dRef)**2
err= math.sqrt(err)

'''
print('num. rebars= ', len(rebars), 'ratio1= ', ratio1)
print(len(distances), 'ratio2= ', ratio2)
print('average: ', avg, 'ratio3= ', ratio3)
print('err= ', err)
'''

testOK= ((ratio1==0) and (ratio2==0) and (abs(ratio3)<1e-6) and (err<1e-6))

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
