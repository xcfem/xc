# -*- coding: utf-8 -*-
''' Test 3-refinement algorithm as defined in: «Algorithms for Quadrilateral and Hexahedral Mesh Generation» Robert Schneiders. Shell elements. Test connectivity template number 4.

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

## Define material.
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E= 2.1e6, nu= 0.3, h= 0.1, rho= 1.33)
## Define elements.
modelSpace.setDefaultMaterial(shellMat)
shell1= modelSpace.newElement("ShellMITC4",[n0.tag, n1.tag, n2.tag, n3.tag])

## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern(lp0.name)
loadVector= xc.Vector([1.0,0.0,0.0])
elementLoad= shell1.vector3dUniformLoadGlobal(loadVector)
modelSpace.addLoadCaseToDomain(lp0.name)

SL= 2
n0.setProp("subdivisionLevel", SL) # mark all nodes.
n1.setProp("subdivisionLevel", SL)
n2.setProp("subdivisionLevel", SL)
n3.setProp("subdivisionLevel", SL)

xcTotalSet= modelSpace.getTotalSet()

maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)
while maxNodeSubdivisionLevel>0:
    mesh_refinement.refinement_step(modelSpace= modelSpace, xcSet= xcTotalSet)
    mesh_refinement.compute_subdivision_levels(xcSet= xcTotalSet)
    maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)

nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesOK= (nNodes==100)
nElementsOK= (nElements==81)
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
