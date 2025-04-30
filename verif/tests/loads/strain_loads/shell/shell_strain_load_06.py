# -*- coding: utf-8 -*-
''' Check the effect of a temperature gradient across the depth of a cantilever
beam.
'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

L= 1.0 # Plate length (m)
B= 0.2 # Plate depth (m)
E= 2.1e6*9.81/1e-4 # Elastic modulus
nu= 0.3
alpha= 1.2e-5 # Thermal expansion coefficient of the steel
AT= 400.0 # Temperature increment (Celsius degrees)
h= 0.2
A= L*h

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

# Materials definition
shellMat= typical_materials.defElasticMembranePlateSection(preprocessor, "shellMat",E,nu,0.0,h)

# Seed element definition
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= shellMat.name
elem= seedElemHandler.newElement("ShellMITC4")

# Block topology
#
#  p4 +------------------------------+ p3
#     |                              |
#     |                              |
#  p1 +------------------------------+ p2
#     |                              |
#     |                              |
#     +------------------------------+
#    p6                              p5
#
B_2= B/2.0
p1= modelSpace.newKPoint(0,0,0)
p2= modelSpace.newKPoint(L,0,0)
p3= modelSpace.newKPoint(L,0,B_2)
p4= modelSpace.newKPoint(0,0,B_2)
p5= modelSpace.newKPoint(L,0,-B_2)
p6= modelSpace.newKPoint(0,0,-B_2)
surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(p1.tag, p2.tag, p3.tag, p4.tag)
s2= surfaces.newQuadSurfacePts(p1.tag, p2.tag, p5.tag, p6.tag)

# Mesh generation
nDiv= 4
for s in [s1, s2]:
    s.nDivI= 8*nDiv
    s.nDivJ= nDiv
    s.genMesh(xc.meshDir.I)

# Constraints
xcTotalSet= modelSpace.getTotalSet()

for n in xcTotalSet.nodes:
    pos= n.getInitialPos3d
    if(abs(pos.x)<1e-3):
        if(abs(pos.z)<1e-3):
            modelSpace.fixNode('000_FF0',n.tag)
        else:
            modelSpace.fixNode('00F_FF0',n.tag)

# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
## Compute the strains corresponding to each element.
Ttop= AT/2.0
Tbottom= -AT/2.0
for e in xcTotalSet.elements:
    eTag= e.tag
    eleLoad= lp0.newElementalLoad("shell_strain_load")
    eleLoad.elementTags= xc.ID([eTag])
    ### Element Gauss points.
    gaussPoints= e.getGaussModel().getGaussPoints()
    for i, gp in enumerate(gaussPoints):
        gpPos= e.getCartesianCoordinates(gp, True)
        temp= (Ttop-Tbottom)/B*gpPos.z
        for compo in [0,1]: # Components
            eleLoad.setStrainComp(i, compo, alpha*temp) #(index of Gauss point, index of component, value)

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

#modelSpace.analysis= predefined_solutions.penalty_newton_raphson(feProblem)
result= modelSpace.analyze(calculateNodalReactions= True)

# Find node at the top center.
midEndNod= p2.getNode()

deltaZ= midEndNod.getDisp[2]
deltaZRef= -alpha*(Ttop-Tbottom)*L**2/B/2.0 # Theoretical displacement.
err= abs(deltaZ-deltaZRef)/-deltaZRef

'''
print('deltaZ= ', deltaZ)
print('deltaZRef= ', deltaZRef)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<.01):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)
# # oh.displayFEMesh()
# # oh.displayLocalAxes()
# for strainLabel in modelSpace.shellStrainComponents:
#     oh.displayLoads(elLoadComp= strainLabel, defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uX', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uY', defFScale= 10.0)
# oh.displayDispRot(itemToDisp='uZ', defFScale= 10.0)
# oh.displayIntForc(itemToDisp='N1', defFScale= 10.0)
# oh.displayIntForc(itemToDisp='N2', defFScale= 10.0)
# oh.displayReactions()

