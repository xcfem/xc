# -*- coding: utf-8 -*-
''' Trivial test concering reinforced concrete rectangular section.'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O), Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, AO_O, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es, l.pereztato@ciccp.es"

import math
from materials.sections.fiber_section import def_simple_RC_section
from materials.ec2 import EC2_materials
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Geometry of the reinforcement.
spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= nBarsA*spacing+2.0*lateralCover

## First row
barDiameter= 25e-3 # Diameter of the reinforcement bar.
barAreaA= math.pi*(barDiameter/2.0)**2 # Area of the reinforcement bar.
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, areaRebar= barAreaA, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)
areaA= rowA.getAs()

## Second row
barAreaB= math.pi*(barDiameter/2.0)**2 # Area of the reinforcement bar.
### Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, areaRebar= barAreaB, rebarsSpacing= spacing, width= width-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)
areaB= rowB.getAs()
area= areaA+areaB

# Define reinforcement layers.
reinfLayers= def_simple_RC_section.LongReinfLayers([rowA, rowB])

# Define reinforced concrete rectangular section.
## Materials.
concrete= EC2_materials.C20
steel= EC2_materials.S500C
## Geometry
b= width
h= 0.20
## RC section.
rcSection= def_simple_RC_section.RCRectangularSection(name='RC section', width= b, depth= h, concrType= concrete, reinfSteelType= steel)
rcSection.positvRebarRows= reinfLayers # Reinforcement on the positive side.

# Define finite element problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
## Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

## Materials.
## Fiber-section material.
rcSection.defRCSection2d(preprocessor,matDiagType= 'k')


## Create mesh.
### Create nodes.
span= 5
numDiv= 10
beamNodes= list()
for i in range(0,numDiv+1):
    x= i/numDiv*span
    beamNodes.append(nodes.newNodeXY(x,0.0))
