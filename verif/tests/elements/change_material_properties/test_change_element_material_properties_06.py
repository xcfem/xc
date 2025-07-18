# -*- coding: utf-8 -*-
'''Change the properties of the material of a ShellMITC4 element and
check that it works as expected.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= modelSpace.newNodeXYZ(0,0,0)
n2= modelSpace.newNodeXYZ(1,0,0)
n3= modelSpace.newNodeXYZ(1,1,0)
n4= modelSpace.newNodeXYZ(0,1,0)
n11= modelSpace.newNodeXYZ(0,0,1)
n12= modelSpace.newNodeXYZ(1,0,1)
n13= modelSpace.newNodeXYZ(1,1,1)
n14= modelSpace.newNodeXYZ(0,1,1)

# Materials definition
E= 2.1e9 # Steel Young's modulus Pa.
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 7850 # specific mass [kg/m3].
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat", E, nu, dens, h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= shellMat.name
shellA= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))
shellB= elements.newElement("ShellMITC4",xc.ID([n11.tag,n12.tag,n13.tag,n14.tag]))

factor= 5
# Access the materials on each integration point
for mat in shellB.physicalProperties.getVectorMaterials:
    mat.E= factor*E # Change material parameters.
    
newE= shellB.physicalProperties.getVectorMaterials[0].E

ratio1= (newE-factor*E)/E

# Constraints
modelSpace.fixNode000_FFF(n1.tag)
modelSpace.fixNode('000_FFF', n2.tag)
modelSpace.fixNode('FF0_FFF', n3.tag)
modelSpace.fixNode('FF0_FFF', n4.tag)
modelSpace.fixNode('000_FFF', n11.tag)
modelSpace.fixNode('000_FFF', n12.tag)
modelSpace.fixNode('FF0_FFF', n13.tag)
modelSpace.fixNode('FF0_FFF', n14.tag)

# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
F= 10e3 # Load magnitude.
lp0.newNodalLoad(n3.tag,xc.Vector([0,F,0,0,0,0]))
lp0.newNodalLoad(n4.tag,xc.Vector([0,F,0,0,0,0]))
lp0.newNodalLoad(n13.tag,xc.Vector([0,F,0,0,0,0]))
lp0.newNodalLoad(n14.tag,xc.Vector([0,F,0,0,0,0]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

uy3= n3.getDisp[1] # Node 3 yAxis displacement
uy4= n4.getDisp[1] # Node 4 yAxis displacement
uy13= n13.getDisp[1] # Node 13 yAxis displacement
uy14= n14.getDisp[1] # Node 14 yAxis displacement

ratio2= abs(uy3-factor*uy13)/abs(uy3)
ratio3= abs(uy4-factor*uy14)/abs(uy4)

'''
print('old Young modulus: ', oldE, ' ksi')
print('old poisson ratio: ', old_nu)
print('new Young modulus: ', newE, ' ksi')
print('new poisson ratio: ', new_nu)
print("ratio1= ",ratio1)
print("uy3= ",uy3*1e3, 'mm')
print("uy4= ",uy4*1e3, 'mm')
print("uy13= ",uy13*1e3, 'mm')
print("uy14= ",uy14*1e3, 'mm')
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-5) & (abs(ratio3)<1e-5):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# oh.displayFEMesh()
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
