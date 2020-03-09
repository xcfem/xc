# -*- coding: utf-8 -*-
'''8 node brick element Patch test.

Reference: MacNeal & Harder, “A Proposed Standard Set of Problems to Test Finite Element Accuracy”, Finite Elements in Analysis and Design, 1 (1985) 3-20
'''

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
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from postprocess import output_handler

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Materials definition
elast= typical_materials.defElasticIsotropic3d(preprocessor, "elast3d",1e6,0.25,0.0)

nodes= preprocessor.getNodeHandler 
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod0= nodes.newNodeXYZ(0,0,0)
nod1= nodes.newNodeXYZ(1,0,0)
nod2= nodes.newNodeXYZ(1,1,0)
nod3= nodes.newNodeXYZ(0,1,0)
nod4= nodes.newNodeXYZ(0,0,1)
nod5= nodes.newNodeXYZ(1,0,1)
nod6= nodes.newNodeXYZ(1,1,1)
nod7= nodes.newNodeXYZ(0,1,1)
nod8= nodes.newNodeXYZ(0,0,2)
nod9= nodes.newNodeXYZ(1,0,2)
nod10= nodes.newNodeXYZ(1,1,2)
nod11= nodes.newNodeXYZ(0,1,2)
nod12= nodes.newNodeXYZ(0,0,3)
nod13= nodes.newNodeXYZ(1,0,3)
nod14= nodes.newNodeXYZ(1,1,3)
nod15= nodes.newNodeXYZ(0,1,3)

elements= preprocessor.getElementHandler
elements.defaultMaterial= "elast3d"


brick0= elements.newElement("Brick",xc.ID([nod0.tag, nod1.tag, nod2.tag, nod3.tag, nod4.tag, nod5.tag, nod6.tag, nod7.tag]))
brick1= elements.newElement("Brick",xc.ID([nod4.tag, nod5.tag, nod6.tag, nod7.tag, nod8.tag, nod9.tag, nod10.tag, nod11.tag]))
brick2= elements.newElement("Brick",xc.ID([nod8.tag, nod9.tag, nod10.tag, nod11.tag, nod12.tag, nod13.tag, nod14.tag, nod15.tag]))

# totalSet= preprocessor.getSets.getSet('total')
# for e in totalSet.getElements:
#     print(e.tag, 'K= ', e.getTangentStiff().Norm())

constraints= preprocessor.getBoundaryCondHandler
nod0.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod1.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod2.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )
nod3.fix(xc.ID([0,1,2]),xc.Vector([0,0,0]) )

# Load definition
P= 1e3 # punctual load.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
## Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
## Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lPatterns.currentLoadPattern= "0"
nod4.newLoad(xc.Vector([0,0,P]))
nod5.newLoad(xc.Vector([0,0,P]))
nod6.newLoad(xc.Vector([0,0,P]))
nod7.newLoad(xc.Vector([0,0,P]))
nod8.newLoad(xc.Vector([0,0,P]))
nod9.newLoad(xc.Vector([0,0,P]))
nod10.newLoad(xc.Vector([0,0,P]))
nod11.newLoad(xc.Vector([0,0,P]))
nod12.newLoad(xc.Vector([0,0,P]))
nod13.newLoad(xc.Vector([0,0,P]))
nod14.newLoad(xc.Vector([0,0,P]))
nod15.newLoad(xc.Vector([0,0,P]))
## We add the load case to domain.
lPatterns.addToDomain(lp0.name)

# # Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# # Uncomment to display the mesh
# oh.displayFEMesh()

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

elementStresses= brick0.physicalProperties.getVectorMaterials.generalizedStresses
nodeStresses= brick0.getExtrapolatedValues(elementStresses)

sz= nodeStresses.noRows
sZnodes= list()
sZerr= 0.0
tauErr= 0.0
for i in range(0,sz):
    sZ= nodeStresses(i,2)
    s12= nodeStresses(i,3)/2.0
    s23= nodeStresses(i,4)/2.0
    s31= nodeStresses(i,5)/2.0
    sZnodes.append(sZ)
    sZerr= (sZ-12.0*P)**2
    tauErr= s12**2+s23**2+s31**2

sZerr/= 8.0
tauErr/= 8.0

# oh.displayDispRot('uZ')
# oh.displayStresses('sigma_zz')
# oh.displayStrains('epsilon_xx')
# oh.displayStrains('epsilon_yy')

'''
gaussPoints= brick.gaussPointsPositions
print(elementStresses)
print(nodeStresses)
print(sZnodes)
print(gaussPoints)
print(sZerr)
print(tauErr)
'''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if(sZerr<1e-20 and tauErr<1e-20):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
