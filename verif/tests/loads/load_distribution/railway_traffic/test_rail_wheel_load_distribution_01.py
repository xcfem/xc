# -*- coding: utf-8 -*-
''' Trivial test to check that the distribution of wheel loads as
    defined in the roadway traffic modules works fine.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2022, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from actions.railway_traffic import wheel_load as wl


# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Define materials
## Materials.
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 1.33 # specific mass [kg/m2].
dummySection= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


# Problem geometry.
span= 5

## K-points.
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(span,0,0))
pt3= points.newPoint(geom.Pos3d(span,span,0))
pt4= points.newPoint(geom.Pos3d(0,span,0))
corners= [pt1, pt2, pt3, pt4]
## Surface.
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)
s.nDivI= 20
s.nDivJ= 20

# Generate mesh.
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= dummySection.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))
s.genMesh(xc.meshDir.I)

# Constraints.
constrainedNodes= list()
for pt in corners:
    n= pt.getNode()
    modelSpace.fixNode000_FFF(n.tag)
    constrainedNodes.append(n)

# Wheel load.
originSet= modelSpace.getTotalSet()
spreadingLayers= [(0.5, 1), (0.5, 1)]
localCoordinateSystem= geom.CooSysRect3d3d(geom.Vector3d(1,0,0), geom.Vector3d(0,1,0))
wheelLoad= wl.WheelLoad(pos= geom.Pos3d(span/2.0, span/2.0, 0.0), ld= 150e3, localCooSystem= localCoordinateSystem)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
wheelLoad.defDeckConcentratedLoadsThroughLayers(spreadingLayers= spreadingLayers, originSet= originSet, deckThickness= 0.3)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)
## Compute solution
result= modelSpace.analyze(1, calculateNodalReactions= True)
if(result!=0):
    print('Can\'t solve.')
    exit(1)

R= xc.Vector(6*[0.0])
for n in constrainedNodes:
    R+= n.getReaction

ratio1= abs(wheelLoad.load-R[2])/wheelLoad.load
ratio2= math.sqrt(R[0]**2+R[1]**2+R[3]**2+R[4]**2+R[5]**2)

'''
print('R=', R)
print('ratio1= ', ratio1)
print('ratio2= ', ratio2)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-6) & (abs(ratio2)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# #########################################################
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# ## Uncomment to display the mesh
# oh.displayFEMesh()

# ## Uncomment to display the element axes
# # oh.displayLocalAxes()

# ## Uncomment to display the loads
# oh.displayLoads()

# ## Uncomment to display the reactions
# oh.displayReactions()

