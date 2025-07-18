# -*- coding: utf-8 -*-
'''Trivial test of earth pressure due to a uniform load in the backfill
surface.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from actions.earth_pressure import earth_pressure
from misc_utils import log_messages as lmsg

# Material properties.
E= 31476e6 # Concrete elastic modulus (Pa)
nu= 0.2 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus

# Cross section properties
sectionDepth= 0.5 # section depth (m)
A= sectionDepth # Cross section area (m2)
Iz= 1/12 * sectionDepth**3 # Cross section moment of inertia (m4)

# Geometry
H= 5 # Stem height (m)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
## K-points.
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(0,H,0)
## Lines.
ln= modelSpace.newLine(p1, p2)
ln.setElemSize(0.5)

## Coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin")

## Define material.
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.I= Iz
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section", sectionProperties)

# Define element.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(section)
modelSpace.newSeedElement('ElasticBeam2d')
ln.genMesh(xc.meshDir.I)

modelSpace.fixNode000(p1.getNode().tag)

# Define earth pressure.
zGround= p2.getPos.y-0.5
qUnif= 5e3 # 5 kN/m
zBottomSoils=[-1e3]
kSoils= [0.486]

# zGround: global Z coordinate of ground level
# zBottomSoils: list of global Z coordinates of the bottom level
#               for each soil (from top to bottom)
# KSoils: list of pressure coefficients for each soil (from top 
#         to bottom)
# qUnif: uniform load over the backfill surface.
pressure= earth_pressure.UniformPressureOnBackfill(zGround= zGround, zBottomSoils= zBottomSoils, KSoils= kSoils, qUnif= qUnif)


xcTotalSet= modelSpace.getTotalSet()
groundNode= xcTotalSet.getNearestNode(geom.Pos3d(0, zGround, 0))

# Create load.
vDir= xc.Vector([-1,0])
lp0= modelSpace.newLoadPattern(name= '0', setCurrent= True)
# xcSet: set that contains the elements that will receive the load.
# vDir: unit xc vector defining pressures direction
# iCoo: index of the coordinate that represents depth.
# delta: soil-wall friction angle (usually: delta= 2/3*Phi).
pressure.appendLoadToCurrentLoadPattern(xcSet= xcTotalSet, vDir= vDir, iCoo= 1, delta= 0.0)
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)
if(result!=0):
    lmsg.error("Can't solve.")


Rx= p1.getNode().getReaction[0] # x reaction on node 1.
virtualBack= geom.Segment2d(geom.Pos2d(0,0), geom.Pos2d(0, zGround))
RxRef= -pressure.getForces2D(virtualBack, numDiv= 10, beta= 0.0).getResultant()[0]
ratio1= abs(Rx-RxRef)/abs(RxRef)

delta= groundNode.getDisp[0]
deltaTeor= -kSoils[0]*qUnif*zGround**4/8/E/Iz
ratio2= abs(delta-deltaTeor)/abs(deltaTeor)

'''
print(Rx, RxRef, ratio1)
print('delta: ', delta*1e3, 'mm')
print('deltaTeor: ', deltaTeor*1e3, 'mm')
print('ratio2= ', ratio2)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio1)<1e-10 and abs(ratio2)<1e-6:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
 
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
# # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
# # oh.displayLocalAxes()
# oh.displayLoads()
# # oh.displayIntForcDiag('N')
# oh.displayIntForcDiag('M')
# # oh.displayLocalAxes()
