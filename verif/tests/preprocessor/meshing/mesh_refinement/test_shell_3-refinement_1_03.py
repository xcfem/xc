# -*- coding: utf-8 -*-
''' Test 3-refinement algorithm as defined in: «Algorithms for Quadrilateral and Hexahedral Mesh Generation» Robert Schneiders. Shell elements. Test connectivity template number 1.

'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


import geom
import xc

from model import predefined_spaces
from materials import typical_materials
from model.mesh import mesh_refinement

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't print warnings
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes.
n0= modelSpace.newNodeXYZ(0, 0, 0)
n1= modelSpace.newNodeXYZ(0, 1, 0)
n2= modelSpace.newNodeXYZ(0, 1, 1)
n3= modelSpace.newNodeXYZ(0, 0, 1)

n10= modelSpace.newNodeXYZ(0, 3, 0)
n11= modelSpace.newNodeXYZ(0, 3+1, 0)
n12= modelSpace.newNodeXYZ(0, 3+1, 1)
n13= modelSpace.newNodeXYZ(0, 3, 1)

n20= modelSpace.newNodeXYZ(0, 3, 3)
n21= modelSpace.newNodeXYZ(0, 3+1, 3)
n22= modelSpace.newNodeXYZ(0, 3+1, 3+1)
n23= modelSpace.newNodeXYZ(0, 3, 3+1)

n30= modelSpace.newNodeXYZ(0, 0, 3)
n31= modelSpace.newNodeXYZ(0, 1, 3)
n32= modelSpace.newNodeXYZ(0, 1, 3+1)
n33= modelSpace.newNodeXYZ(0, 0, 3+1)

## Define material.
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E= 2.1e6, nu= 0.3, h= 0.1, rho= 1.33)
## Define elements.
modelSpace.setDefaultMaterial(shellMat)
shell1= modelSpace.newElement("ShellMITC4",[n0.tag, n1.tag, n2.tag, n3.tag])
shell2= modelSpace.newElement("ShellMITC4",[n10.tag, n11.tag, n12.tag, n13.tag])
shell3= modelSpace.newElement("ShellMITC4",[n20.tag, n21.tag, n22.tag, n23.tag])
shell4= modelSpace.newElement("ShellMITC4",[n30.tag, n31.tag, n32.tag, n33.tag])


## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadVector= xc.Vector([1.0,0.0,0.0])
elementLoad= shell1.vector3dUniformLoadGlobal(loadVector)
modelSpace.addLoadCaseToDomain(lp0.name)

# Set subdivision levels.
## Quad 1
n0.setProp("subdivisionLevel",2)
n1.setProp("subdivisionLevel",0)
n2.setProp("subdivisionLevel",0)
n3.setProp("subdivisionLevel",0)
## Quad 2
n11.setProp("subdivisionLevel",2)
n10.setProp("subdivisionLevel",0)
n12.setProp("subdivisionLevel",0)
n13.setProp("subdivisionLevel",0)
## Quad 3
n22.setProp("subdivisionLevel",2)
n21.setProp("subdivisionLevel",0)
n20.setProp("subdivisionLevel",0)
n23.setProp("subdivisionLevel",0)
## Quad 4
n33.setProp("subdivisionLevel",2)
n31.setProp("subdivisionLevel",0)
n32.setProp("subdivisionLevel",0)
n30.setProp("subdivisionLevel",0)

xcTotalSet= modelSpace.getTotalSet()

maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)
while maxNodeSubdivisionLevel>0:
    mesh_refinement.refinement_step(modelSpace= modelSpace, xcSet= xcTotalSet)
    mesh_refinement.compute_subdivision_levels(xcSet= xcTotalSet)
    maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)

nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesOK= (nNodes==4*10)
nElementsOK= (nElements==4*5)
testOK= (maxNodeSubdivisionLevel==0) and nNodesOK and nElementsOK

'''
print('number of nodes: ', nNodes)
print('number of elements: ', nElements)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
