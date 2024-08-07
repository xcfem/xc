# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FX= 1.0e5 # Force magnitude
FY= 2.0e5 
FZ= 3.0e5
MX= 4.0
MY= 5.0 
MZ= 6.0

import xc

from model import predefined_spaces
from materials import bridge_bearings
from solution import predefined_solutions



diamPot= 993e-3


# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# unidirectional Y POT material
pot_mat= bridge_bearings.PTFEPotBearingMat(d=diamPot,unidirX=False,unidirY=True)
pot_mat.defineMaterials(preprocessor)

nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(1,1,1)
nod2= nodes.newNodeXYZ(1,1,1)



potBearing= bridge_bearings.PotBearing(pot_mat,nod1,nod2)


# Constraints
modelSpace.fixNode000_000(nod1.tag)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,3,0.0) # Node 2
spc= modelSpace.constraints.newSPConstraint(nod2.tag,4,0.0)
spc= modelSpace.constraints.newSPConstraint(nod2.tag,5,0.0)


# Loads definition
lp0= modelSpace.newLoadPattern(name= '0')
lp0.newNodalLoad(nod2.tag,xc.Vector([FX,FY,FZ,MX,MY,MZ]))
# We add the load case to domain.
modelSpace.addLoadCaseToDomain(lp0.name)


# Solution
analysis= predefined_solutions.simple_static_linear(feProblem)
result= analysis.analyze(1)
nodes.calculateNodalReactions(False, 1e-7)

# Compute the reaction vector.
FXres=potBearing.getFXlocal()
FYres=potBearing.getFYlocal()
ux=nod2.getDisp[0]
uz=nod2.getDisp[2]

ratio1=abs(FXres-FX)/FX
ratio2=abs(FYres-FY)/FY
ratio3=abs(ux+uz)


''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-5) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    lmsg.error('ratio1= ', str(ratio1))
    lmsg.error('ratio2= ', str(ratio2))
    lmsg.error('ratio3= ', str(ratio3))
