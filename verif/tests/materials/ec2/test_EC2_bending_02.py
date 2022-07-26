# -*- coding: utf-8 -*-
''' Checks the computation of the bending strength according to EC2.
    Result have been compared with the results obtained from
    
    Home made test.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import geom
import xc

from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
import math

radius= 0.5 # Cross-section radius expressed in meters.
rebarDiam= 32e-3 # Bar diameter expressed in meters.
nCover= 0.025+12e-3
cover= nCover+rebarDiam/2.0 # Concrete cover expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concr= EC2_materials.C30
concr.alfacc=1.0
steel= EC2_materials.S500B
section= def_column_RC_section.RCCircularSection(name='test',Rext= radius, concrType=concr, reinfSteelType= steel)

section.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars=22, width= 2*math.pi*(radius-cover), nominalCover= nCover)])

section.defRCSection(preprocessor,matDiagType= 'd')

## Check that reinforcement area is correctly defined.
fiberSection= section.fs
As= 0.0
numBars= 0
for fiber in fiberSection.getFibers():
    material= fiber.getMaterial()
    tipo= material.type()
    if(tipo=='XC::Steel01'):
        As+= fiber.getArea()
        numBars+= 1
ratio0= abs(As-section.mainReinf.getAs())/As
ratio1= abs(numBars-sum(section.mainReinf.getNBar()))

diagIntsecHA= section.defInteractionDiagram(preprocessor)

# Compute capacity factor.
fc= diagIntsecHA.getCapacityFactor(geom.Pos3d(-10e3, 2800e3,0))

ratio2= abs(fc-1.0306462700710126)

'''
print('reinf. layers As= ', section.mainReinf.getAs())
print('As= ', As)
print('ratio0= ', ratio0)
print('numBars= ', numBars)
print('ratio1= ', ratio1)
print("fc= ",fc)
print("ratio2= ",ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio0)<1e-6) & (abs(ratio1)<1e-12) & (abs(ratio2)<1e-5)):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
