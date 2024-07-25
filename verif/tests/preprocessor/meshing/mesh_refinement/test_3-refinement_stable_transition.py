# -*- coding: utf-8 -*-
''' Test 3-refinement algorithm as defined in: «Algorithms for Quadrilateral and Hexahedral Mesh Generation» Robert Schneiders.

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
from model.mesh import mesh_refinement

from model import predefined_spaces
from materials import typical_materials

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Don't print warnings
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

## Define nodes.
n0= modelSpace.newNodeXY(0, 0)
n1= modelSpace.newNodeXY(1, 0)
n2= modelSpace.newNodeXY(2, 0)
n3= modelSpace.newNodeXY(0, 1)
n4= modelSpace.newNodeXY(1, 1)
n5= modelSpace.newNodeXY(2, 1)
n6= modelSpace.newNodeXY(0, 2)
n7= modelSpace.newNodeXY(1, 2)
n8= modelSpace.newNodeXY(2, 2)
n9= modelSpace.newNodeXY(0, 3)
n10= modelSpace.newNodeXY(1, 3)
n11= modelSpace.newNodeXY(2, 3)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 30e6, nu= 0.3, rho= 0.0)
## Define elements.
modelSpace.setDefaultMaterial(elast2d)
quad1= modelSpace.newElement("FourNodeQuad",[n0.tag, n1.tag, n4.tag, n3.tag])
quad2= modelSpace.newElement("FourNodeQuad",[n1.tag, n2.tag, n5.tag, n4.tag])
quad3= modelSpace.newElement("FourNodeQuad",[n3.tag, n4.tag, n7.tag, n6.tag])
quad4= modelSpace.newElement("FourNodeQuad",[n4.tag, n5.tag, n8.tag, n7.tag])
quad5= modelSpace.newElement("FourNodeQuad",[n6.tag, n7.tag, n10.tag, n9.tag])
quad6= modelSpace.newElement("FourNodeQuad",[n7.tag, n8.tag, n11.tag, n10.tag])

# Subdivision level assignment
SL= 2
quad1.setProp("subdivisionLevel", SL)
quad2.setProp("subdivisionLevel", SL)

xcTotalSet= modelSpace.getTotalSet()
maxNodeSubdivisionLevel= mesh_refinement.refine_mesh(modelSpace= modelSpace, xcSet= xcTotalSet)
            
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesOK= (nNodes==238)
nElementsOK= (nElements==214)
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
