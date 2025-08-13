# -*- coding: utf-8 -*-
''' Test PotBearing class.'''
from __future__ import print_function
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

FX= 1.0e4 # Force magnitude
FY= 2.0e4 
FZ= 3.0e4
MX= 4.0
MY= 5.0 
MZ= 6.0

import xc

from model import predefined_spaces
from materials import bridge_bearings
from solution import predefined_solutions



diamPot= 993e-3
ang=30 # counterclockwise angle (degrees) between local and global X-axes
angRad=math.radians(ang)
vDirXlocal=xc.Vector([math.cos(angRad),math.sin(angRad),0])
vDirYlocal=xc.Vector([-math.sin(angRad),math.cos(angRad),0])


# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# multi-directional POT matarial
pot_mat= bridge_bearings.PTFEPotBearingMat(d=diamPot,unidirX=False,unidirY=False)
pot_mat.defineMaterials(preprocessor)


nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nod1= nodes.newNodeXYZ(1,1,1)
nod2= nodes.newNodeXYZ(1,1,1)

# POT bearing element with local X-axis 30 degrees counterclockwise from global X-axis
potBearing= bridge_bearings.PotBearing(pot_mat,nod1,nod2,orientation=[vDirXlocal,vDirYlocal])


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

FXres=potBearing.getFXlocal()
FYres=potBearing.getFYlocal()
FXglobal=FXres*math.cos(angRad)-FYres*math.sin(angRad)
FYglobal=FXres*math.sin(angRad)+FYres*math.cos(angRad)
ElongXlres=potBearing.getElongXlocal()
ElongYlres=potBearing.getElongYlocal()
uNod2= potBearing.getUxUyUzNodB()

ratio1= abs(FXglobal-FX)/FX
ratio2= abs(FYglobal-FY)/FY
ratio3= abs(uNod2.dot(vDirYlocal.Normalized())-ElongYlres)

''' 
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
   '''

# This test has an intermittent fault the error messages have been extended
# to locate the variable that causes the error. LP 20231108
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<1e-15) & (ratio2<1e-15) &  (ratio3<1e-10):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    lmsg.error('ratio1= '+str(ratio1))
    lmsg.error('ratio2= '+str(ratio2))
    lmsg.error('ratio3= '+str(ratio3)) # ratio3= 1.2646541989592697e+284 08/11/2023
    lmsg.error('  uNod2= '+str(uNod2))
    lmsg.error('  vDirYlocal= '+str(vDirYlocal))
    lmsg.error('  ElongYlres= '+str(ElongYlres))
