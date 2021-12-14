# -*- coding: utf-8 -*-
''' Home made test.'''
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
from materials.sections import section_properties
from materials.sections import def_secc_aggregation

L= 2 # Bar length.
P= 10e3 # Transverse uniform load.
n= 1e6 # Axial uniform load.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler

sectionTest= section_properties.RectangularSection("sectionTest",b=.20,h=.30) # Section geometry.
sectionTestMaterial= typical_materials.MaterialData("sectionTestMaterial",E=7E9,nu=0.3,rho=2500) # Section material.



# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Definimos el material
def_secc_aggregation.def_secc_aggregation3d(preprocessor, sectionTest,sectionTestMaterial)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(L/2,0,0)
nod= nodes.newNodeXYZ(L,0,0)

# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,0,1]))    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= sectionTest.sectionName
elements.numSections= 3 # Number of sections along the element.
elements.defaultTag= 1
beam3d1= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]))
beam3d2= elements.newElement("ForceBeamColumn3d",xc.ID([2,3]))
    
# Constraints
modelSpace.fixNode000_000(1)



# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam3d_uniform_load")
eleLoad.elementTags= xc.ID([1,2]) 
eleLoad.transComponent= -P 
eleLoad.axialComponent= n
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(1)

nodes.calculateNodalReactions(True,1e-7)
nod3= nodes.getNode(3)
delta0= nod3.getDisp[0] 
delta1= nod3.getDisp[1] 
nod1= nodes.getNode(1)
RN= nod1.getReaction[0] 
RN2= nod3.getReaction[0] 




elem1= elements.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]

N0= scc0.getStressResultantComponent("N")


F= (n*L)
delta0Teor= (n*L**2/2/sectionTestMaterial.E/sectionTest.A())
ratio0= ((delta0-delta0Teor)/delta0Teor)
Q= P*L
delta1Teor= (-Q*L**3/8/sectionTestMaterial.E/sectionTest.Iz())
ratio1= ((delta1-delta1Teor)/delta1Teor)
ratio2= (abs((N0-F)/F))
ratio3= (abs((RN+F)/F))


''' 
print("delta0= ",delta0)
print("delta0Teor= ",delta0Teor)
print("ratio0= ",ratio0)
print("delta1= ",delta1)
print("delta1Teor= ",delta1Teor)
print("ratio1= ",ratio1)
print("F= ",F)
print("N0= ",N0)
print("ratio2= ",ratio2)
print("RN= ",RN)
print("RN2= ",RN2)
print("ratio3= ",ratio3)
 '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio0)<1e-4) & (abs(ratio1)<0.03) & (abs(ratio2)<1e-14) & (abs(ratio3)<1e-14):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
