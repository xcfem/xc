# -*- coding: utf-8 -*-
from __future__ import print_function
'''Rigid beam between two nodes. Home made test'''

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
import math

# Geometry
L= 15 # Bar length (m)

# Load
F= 1.5e3 # Load magnitude (kN)

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics3D(nodes)
nod1= nodes.newNodeXYZ(0,0.0,0.0)
nod2= nodes.newNodeXYZ(L,0.0,0.0)


# Constraints
constraints= preprocessor.getBoundaryCondHandler

modelSpace.fixNode000(nod1.tag)
fulcrum= modelSpace.setFulcrumBetweenNodes(nod1.tag,nod2.tag)

# Load definition.
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nod2.tag,xc.Vector([-F,F,-F]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution

analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)


deltaX= nod2.getDisp[0]
deltaY= nod2.getDisp[0]
deltaZ= nod2.getDisp[0]  # Node 2 displacement


ratio1= math.sqrt(deltaX**2+deltaY**2+deltaZ**2)

''' 
    print(deltaX)
    print(deltaY)
    print(deltaZ)
    print(ratio1)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ratio1<1e-11:
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
