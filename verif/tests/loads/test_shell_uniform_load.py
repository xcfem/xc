# -*- coding: utf-8 -*-
''' Trivial test for uniform load on shell element.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT  AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.Ortega@ciccp.es"


import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

v1= xc.Vector([0,math.sqrt(2)/2,math.sqrt(2)/2])
v2= xc.Vector([0,-math.sqrt(2)/2,math.sqrt(2)/2])

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(2,0,0)
n3= nodes.newNodeXYZ(2,2,0)
n4= nodes.newNodeXYZ(0,2,0)

# Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.1 # thickness.
dens= 1.33 # specific mass [kg/m2].
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

iVector= elem.getIVector3d(True)
jVector= elem.getJVector3d(True)
kVector= elem.getKVector3d(True)

# Constraints
modelSpace.fixNode000_FFF(n1.tag)
modelSpace.fixNode000_FFF(n2.tag)
modelSpace.fixNode000_FFF(n3.tag)
modelSpace.fixNode000_FFF(n4.tag)

# Loads definition
iComponent= -1.0e3
jComponent= -2.0e3
kComponent= -3.0e3
area= elem.getArea(True)
force= area*(iComponent*iVector+jComponent*jVector+kComponent*kVector)

lp0= modelSpace.newLoadPattern(name= '0')

eleLoad= lp0.newElementalLoad("shell_uniform_load")
eleLoad.elementTags= xc.ID([elem.tag]) 
eleLoad.transComponent= kComponent
eleLoad.axial1Component= iComponent
eleLoad.axial2Component= jComponent

# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

reaction= xc.Vector(6*[0.0])
for n in [n1,n2,n3,n4]:
    reaction+= n.getReaction

reactionForce= geom.Vector3d(reaction[0], reaction[1], reaction[2])
err= (reactionForce+force).getModulus()

'''
print('force: ', force)
print('reaction: ', reactionForce)
print(err)
'''

# # #########################################################
# # # Graphic stuff.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(modelSpace)

# #oh.displayFEMesh()
# oh.displayLoads()

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-10:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
