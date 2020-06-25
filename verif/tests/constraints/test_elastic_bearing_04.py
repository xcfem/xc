# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

KX= 1000 # Spring constant
KY= 2000 # Spring constant
KZ= 3000 # Spring constant
FX= 1 # Force magnitude
FY= 2 
FZ= 3

import xc_base
import geom
import xc
from model import predefined_spaces
from model.boundary_cond import spring_bound_cond as springs

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nodXYZ= [nodes.newNodeXYZ(i,1,0) for i in range(3)] #nodes to be connected to
                                                    #springs in X,Y,Z directions
nodZ= [nodes.newNodeXYZ(i,2,0) for i in range(3)] #nodes to be connected to
                                                  #springs in Z direction only


#Generation on spring boundary conditions
springXYZ=springs.SpringBC(name='springXYZ',modelSpace=modelSpace,Kx=KX,Ky=KY,Kz=KZ)
springXYZ.applyOnNodesLst(Nodelist=nodXYZ)

fixedNodXYZ=[nodes.getNode(7),nodes.getNode(8),nodes.getNode(9)] #fixed nodes

springZ=springs.SpringBC(name='springZ',modelSpace=modelSpace,Kx=0,Ky=0,Kz=KZ)
springZ.applyOnNodesLst(Nodelist=nodZ)

fixedNodZ=[nodes.getNode(10),nodes.getNode(11),nodes.getNode(12)] #fixed nodes


# Constraints
constraints= preprocessor.getBoundaryCondHandler
for n in range(1,7):
    constraints.newSPConstraint(n,3,0.0)
    constraints.newSPConstraint(n,4,0.0)
    constraints.newSPConstraint(n,5,0.0)
for n in range(4,7):
    constraints.newSPConstraint(n,0,0.0)
    constraints.newSPConstraint(n,1,0.0)


# Loads definition
loadHandler= preprocessor.getLoadHandler

lPatterns= loadHandler.getLoadPatterns

#Load modulation.
ts= lPatterns.newTimeSeries("constant_ts","ts")
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
for n in range(1,4):
    lp0.newNodalLoad(n,xc.Vector([FX,FY,FZ,0,0,0]))
for n in range(4,7):
    lp0.newNodalLoad(n,xc.Vector([0,0,FZ,0,0,0]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)


# Solution
result= modelSpace.analyze(calculateNodalReactions= True)

deltX_nodXYZ=[nodXYZ[i].getDisp[0] for i in range(3)]
deltY_nodXYZ=[nodXYZ[i].getDisp[1] for i in range(3)]
deltZ_nodXYZ=[nodXYZ[i].getDisp[2] for i in range(3)]

RX_fixedNodXYZ=[fixedNodXYZ[i].getReaction[0] for i in range(3)]
RY_fixedNodXYZ=[fixedNodXYZ[i].getReaction[1] for i in range(3)]
RZ_fixedNodXYZ=[fixedNodXYZ[i].getReaction[2] for i in range(3)]

deltZ_nodZ=[nodZ[i].getDisp[2] for i in range(3)]

RZ_fixedNodZ=[fixedNodZ[i].getReaction[2] for i in range(3)]

'''
print('deltX_nodXYZ= ', deltX_nodXYZ)
print('deltY_nodXYZ= ', deltY_nodXYZ)
print('deltZ_nodXYZ= ', deltZ_nodXYZ)

print('RX_fixedNodXYZ= ', RX_fixedNodXYZ)
print('RY_fixedNodXYZ= ', RY_fixedNodXYZ)
print('RZ_fixedNodXYZ= ', RZ_fixedNodXYZ)

print('deltZ_nodZ= ', deltZ_nodZ)

print('RZ_fixedNodZ= ', RZ_fixedNodZ)
'''

ratio1=sum([-RX_fixedNodXYZ[i]/FX for i in range(3)])/3.0
ratio2=sum([KX*deltX_nodXYZ[i]/FX for i in range(3)])/3.0 
ratio3=sum([-RY_fixedNodXYZ[i]/FY for i in range(3)])/3.0
ratio4=sum([KY*deltY_nodXYZ[i]/FY for i in range(3)])/3.0 
ratio5=sum([-RZ_fixedNodXYZ[i]/FZ for i in range(3)])/3.0
ratio6=sum([KZ*deltZ_nodXYZ[i]/FZ for i in range(3)])/3.0 
ratio7=sum([-RZ_fixedNodZ[i]/FZ for i in range(3)])/3.0
ratio8=sum([KZ*deltZ_nodZ[i]/FZ for i in range(3)])/3.0 

'''
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("ratio3= ",(ratio3))
print("ratio4= ",(ratio4))
print("ratio5= ",(ratio5))
print("ratio6= ",(ratio6))
print("ratio7= ",(ratio7))
print("ratio8= ",(ratio8))
'''
  
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1-1.0)<1e-5) & (abs(ratio2-1.0)<1e-5) & (abs(ratio3-1.0)<1e-5) & (abs(ratio4-1.0)<1e-5)  & (abs(ratio5-1.0)<1e-5) & (abs(ratio6-1.0)<1e-5) & (abs(ratio7-1.0)<1e-5) & (abs(ratio8-1.0)<1e-5):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
