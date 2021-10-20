# -*- coding: utf-8 -*-
''' Checks the computation of the interaction diagram according
    to Eurocode 2.
   Based on the article: 
   "Interaction diagrams for reinforced
    concrete circular cross-section"
   of Željko Smolčić and Davor Grandić '''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from materials.ec2 import EC2_materials

radius= 0.25 # Cross-section radius expressed in meters.
rebarDiam= 16e-3 # Main reinforcement diameter expressed in meters.
cover= 0.05 #+shearReinfDiam+rebarDiam/2.0 # Concrete cover expressed in meters.
rebarArea= math.pi*(rebarDiam/2.0)**2 # Rebar area expressed in square meters.
numRebars= math.ceil(41.55e-4/rebarArea) # Number of rebars.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
## Concrete material.
concr= EC2_materials.C30
concr.alfacc=0.85 # long term compressive strength factor
concrDiagram= concr.defDiagD(preprocessor) # stress-strain diagram for concrete.
## Reinforcing steel.
steel= EC2_materials.S500B
steelDiagram= steel.defDiagD(preprocessor) # stress-strain diagram for steel.

# Cross-section geometry.
## Concrete circular region
columnGeometry= preprocessor.getMaterialHandler.newSectionGeometry("columnGeometry")
regions= columnGeometry.getRegions
concrete= regions.newCircularRegion(concrDiagram.name) # concrete region.
concrete.nDivCirc= 20 # number of divisions along the perimeter.
concrete.nDivRad= 5 # number of divisions along the radius.
concrete.extRad= radius # external radius.
concrete.intRad= 0.0 # internal radius.
concrete.initAngle= 0.0 # initial angle.
concrete.finalAngle= 2*math.pi # final angle (2*PI for whole circle).

## Definition of reinforcement
reinforcement= columnGeometry.getReinfLayers.newCircReinfLayer(steelDiagram.name)
reinforcement.numReinfBars= int(numRebars) # number of rebars.
reinforcement.barArea= rebarArea # area of each rebar cross-section.
reinforcement.initAngle= 0.0 # initial angle.
reinforcement.finalAngle= 2*math.pi # final angle (2*PI for whole circle).
reinforcement.radius= concrete.extRad-cover # radius.

# Fiber section definition
## Create fiber-section object.
materialHandler= preprocessor.getMaterialHandler
secHA= materialHandler.newMaterial("fiber_section_3d","secHA")
fiberSectionRepr= secHA.getFiberSectionRepr()
## Assign geometry.
fiberSectionRepr.setGeomNamed(columnGeometry.name)
## Create fibers from the geometry data.
secHA.setupFibers()
#fibers= secHA.getFibers()

# Create interaction diagram.
param= xc.InteractionDiagramParameters()
param.concreteTag= concrDiagram.tag # Set concrete type.
param.reinforcementTag= steelDiagram.tag # Set steel type.
interactionDiagram= materialHandler.calcInteractionDiagram(secHA.name,param)

# Compute capacity factor.
N= -1570e3 # Axial force at column top
My= 392e3 # Y bending moment at column top
Mz= 0.0 # Z bending moment at column top
CF= interactionDiagram.getCapacityFactor(geom.Pos3d(N,My,Mz))
CFRef= 1.07887046522 # Almost 1.0, no big deal.
ratio= abs(CF-CFRef)/CFRef

'''
print("CF= ",CF)
print("ratio= ",ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
