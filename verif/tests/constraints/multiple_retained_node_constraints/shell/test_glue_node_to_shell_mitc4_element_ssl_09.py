# -*- coding: utf-8 -*-
''' Glue beam nodes to shell element. Home made test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

E= 2.1e9 # Young modulus of the steel en N/m2.
nu= 0.3 # Poisson's ratio.
h= 0.04 # Thickness (m).
rho= 7860 # Density kg/m2.

L= 2.0 # Table side (m).
a= .02 # Leg side (m).
A= a*a # Leg cross-section area (m2).
nu= 0.3 # Poisson's ratio.
G= E/(2.0*(1+nu))
Iy= a**4/12 # Inertia of the leg section (m4).
Iz= Iy
J= a**4/7.0 # Cross section torsion constant (m4)

import math
import geom
import xc
from scipy.constants import g
from model import predefined_spaces
from materials import typical_materials
from postprocess.quick_inquiry import nodal_reactions
from solution import predefined_solutions

def get_quads_from_array(nArray):
    ''' Generate the shell elements that cover the cells defined by the given 
        array.

    :param array: array of k-points.
    '''
    retval= list()
    nRows= len(nArray)
    nCols= len(nArray[0])
    for i in range(0, nRows-1):
        for j in range(0, nCols-1):
            n0= nArray[i][j]
            n1= nArray[i+1][j]
            n2= nArray[i+1][j+1]
            n3= nArray[i][j+1]
            retval.append([n0, n1, n2, n3])
    return retval

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

halfSide= L/2.0
xi= [-halfSide, 0, halfSide]
yi= [-halfSide, 0, halfSide]
tableNodes= list()
for y in yi:
    nodeRow= list()
    for x in xi:
        newNode= modelSpace.newNode(x, y, 0)
        nodeRow.append(newNode)
    tableNodes.append(nodeRow)

quarterSide= halfSide/2.0
xi= [-quarterSide, quarterSide]
yi= [-quarterSide, quarterSide]
legTopNodes= list()
for y in yi:
    for x in xi:
        newNode= modelSpace.newNode(x, y, 0)
        legTopNodes.append(newNode)

legBottomNodes= list()
for n in legTopNodes:
    pos= n.getInitialPos3d
    newNode= modelSpace.newNode(pos.x, pos.y, -1.5*halfSide)
    legBottomNodes.append(newNode)
    
# Materials definition
## Table.
tableMat= typical_materials.defElasticMembranePlateSection(preprocessor, "tableMat",E,nu,rho,h)
## Legs.
sectionProperties= xc.CrossSectionProperties3d()
sectionProperties.A= A; sectionProperties.E= E; sectionProperties.G= G
sectionProperties.Iz= Iz; sectionProperties.Iy= Iy; sectionProperties.J= J
legMat= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "scc",sectionProperties)

# Element definition.
## Table.
modelSpace.setDefaultMaterial(tableMat)
nodeQuads= get_quads_from_array(tableNodes)
tableElements= list()
for nQuad in nodeQuads:
    newElement= modelSpace.newElement("ShellMITC4", [nQuad[0].tag, nQuad[1].tag,nQuad[2].tag, nQuad[3].tag])
    tableElements.append(newElement)
## Legs.
### Geometric transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(legMat)
legElements= list()
for nTop, nBottom in zip(legTopNodes, legBottomNodes):
    newElement= modelSpace.newElement("ElasticBeam3d",xc.ID([nTop.tag,nBottom.tag]))

# Glue nodes.
nodePosErr= 0.0
xcTotalSet= modelSpace.getTotalSet()
gluedDOFs= xc.ID([0,1,2,3,4,5])
for n in legTopNodes:
    pos= n.getInitialPos3d
    nearestElement= xcTotalSet.getNearestElement(pos)
    distToElement2= nearestElement.getDist2(pos, True) # squared distance.
    nodePosErr+= distToElement2
    glue= modelSpace.glueNodeToElement(n.tag, nearestElement.tag, xc.ID(gluedDOFs))

# Constraints.
for nBottom in legBottomNodes:
    modelSpace.fixNode('000_FFF', nBottom.tag)
    
# Load definition.
lpG= modelSpace.newLoadPattern(name= 'G', setCurrent= True)
accel= xc.Vector([0,0,g])
for e in tableElements:
    e.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lpG.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem) # => OK
# analysis= predefined_solutions.transformation_krylov_newton(feProblem) # => KO
# analysis= predefined_solutions.penalty_newton_line_search_mumps(feProblem) # => OK
# analysis= predefined_solutions.penalty_newton_raphson(feProblem, maxNumIter= 2) # => KO
# analysis= predefined_solutions.transformation_newton_raphson_band_gen(feProblem, convergenceTestTol= 1e-8, printFlag= 1) # => KO
result= analysis.analyze(1)

nodes.calculateNodalReactions(False,1e-7)

# Check results.
nodePosErr= math.sqrt(nodePosErr)
Rx= 0; Ry= 0; Rz= 0
for n in legBottomNodes:
    R= n.getReaction
    Rx+= R[0]
    Ry+= R[1]
    Rz+= R[2]
refRz= rho*L*L*h*g
errRz= abs(Rz-refRz)/refRz

'''
print(nodePosErr)
print(Rx/1e3)
print(Ry/1e3)
print(Rz/1e3, refRz/1e3, errRz)
'''

testOK= (nodePosErr<1e-12) and (abs(Rx)<1e-10) and (abs(Ry)<1e-10) and (abs(errRz)<1e-5)
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic output
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# # oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp= 'uZ', defFScale= 100.0)

