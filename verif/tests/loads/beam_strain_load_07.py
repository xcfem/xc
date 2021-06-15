# -*- coding: utf-8 -*-
from __future__ import print_function
''' home made test
    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 1.0 # Bar length (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
A= 4e-4 # bar area expressed in square meters
Iy= 80.1e-8 # Cross section moment of inertia (m4)
Iz= 8.49e-8 # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
AT= 10 # Temperature increment (Celsius degrees)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 # First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(L/2,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0.0)


lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
# Materials definition
section= typical_materials.defElasticShearSection3d(preprocessor, "section",A,E,G,Iz,Iy,J,1.0)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
elements.defaultTag= 1
beam1= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]))
beam2= elements.newElement("ForceBeamColumn3d",xc.ID([2,3]))

    
# Constraints

modelSpace.fixNode000_000(1)
modelSpace.fixNode000_000(3)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("beam_strain_load")
eleLoad.elementTags= xc.ID([1,2])
thermalDeformation= xc.DeformationPlane(alpha*AT)
eleLoad.backEndDeformationPlane= thermalDeformation
eleLoad.frontEndDeformationPlane= thermalDeformation



loadHandler= preprocessor.getLoadHandler

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution procedure
analysis= predefined_solutions.plain_static_modified_newton(feProblem)
result= analysis.analyze(1)


nod2= nodes.getNode(2)
dX= nod2.getDisp[0] 
dY= nod2.getDisp[1] 
dZ= nod2.getDisp[2] 

elem1= elements.getElement(1)
elem1.getResistingForce()
scc0= elem1.getSections()[0]


axil= scc0.getStressResultantComponent("N")
Ymoment= scc0.getStressResultantComponent("My")
Zmoment= scc0.getStressResultantComponent("Mz")
Yshear= scc0.getStressResultantComponent("Vy")
Zshear= scc0.getStressResultantComponent("Vz")



N= (-E*A*alpha*AT)
ratio= ((axil-N)/N)

''' 
print("dX= ",dX)
print("dY= ",dY)
print("dZ= ",dZ)
print("N= ",N)
print("axil= ",axil)
print("ratio= ",ratio)
print("Ymoment= ",Ymoment)
print("Zmoment= ",Zmoment)
print("Yshear= ",Yshear)
print("Zshear= ",Zshear)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio)<1e-10) & (abs(Ymoment)<1e-10) & (abs(Zmoment)<1e-10) & (abs(Yshear)<1e-10) & (abs(Zshear)<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
