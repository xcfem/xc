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
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Define nodes.
n0= modelSpace.newNodeXYZ(0, 0, 0)
n1= modelSpace.newNodeXYZ(0, 8, 0)
n2= modelSpace.newNodeXYZ(0, 8, 15)
n3= modelSpace.newNodeXYZ(0, 0, 15)
n4= modelSpace.newNodeXYZ(0, 15, 0)
n5= modelSpace.newNodeXYZ(0, 15, 8)
n6= modelSpace.newNodeXYZ(0, 25, 0)
n7= modelSpace.newNodeXYZ(0, 25, 12)
n8= modelSpace.newNodeXYZ(0, 8, 25)
n9= modelSpace.newNodeXYZ(0, 0, 25)
n10= modelSpace.newNodeXYZ(0, 20, 22)
n11= modelSpace.newNodeXYZ(0, 32, 12)
n12= modelSpace.newNodeXYZ(0, 32, 22)
n13= modelSpace.newNodeXYZ(0, 42, 12)
n14= modelSpace.newNodeXYZ(0, 42, 22)
n15= modelSpace.newNodeXYZ(0, 52, 12)
n16= modelSpace.newNodeXYZ(0, 52, 22)
n17= modelSpace.newNodeXYZ(0, 15, 31)
n18= modelSpace.newNodeXYZ(0, 25, 37)
n19= modelSpace.newNodeXYZ(0, 15, 40)
n20= modelSpace.newNodeXYZ(0, 32, 32)
n21= modelSpace.newNodeXYZ(0, 42, 32)
n22= modelSpace.newNodeXYZ(0, 52, 32)
n23= modelSpace.newNodeXYZ(0, 7, 36)
n24= modelSpace.newNodeXYZ(0, 0, 36)
n25= modelSpace.newNodeXYZ(0, 42, 42)
n26= modelSpace.newNodeXYZ(0, 52, 42)

## Define material.
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E= 2.1e6, nu= 0.3, h= 0.1, rho= 1.33)
## Define elements.
modelSpace.setDefaultMaterial(shellMat)
shell1= modelSpace.newElement("ShellMITC4",[n0.tag, n1.tag, n2.tag, n3.tag])
shell2= modelSpace.newElement("ShellMITC4",[n1.tag, n4.tag, n5.tag, n2.tag])
shell3= modelSpace.newElement("ShellMITC4",[n4.tag, n6.tag, n7.tag, n5.tag])
shell4= modelSpace.newElement("ShellMITC4",[n3.tag, n2.tag, n8.tag, n9.tag])
shell5= modelSpace.newElement("ShellMITC4",[n2.tag, n5.tag, n7.tag, n10.tag])
shell6= modelSpace.newElement("ShellMITC4",[n7.tag, n11.tag, n12.tag, n10.tag])
shell7= modelSpace.newElement("ShellMITC4",[n11.tag, n13.tag, n14.tag, n12.tag])
shell8= modelSpace.newElement("ShellMITC4",[n13.tag, n15.tag, n16.tag, n14.tag])
shell9= modelSpace.newElement("ShellMITC4",[n8.tag, n2.tag, n10.tag, n17.tag])
shell10= modelSpace.newElement("ShellMITC4",[n17.tag, n10.tag, n18.tag, n19.tag])
shell11= modelSpace.newElement("ShellMITC4",[n10.tag, n12.tag, n20.tag, n18.tag])
shell12= modelSpace.newElement("ShellMITC4",[n12.tag, n14.tag, n21.tag, n20.tag])
shell13= modelSpace.newElement("ShellMITC4",[n21.tag, n14.tag, n16.tag, n22.tag])
shell14= modelSpace.newElement("ShellMITC4",[n9.tag, n8.tag, n23.tag, n24.tag])
shell15= modelSpace.newElement("ShellMITC4",[n8.tag, n17.tag, n19.tag, n23.tag])
shell16= modelSpace.newElement("ShellMITC4",[n18.tag, n20.tag, n21.tag, n25.tag])
shell17= modelSpace.newElement("ShellMITC4",[n25.tag, n21.tag, n22.tag, n26.tag])

# Subdivision level assignment
p1= geom.Pos3d(0.0, 35.0, 27.0) # Level 1
p2= geom.Pos3d(0.0, 20.0, 5.0) # Level 2
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
# oh.displayFEMesh()
# oh.displayLocalAxes()
# # oh.displayReactions()
# # oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# # oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
