# -*- coding: utf-8 -*-
''' Deflection of a cantilever beam due to a temperature gradient.

    Strain component 4: curvature around local x axis 
    (normal to local y axis)

    Home made test based on the corresponding classical formulas.
'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2023, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

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
modelSpace.addLoadCaseToDomain(lp0.name) # We add the load case to domain.

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)

### Get displacements.
zDisp= 0.0
count= 0
# Get the line at x= L
for l in xcTotalSet.lines:
    center= l.getCentroid()
    if(abs(center.y-L)<1e-4):
        for n in l.nodes:
            zDisp+= n.getDisp[2]
            count+= 1
zDisp/=count

# Check result.
zDispRef= -temperatureGradient*alpha*L**2/2.0/thickness# Deflection of a cantileve
ratio= abs(zDisp-zDispRef)/zDispRef

'''
print("zDeflection= ", zDisp*1e3, 'mm')
print("zDeflectionRef= ", zDispRef*1e3, 'mm')
print("ratio= ", ratio)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(abs(ratio)<1e-8):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# for strainLabel in modelSpace.shellStrainComponents:
#     oh.displayLoads(elLoadComp= strainLabel)
# # # oh.displayDispRot(itemToDisp='uZ', defFScale= 100.0)
# # # oh.displayReactions()
