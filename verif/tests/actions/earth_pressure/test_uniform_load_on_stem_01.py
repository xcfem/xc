# -*- coding: utf-8 -*-
''' Home made test.  Uniform load on earth retaining stem.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
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
n1= modelSpace.newNode(0,0)
n2= modelSpace.newNode(0,H)

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
beam2d= modelSpace.newElement("ElasticBeam2d", [n1.tag,n2.tag])

modelSpace.fixNode000(n1.tag)

q= 10e3 # 10 kN/m
pressure= earth_pressure.UniformLoadOnStem(qLoad= q)

xcTotalSet= modelSpace.getTotalSet()

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
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve.")
    
delta= n2.getDisp[0] # x displacement of node 2.
deltaTeor= -q*H**4/8/E/Iz
ratio= abs(delta-deltaTeor)/abs(deltaTeor)

'''
print('delta: ', delta*1e3, 'mm')
print('deltaTeor: ', deltaTeor*1e3, 'mm')
print('ratio= ', ratio)
'''

import os
fname= os.path.basename(__file__)
if abs(ratio)<1e-6:
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
