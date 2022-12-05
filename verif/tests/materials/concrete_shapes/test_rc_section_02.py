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

# Reinforcement row scheme:
#
#    |  o    o    o    o    o    o    o    o    o    o  |
#    <->           <-->                               <-> 
#    lateral      spacing                           lateral
#     cover                                          cover
#

# Define reinforcement geometry.
spacing= 0.15 # spacing of reinforcement.
nBarsA= 10 # number of bars.
cover= 0.035 # concrete cover.
lateralCover= cover # concrete cover for the bars at the extremities of the row.
width= nBarsA*spacing+2.0*lateralCover

## First row
barDiameter= 25e-3 # Diameter of the reinforcement bar.
### Reinforcement row.
rowA= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width, nominalCover= cover, nominalLatCover= lateralCover)
areaA= rowA.getAs()

## Second row
### Reinforcement row.
rowB= def_simple_RC_section.ReinfRow(rebarsDiam= barDiameter, rebarsSpacing= spacing, width= width-spacing, nominalCover= cover, nominalLatCover= lateralCover+spacing/2.0)
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
rcSection.negatvRebarRows= reinfLayers # Reinforcement on the negative side.

# Compute homogenized areas.
hCoef= rcSection.getHomogenizationCoefficient() # homogenization coefficient.
hArea= rcSection.getAreaHomogenizedSection() # homogenized area.
concreteArea= b*h
steelArea= reinfLayers.getAs()
hRefArea= concreteArea+hCoef*steelArea # reference value for homogenized area.
ratio1= abs(hArea-hRefArea)/hRefArea

# Compute reinforcement position.
hAsNeg= rcSection.hAsNeg()
hAsNegRef= h-(.035+barDiameter/2.0)
ratio2= abs(hAsNeg-hAsNegRef)/hAsNegRef

# Compute section centroid.
cog= rcSection.hCOGHomogenizedSection()
cogRef= (h/2.0*concreteArea+(h-cover-barDiameter/2.0)*hCoef*steelArea)/(concreteArea+hCoef*steelArea)
ratio3= abs(cog-cogRef)/cogRef


# Compute inertia.
iz= rcSection.getIzHomogenizedSection()
d= h/2.0-cogRef
nBars= sum(reinfLayers.getNBar())
## Reinforced concrete.
izRef= 1/12*b*h**3+concreteArea*d**2
## Steel.
izRef+= hCoef*nBars*math.pi*(barDiameter/2.0)**4/4.0
d= hAsNegRef-cogRef
steelInertiaRef= (hCoef-1)*steelArea*d**2
izRef+= steelInertiaRef
ratio4= abs(iz-izRef)/izRef

'''
print('homogenization coefficient: hCoef= ', hCoef)
print('ratio1= ', ratio1)
print('hAsNeg= ', hAsNeg, ' m')
print('ratio2= ', ratio2)
print('center of gravity: cog= ', cog, 'm')
print('center of gravity: cogRef= ', cogRef, 'm')
print('ratio3= ', ratio3)
print('Iz= ', iz, 'm4')
print('IzRef= ', izRef, 'm4')
print('ratio4= ', ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0 and (abs(ratio2)<1e-12) and (abs(ratio3)<1e-12) and (abs(ratio4)<1e-12)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

