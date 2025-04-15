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

# Problem data.
L= 10.0 # Length of the beam
E= 2.1e6*9.81/1e-4 # Elastic modulus
b= 1 # width of the beam

alpha= 1.2e-5 # Thermal expansion coefficient of steel 1/ºC
Ttop=20 # Temperature at top side (Celsius degrees)
Tbottom=10 # Temperature at bottom side (Celsius degrees)
thickness= 0.2

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

## Problem geometry.
### K-points.
pt1= modelSpace.newKPoint(0,0,0)
pt2= modelSpace.newKPoint(b,0,0)
pt3= modelSpace.newKPoint(b,L,0)
pt4= modelSpace.newKPoint(0,L,0)
### Surface
s= modelSpace.newQuadSurface(pt1, pt2, pt3, pt4)
s.setElemSizeIJ(0.5,0.5)

## FE mesh
### Define seed element.
#### Material
mat= typical_materials.defElasticMembranePlateSection(preprocessor, name= "mat",E= E, nu= 0.3, rho= 0.0, h= thickness)
modelSpace.setDefaultMaterial(mat)
modelSpace.newSeedElement("ShellMITC4")

### Generate mesh.
s.genMesh(xc.meshDir.I)

### Constraints.
xcTotalSet= modelSpace.getTotalSet()
# Get the line at y= 0
for l in xcTotalSet.lines:
    center= l.getCentroid()
    if(center.y==0.0):
        for n in l.nodes:
            modelSpace.fixNode('000_0FF', n.tag)
            
## Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
eleLoad= lp0.newElementalLoad("shell_strain_load")
eleLoad.elementTags= xc.ID(xcTotalSet.getElementTags())
temperatureGradient= 13.2 # temperature gradient Ttop-Tbottom.
curvature= -temperatureGradient*alpha/thickness
# strain component 4: curvature around local x axis (normal to local y axis).
eleLoad.setStrainComp(0,4,curvature) #(id of Gauss point, id of component, value)
eleLoad.setStrainComp(1,4,curvature)
eleLoad.setStrainComp(2,4,curvature)
eleLoad.setStrainComp(3,4,curvature)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Graphic stuff.
from postprocess import output_handler
oh= output_handler.OutputHandler(modelSpace)

# for strainLabel in modelSpace.shellStrainComponents:
#     oh.displayLoads(elLoadComp= strainLabel)

fname= os.path.basename(__file__)
outputFileName= '/tmp/'+fname.replace('.py', '.jpeg')
oh.displayLoads(elLoadComp= 'epsilon_1', fileName= outputFileName)

# Check that file exists
testOK= os.path.isfile(outputFileName)

if testOK:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

os.remove(outputFileName)
