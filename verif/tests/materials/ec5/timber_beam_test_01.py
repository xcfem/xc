# -*- coding: utf-8 -*-
'''
Example 2.3.1 of the book "Calcul de structures en bois. Yves Benoit • Bernard Legrand • Vincent Tastet. Deuxième tirage 2008. EYROLLES AFNOR"
'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2021, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import math
import xc_base
import geom
import xc

from solution import predefined_solutions
from model import predefined_spaces
from materials.ec5 import EC5_materials
from postprocess import output_handler

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
pt1= points.newPntFromPos3d(geom.Pos3d(0,0,0))
pt2= points.newPntFromPos3d(geom.Pos3d(L,0,0))

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
beam3d= seedElemHandler.newElement("ElasticTimoshenkoBeam3d",xc.ID([0,0]))
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
    

delta= nC.getDisp[2]
# Analytical solution
elasticMaterial= wood.getElasticMaterial()
EI= elasticMaterial.E*section.Iz()
AG= elasticMaterial.G()*section.A()
sry= 1.0/section.alphaY()
v1= 5.0*ulsLoad*L**4/(384.0*EI)
v2= L*L*ulsLoad*sry/(8.0*AG)
deltaRef= -(v1+v2)

ratio1= abs(delta-deltaRef)/deltaRef

MMax= 0.0
for e in xcTotalSet.elements:
    e.getResistingForce()
    Mz1= e.getMz1
    Mz2= e.getMz2
    MMax= max(MMax, abs(Mz1))
    MMax= max(MMax, abs(Mz2))

MRef= ulsLoad*L*L/8.0
ratio2= abs(MMax-MRef)/MRef

sgMax= MMax/section.Iz()*section.h/2
k_sys= 1.1 # System strength factor.
fm_d= section.getDesignBendingStrength(loadDurationClass= 'medium_term', serviceClass= 1, k_sys= 1.1)

CF= sgMax/fm_d
CFRef= 0.45573286576705063
ratio3= abs(CF-CFRef)/CFRef

# Instantaneous deflection.
# Load definition 1.
modelSpace.removeLoadCaseFromDomain(lp0.name) # Remove the previous load case.
lp1= modelSpace.newLoadPattern(name= 'SLS1')
q_inst1= 0.75e3 # kN/m
eleLoad= lp1.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID(xcTotalSet.getElementTags())
eleLoad.transComponent= -q_inst1
modelSpace.addLoadCaseToDomain(lp1.name) # We add the load case to domain.

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

w_q_inst1= nC.getDisp[2]
w_q_inst1_ref= -7.501047972907759e-3
ratio4= abs(w_q_inst1-w_q_inst1_ref)/w_q_inst1_ref

# Load definition 2.
modelSpace.removeLoadCaseFromDomain(lp1.name) # Remove the previous load case.
lp2= modelSpace.newLoadPattern(name= 'SLS2')
q_inst2= 1.0e3 # kN/m
eleLoad= lp2.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID(xcTotalSet.getElementTags())
eleLoad.transComponent= -q_inst2
modelSpace.addLoadCaseToDomain(lp2.name) # We add the load case to domain.

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

w_q_inst2= nC.getDisp[2]
w_q_inst2_ref= -10.001397297210477e-3
ratio5= abs(w_q_inst2-w_q_inst2_ref)/w_q_inst2_ref

# Load definition for creep.
modelSpace.removeLoadCaseFromDomain(lp2.name) # Remove the previous load case.
lp3= modelSpace.newLoadPattern(name= 'SLS3')
k_def= section.wood.getKDef(serviceClass= 1)
q_creep= k_def*(0.57e3+0.3*1.5e3)*spacing # kN/m
eleLoad= lp3.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID(xcTotalSet.getElementTags())
eleLoad.transComponent= -q_creep
modelSpace.addLoadCaseToDomain(lp3.name) # We add the load case to domain.

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
    

w_q_creep= nC.getDisp[2]
w_q_creep_ref= -3.06042757294636e-3
ratio6= abs(w_q_creep-w_q_creep_ref)/w_q_creep_ref

w_net_fin= w_q_inst2+w_q_creep

'''
print(scc)
print('deltaRef= ', deltaRef, ' mm')
print('delta= ', delta*1e3, ' mm')
print('ratio1= ', ratio1)
print('MMax= ', MMax/1e3, ' kN.m')
print('MRef= ', MRef/1e3, ' kN.m')
print('ratio2= ', ratio2)

print('sgMax= ', sgMax/1e6, ' MPa')
print('fm_d= ', fm_d/1e6, ' MPa')
print('CF= ', CF, ' MPa')
print('ratio3= ', ratio3)

print('w_q_inst1= ', w_q_inst1*1e3, ' mm')
print('w_q_inst1_ref= ', w_q_inst1_ref*1e3, ' mm')
print('ratio4= ', ratio4)

print('w_q_inst2= ', w_q_inst2*1e3, ' mm')
print('w_q_inst2_ref= ', w_q_inst2_ref*1e3, ' mm')
print('ratio5= ', ratio5)

print('w_q_creep= ', w_q_creep*1e3, ' mm')
print('w_q_creep_ref= ', w_q_creep_ref*1e3, ' mm')
print('ratio6= ', ratio6)

print('w_net_fin= ', w_net_fin*1e3, ' mm')
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((abs(ratio1)<1e-9) and (abs(ratio2)<1e-9) and (abs(ratio3)<1e-9) and (abs(ratio4)<1e-9) and (abs(ratio5)<1e-9) and (abs(ratio6)<1e-9)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
#########################################################
# Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

## Uncomment to display blocks.
#oh.displayBlocks()

## Uncomment to display the mesh.
#oh.displayFEMesh()

## Uncomment to display the directions of the strong and weak axes.
# oh.displayStrongWeakAxis()

## Uncomment to display the loads.
# oh.displayLoads()

## Uncomment to display the vertical displacement.
# oh.displayDispRot(itemToDisp='uZ')
#oh.displayNodeValueDiagram(itemToDisp='uX')

## Uncomment to display the reactions.
#oh.displayReactions()

## Uncomment to display the internal force.
# oh.displayIntForcDiag('Mz')

