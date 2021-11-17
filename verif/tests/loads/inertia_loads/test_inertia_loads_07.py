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
from materials import typical_materials
import math

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
n1= nodes.newNodeXYZ(-1,-1,0)
n2= nodes.newNodeXYZ(1,-2,0)
n3= nodes.newNodeXYZ(1,2,0)
n4= nodes.newNodeXYZ(-1,1,0)

# Materials definition
E= 2.1e6 # Steel Young's modulus [kg/cm2].
nu= 0.3 # Poisson's ratio.
h= 0.2 # thickness.
rho= 4.0 # specific mass [kg/m2].
plateMat= typical_materials.defElasticMembranePlateSection(preprocessor, "plateMat",E,nu,rho,h)
volRho= plateMat.rho
arealRho= plateMat.arealRho
refArealRho= rho*h
ratio0= abs(arealRho-refArealRho)/refArealRho

elements= preprocessor.getElementHandler
elements.defaultMaterial= plateMat.name
elem= elements.newElement("ShellMITC4",xc.ID([n1.tag,n2.tag,n3.tag,n4.tag]))

## Whole model mass data.
xcTotalSet= modelSpace.getTotalSet()
massZ= xcTotalSet.getTotalMassComponent(2)
massRefZ= refArealRho*elem.getArea(True)
ratio1= abs(massZ-massRefZ)/massRefZ

gravity= 1.0#9.81 # m/s2
# Element mass
eMass= elem.getArea(True)*rho*h
eForce= eMass*gravity

# Constraints.
constrainedNodes= [n1, n2, n3, n4]
for n in constrainedNodes:
    modelSpace.fixNode000_FFF(n.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
modelSpace.setCurrentLoadPattern("0")
accel= xc.Vector([0,0,gravity])
elem.createInertiaLoad(accel)
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
result= modelSpace.analyze(calculateNodalReactions= True)


zReaction= 0.0
for n in constrainedNodes:
    zReaction+= n.getReaction[2]

err= (eForce-zReaction)/eForce

'''
print('mass per volume unit; volRho= ', volRho, 'kg/m3')
print('mass per unit 2D area; arealRho= ', arealRho, 'kg/m2')
print('reference mass per unit 2D area; refArealRho= ', refArealRho, 'kg/m2')
print('ratio0= ', ratio0)
print('mass: ', massZ, 'kg')
print('reference mass: ', massRefZ, 'kg')
print('ratio1= ', ratio1)
print('zREaction= ', zReaction)
print('eMass= ', eMass)
print('eForce= ', eForce)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio0)<1e-12 and abs(ratio1)<1e-12 and abs(err)<1e-12 :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

