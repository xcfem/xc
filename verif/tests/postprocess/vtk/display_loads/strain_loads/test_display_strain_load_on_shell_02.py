# -*- coding: utf-8 -*-
''' Check strain load representation on shell elements.

    Reference:  Cálculo de estructuras por el método de los elemen-
    tos finitos. 1991. E. Oñate, page 165, example 5.3

    isbn={9788487867002}
    url={https://books.google.ch/books?id=lV1GSQAACAAJ}

'''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Size of element edge (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
alpha= 1.2e-5 # Thermal expansion coefficient of steel 1/ºC
b= 1 # width of the element
# A= h*h # bar area expressed in square meters
# I= (h)**4/12 # Cross section moment of inertia (m4)
Ttop=10 # Temperature at top side (Celsius degrees)
Tbottom=0 # Temperature at bottom side (Celsius degrees)
thickness=2e-2

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
# Create nodes.
nod1= nodes.newNodeXYZ(0.0,b,0.0)
nod2= nodes.newNodeXYZ(L,b,0.0)
nod3= nodes.newNodeXYZ(L,0.0,0.0)
nod4= nodes.newNodeXYZ(0,0.0,0.0)


# Materials definition
memb1= typical_materials.defElasticMembranePlateSection(preprocessor=preprocessor, name="memb1",E=E,nu=0.3,rho=0.0,h=thickness)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem1= elements.newElement("ShellMITC4",xc.ID([nod4.tag,nod3.tag,nod2.tag,nod1.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(nod1.tag,0,0.0)
spc= constraints.newSPConstraint(nod1.tag,1,0.0)
spc= constraints.newSPConstraint(nod1.tag,2,0.0)
spc= constraints.newSPConstraint(nod1.tag,4,0.0)
spc= constraints.newSPConstraint(nod4.tag,0,0.0)
spc= constraints.newSPConstraint(nod4.tag,1,0.0)
spc= constraints.newSPConstraint(nod4.tag,2,0.0)
spc= constraints.newSPConstraint(nod4.tag,4,0.0)


# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID([elem1.tag])
curvature=alpha*(Tbottom-Ttop)/thickness  # rad/m
# Curvature is positive when the center of curvature
# lies along the direction of the normal to the element
# so, in this case, is negative.
eleLoad.setStrainComp(0,3,curvature) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,3,curvature)
eleLoad.setStrainComp(2,3,curvature)
eleLoad.setStrainComp(3,3,curvature)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in modelSpace.shellStrainComponents:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'kappa_1', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName) # Clean after yourself.
