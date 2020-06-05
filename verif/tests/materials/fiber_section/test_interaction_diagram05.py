# -*- coding: utf-8 -*-
''' Checks the computation of the interaction diagram.
   Home made test.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import xc_base
import geom
import xc

from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections.fiber_section import def_column_RC_section
import math

radius= 0.75/2.0 # Cross-section radius expressed in meters.
rebarDiam= 20e-3 # Bar diameter expressed in meters.
nCover= 0.06+12e-3
cover= nCover+rebarDiam/2.0 # Concrete cover expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
concr= EHE_materials.HA30
concr.alfacc=0.85 #f_maxd= 0.85*fcd concrete long term compressive strength factor (normally alfacc=1)
steel= EHE_materials.B500S
section= def_column_RC_section.RCCircularSection(name='test',Rext= radius, concrType=concr, reinfSteelType= steel)

section.mainReinf= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= rebarDiam, nRebars=14, width= 2*math.pi*(radius-cover), nominalCover= nCover)])

section.defRCSection(preprocessor,matDiagType= 'd')

diagIntsecHA= section.defInteractionDiagram(preprocessor)

# Compute capacity factors.
fc1= diagIntsecHA.getCapacityFactor(geom.Pos3d(1850e3,0,0))
fc2= diagIntsecHA.getCapacityFactor(geom.Pos3d(-152e3,530e3,0))
fc3= diagIntsecHA.getCapacityFactor(geom.Pos3d(-152e3,0,590e3))
fc4= diagIntsecHA.getCapacityFactor(geom.Pos3d(-9097e3,0.0,0.0))

ratio1= abs(fc1-1)
ratio2= abs(fc2-1)
ratio3= abs(fc3-1.0)
ratio4= abs(fc4-1.0)

''' 
print("fc1= ",fc1)
print("ratio1= ",ratio1)
print("fc2= ",fc2)
print("ratio2= ",ratio2)
print("fc3= ",fc3)
print("ratio3= ",ratio3)
print("fc4= ",fc4)
print("ratio4= ",ratio4)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-2) & (abs(ratio2)<1e-2) & (abs(ratio3)<1e-2) & (abs(ratio4)<1e-2)):
  print("test "+fname+": ok.")
else:
  lmsg.error(fname+' ERROR.')
