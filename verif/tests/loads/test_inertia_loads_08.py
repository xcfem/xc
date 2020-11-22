# -*- coding: utf-8 -*-
''' Inertia load on MITC4 shell elements. 
    Equilibrium based home made test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2020, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials
import math

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(0,0,0)
n2= nodes.newNodeXYZ(1,0,0)
n3= nodes.newNodeXYZ(1,1,0)
n4= nodes.newNodeXYZ(0,1,0)

# Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.2 # thickness.
dens= 4.0 # specific mass [kg/m2].
memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)

elements= preprocessor.getElementHandler
elements.defaultMaterial= memb1.name
elem= elements.newElement("ShellNLDKGQ",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))
g= 1.0#9.81 # m/s2

# Element mass
eMass= elem.getArea(True)*dens*h
eForce= eMass*g
nForce= eForce/4.0

# Constraints.
constrainedNodes= [n1, n2, n3, n4]
for n in constrainedNodes:
    modelSpace.fixNode000_FFF(n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,g])
elem.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
modelSpace.analysis= predefined_solutions.penalty_newton_raphson(feProblem)
result= modelSpace.analyze(calculateNodalReactions= True)


zReactions= list()
for n in constrainedNodes:
    zReactions.append(n.getReaction[2])

err= 0.0
for r in zReactions:
    err+=(nForce-r)**2
err= math.sqrt(err)

'''
print(zReactions)
print('eMass= ', eMass)
print('eForce= ', eForce)
print('nForce= ', nForce)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(err)<1e-12 :
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')

