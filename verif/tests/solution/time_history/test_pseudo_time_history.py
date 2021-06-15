# -*- coding: utf-8 -*-
''' Test that shows that we can simulate a time history using the
    partial safety factor of the load pattern.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

E= 30e6 # Young modulus (psi)
l= 10 # Bar length in inches
F1= 1000 # Force magnitude 1 (pounds)

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(0,l)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",E)

''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''

elements= preprocessor.getElementHandler
elements.dimElem= 2 # Bars defined ina a two dimensional space.
elements.defaultMaterial= elast.name
truss= elements.newElement("Truss",xc.ID([n1.tag,n2.tag]))
truss.sectionArea= 1

constraints= preprocessor.getBoundaryCondHandler
# Constrain the displacement of node 1.
spc1= constraints.newSPConstraint(n1.tag,0,0.0)
spc2= constraints.newSPConstraint(n1.tag,1,0.0)
# Constrain the displacement of node 4.
spc3= constraints.newSPConstraint(n2.tag,0,0.0)

# time series for the load pattern:
# Load case definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(n2.tag,xc.Vector([0,F1]))

# Add the load pattern to the domain.
modelSpace.addLoadCaseToDomain(lp0.name)

# Solution
lp0.gammaF= 0.5
result= modelSpace.analyze(calculateNodalReactions= True)

R= n1.getReaction[1]

ratio1= (R+500)/500

lp0.gammaF= 0.75
result= modelSpace.analyze(calculateNodalReactions= True)

R= n1.getReaction[1]

ratio2= (R+750)/750

lp0.gammaF= 1.0
result= modelSpace.analyze(calculateNodalReactions= True)

R= n1.getReaction[1]

ratio3= (R+1000)/1000

'''
print("R= ",R)
print("ratio1= ",ratio1)
print("ratio2= ",ratio2)
print("ratio3= ",ratio3)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15 and abs(ratio2)<1e-15 and abs(ratio3)<1e-15:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


