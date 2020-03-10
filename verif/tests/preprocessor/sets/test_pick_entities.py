# -*- coding: utf-8 -*-
'''Selection of entities inside a geometric object. Home made test.'''

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod0= nodes.newNodeXYZ(0.0,0.0,0.0)
nod1= nodes.newNodeXYZ(0.5,0.5,0.5)
nod2= nodes.newNodeXYZ(2.0,2.0,2.0)
nod3= nodes.newNodeXYZ(3.0,3.0,3.0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,1]))

# Materials
section= typical_materials.defElasticSection3d(preprocessor, "section",1,1,1,1,1,1)

elements= preprocessor.getElementHandler
elements.defaultTransformation= "lin" # Coord. transformation.
elements.defaultMaterial= "section"
ele0= elements.newElement("ElasticBeam3d",xc.ID([nod0.tag,nod1.tag]))
ele1= elements.newElement("ElasticBeam3d",xc.ID([nod2.tag,nod3.tag]))

points= preprocessor.getMultiBlockTopology.getPoints
pt0= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt1= points.newPntFromPos3d(geom.Pos3d(0.5,0.5,0.5))
pt2= points.newPntFromPos3d(geom.Pos3d(3.0,3.0,3.0))
pt3= points.newPntFromPos3d(geom.Pos3d(4.0,4.0,4.0))

l1=  preprocessor.getMultiBlockTopology.getLines.newLine(pt0.tag,pt1.tag)
l2=  preprocessor.getMultiBlockTopology.getLines.newLine(pt2.tag,pt3.tag)

geomObj= geom.BND3d(geom.Pos3d(-1.0,-1.0,-1.0),geom.Pos3d(1.0,1.0,1.0))

xcTotalSet= preprocessor.getSets.getSet('total')
setIn= preprocessor.getSets.defSet('setIn')
setIn.nodes= xcTotalSet.nodes.pickNodesInside(geomObj,0.0)
setIn.elements= xcTotalSet.elements.pickElemsInside(geomObj,0.0)
setIn.points= xcTotalSet.points.pickPointsInside(geomObj,0.0)
setIn.lines= xcTotalSet.lines.pickLinesInside(geomObj,0.0)

nNodes= len(setIn.nodes)
nElements= len(setIn.elements)
nPoints= len(setIn.points)
nLines= len(setIn.lines)

ratio= (nNodes-2)**2+(nElements-1)**2+(nPoints-2)**2+(nLines-1)**2

'''
print nNodes, ' nodes inside.'
print nElements, ' element(s) inside.'
print nPoints, ' points inside.'
print nLines, ' lines(s) inside.'
'''

import os
fname= os.path.basename(__file__)
if (abs(ratio)<1e-15):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

