# -*- coding: utf-8 -*-
''' Test 3-refinement algorithm as defined in: «Algorithms for Quadrilateral and Hexahedral Mesh Generation» Robert Schneiders. Test connectivity template numbrer 2b.

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
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Define nodes.
n0= modelSpace.newNodeXY(0, 0)
n1= modelSpace.newNodeXY(1, 0)
n2= modelSpace.newNodeXY(1, 1)
n3= modelSpace.newNodeXY(0, 1)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 30e6, nu= 0.3, rho= 0.0)
## Define elements.
modelSpace.setDefaultMaterial(elast2d)
quad1= modelSpace.newElement("FourNodeQuad",[n0.tag, n1.tag, n2.tag, n3.tag])

## Define loads.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("quad_strain_load")
eleLoad.elementTags= xc.ID([quad1.tag])
eleLoad.setStrainComp(0,0,1e-4) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,0,1e-4)
eleLoad.setStrainComp(2,0,1e-4)
eleLoad.setStrainComp(3,0,1e-4)


n0.setProp("subdivisionLevel",1) # mark diagonal nodes.
n2.setProp("subdivisionLevel",1)

xcTotalSet= modelSpace.getTotalSet()

maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)
while maxNodeSubdivisionLevel>0:
    mesh_refinement.refinement_step(modelSpace= modelSpace, xcSet= xcTotalSet)
    mesh_refinement.compute_subdivision_levels(xcSet= xcTotalSet)
    maxNodeSubdivisionLevel= mesh_refinement.get_max_node_subdivision_level(xcSet= xcTotalSet)

nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesOK= (nNodes==12)
nElementsOK= (nElements==7)
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
