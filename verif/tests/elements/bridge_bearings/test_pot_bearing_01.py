# -*- coding: utf-8 -*-
''' Home made test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FX= 1.0e4 # Force magnitude
FY= 2.0e4 
FZ= 3.0e4
MX= 4.0e-5
MY= 5.0e-5 
MZ= 6.0

import xc

from materials import bridge_bearings
from model import predefined_spaces
from solution import predefined_solutions

diamPot= 993e-3
rotStiff=1e-5

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Multi-directional pot material 
pot_mat= bridge_bearings.PTFEPotBearingMat(d=diamPot,unidirX=False,unidirY=False,rotStiff=rotStiff)
pot_mat.defineMaterials(preprocessor)

nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(1,1,1)
nod2= nodes.newNodeXYZ(1,1,1)

potBearing= bridge_bearings.PotBearing(pot_mat,nod1,nod2)

# Constraints
modelSpace.fixNode000_000(nod1.tag)
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
reac= potBearing.getReaction()
R= xc.Vector([reac[0],reac[1],reac[2]])
FXres=potBearing.getFXlocal()
FYres=potBearing.getFYlocal()
rotX=potBearing.getRotXlocal()
rotY=potBearing.getRotYlocal()

ratio1= abs(R[0]+FX)/FX
ratio2= abs(R[1]+FY)/FY
ratio3= abs(R[2]+FZ)/FZ
ratio4=abs(FXres-FX)/FX
ratio5=abs(FYres-FY)/FY
ratio6=abs(rotX*rotStiff-MX)/MX
ratio7=abs(rotY*rotStiff-MY)/MY


''' 
print("RX= ",R[0])
print("RY= ",R[1])
print("RZ= ",R[2])
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) & (ratio2<1e-15) & (ratio3<1e-15) & (ratio4<1e-15) & (ratio5<1e-15) & (ratio6<1e-15) & (ratio7<1e-15):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')
