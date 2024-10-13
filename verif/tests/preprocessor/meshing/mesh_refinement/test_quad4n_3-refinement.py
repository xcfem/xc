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
n1= modelSpace.newNodeXY(8, 0)
n2= modelSpace.newNodeXY(8, 15)
n3= modelSpace.newNodeXY(0, 15)
n4= modelSpace.newNodeXY(15, 0)
n5= modelSpace.newNodeXY(15, 8)
n6= modelSpace.newNodeXY(25, 0)
n7= modelSpace.newNodeXY(25, 12)
n8= modelSpace.newNodeXY(8, 25)
n9= modelSpace.newNodeXY(0, 25)
n10= modelSpace.newNodeXY(20, 22)
n11= modelSpace.newNodeXY(32, 12)
n12= modelSpace.newNodeXY(32, 22)
n13= modelSpace.newNodeXY(42, 12)
n14= modelSpace.newNodeXY(42, 22)
n15= modelSpace.newNodeXY(52, 12)
n16= modelSpace.newNodeXY(52, 22)
n17= modelSpace.newNodeXY(15, 31)
n18= modelSpace.newNodeXY(25, 37)
n19= modelSpace.newNodeXY(15, 40)
n20= modelSpace.newNodeXY(32, 32)
n21= modelSpace.newNodeXY(42, 32)
n22= modelSpace.newNodeXY(52, 32)
n23= modelSpace.newNodeXY(7, 36)
n24= modelSpace.newNodeXY(0, 36)
n25= modelSpace.newNodeXY(42, 42)
n26= modelSpace.newNodeXY(52, 42)

## Define material.
elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E= 30e6, nu= 0.3, rho= 0.0)
## Define elements.
modelSpace.setDefaultMaterial(elast2d)
quad1= modelSpace.newElement("FourNodeQuad",[n0.tag, n1.tag, n2.tag, n3.tag])
quad2= modelSpace.newElement("FourNodeQuad",[n1.tag, n4.tag, n5.tag, n2.tag])
quad3= modelSpace.newElement("FourNodeQuad",[n4.tag, n6.tag, n7.tag, n5.tag])
quad4= modelSpace.newElement("FourNodeQuad",[n3.tag, n2.tag, n8.tag, n9.tag])
quad5= modelSpace.newElement("FourNodeQuad",[n2.tag, n5.tag, n7.tag, n10.tag])
quad6= modelSpace.newElement("FourNodeQuad",[n7.tag, n11.tag, n12.tag, n10.tag])
quad7= modelSpace.newElement("FourNodeQuad",[n11.tag, n13.tag, n14.tag, n12.tag])
quad8= modelSpace.newElement("FourNodeQuad",[n13.tag, n15.tag, n16.tag, n14.tag])
quad9= modelSpace.newElement("FourNodeQuad",[n8.tag, n2.tag, n10.tag, n17.tag])
quad10= modelSpace.newElement("FourNodeQuad",[n17.tag, n10.tag, n18.tag, n19.tag])
quad11= modelSpace.newElement("FourNodeQuad",[n10.tag, n12.tag, n20.tag, n18.tag])
quad12= modelSpace.newElement("FourNodeQuad",[n12.tag, n14.tag, n21.tag, n20.tag])
quad13= modelSpace.newElement("FourNodeQuad",[n21.tag, n14.tag, n16.tag, n22.tag])
quad14= modelSpace.newElement("FourNodeQuad",[n9.tag, n8.tag, n23.tag, n24.tag])
quad15= modelSpace.newElement("FourNodeQuad",[n8.tag, n17.tag, n19.tag, n23.tag])
quad16= modelSpace.newElement("FourNodeQuad",[n18.tag, n20.tag, n21.tag, n25.tag])
quad17= modelSpace.newElement("FourNodeQuad",[n25.tag, n21.tag, n22.tag, n26.tag])

# Subdivision level assignment
p1= geom.Pos3d(35.0, 27.0, 0.0) # Level 1
p2= geom.Pos3d(20.0, 5.0, 0.0) # Level 2
xcTotalSet= modelSpace.getTotalSet()

level1Element= xcTotalSet.elements.getNearestElement(p1)
level2Element= xcTotalSet.elements.getNearestElement(p2)
level1Element.setProp("subdivisionLevel",1)
level2Element.setProp("subdivisionLevel",2)

maxNodeSubdivisionLevel= mesh_refinement.refine_mesh(modelSpace= modelSpace, xcSet= xcTotalSet)
            
nNodes= len(xcTotalSet.nodes)
nElements= len(xcTotalSet.elements)

nNodesOK= (nNodes==209)
nElementsOK= (nElements==189)
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
# xcTotalSet.color= xc.Vector([1,1,1])
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
