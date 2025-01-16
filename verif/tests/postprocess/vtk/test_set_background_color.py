# -*- coding: utf-8 -*-
''' Check display that the setBackgroundColor method works. The test does
 not verify the output contents, only that the method runs and the file is
 created.'''


from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import geom
import xc

from solution import predefined_solutions
from model import predefined_spaces
from materials.ec5 import EC5_materials

wood= EC5_materials.C24
sectionWidth= 0.075
sectionHeight= 0.2
section= EC5_materials.RectangularShape(name= 'scc', wood= wood, b= sectionWidth, h= sectionHeight)


feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
scc= section.defElasticShearSection3d(preprocessor)

# Problem geometry
L= 4.5
spacing= 0.5
points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0,0,0))
pt2= points.newPoint(geom.Pos3d(L,0,0))

lines= preprocessor.getMultiBlockTopology.getLines
ln= lines.newLine(pt1.tag,pt2.tag)
ln.nDiv= 10

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 3 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam3d= seedElemHandler.newElement("ElasticTimoshenkoBeam3d")
#beam3d.h= sectionHeight

xcTotalSet= preprocessor.getSets.getSet("total")
xcTotalSet.genMesh(xc.meshDir.I)

L= 4.5 
nA= pt1.getNode()
nC= ln.getNearestNode(geom.Pos3d(L/2,0.0,0.0))
nB= pt2.getNode()

# Constraints
constraints= preprocessor.getBoundaryCondHandler
modelSpace.fixNode('000_0FF',nA.tag) # Node A
modelSpace.fixNode('000_FFF',nB.tag) # Node B


# Load definition.
lp0= modelSpace.newLoadPattern(name= 'ULS')
ulsLoad= spacing*(1.35*0.5e3+1.5*1.5e3)
eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID(xcTotalSet.getElementTags())
eleLoad.transComponent= -ulsLoad
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

#########################################################
# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)
# oh.setBackgroundColor((0,0,0))
oh.setLineWidth(5.0)
# oh.displayDispRot(itemToDisp= 'uZ')

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayDispRot(itemToDisp= 'uZ', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)

