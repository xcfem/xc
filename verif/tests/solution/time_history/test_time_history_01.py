# -*- coding: utf-8 -*-
'''This example demonstrates how to perform time history analysis of a 2D elastic reinforced concrete cantilever column with a gravity load included in the analysis. See Ex1a.Canti2D.EQ.modif.tcl in OpenSees'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Original version in OpenSees:
# EQ ground motion with gravity
# all units are in kip, inch, second
# elasticBeamColumn ELEMENT
#		Silvia Mazzoni & Frank McKenna, 2006
#
#    ^Y
#    |
#    2       __ 
#    |         | 
#    |         | 
#    |         | 
#  (1)      36'
#    |         | 
#    |         | 
#    |         | 
#  =1=    ----  -------->X
#


import os
import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
from solution import predefined_solutions
from scipy.interpolate import interp1d
from scipy.integrate import trapz
from misc_utils import log_messages as lmsg

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
#print("pth= ", pth)
accelFilePath= pth+'/../../aux/A10000.py'
openSeesResultsPath= pth+'/../../aux/test_time_history_01_opensees_results.py'

# *** PROBLEM
FEcase= xc.FEProblem()
prep=FEcase.getPreprocessor
nodes= prep.getNodeHandler
elements= prep.getElementHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes) #defines dimension of
                  #the space: nodes by three coordinates (x,y) and 
                  #three DOF for each node (Ux,Uy,Theta)

## *** MESH ***                  
### *** GEOMETRY ***
n1= nodes.newNodeXY(0.0,0.0)
n2= nodes.newNodeXY(0.0,432.0)

### Single point constraints -- Boundary Conditions
constraints= prep.getBoundaryCondHandler
modelSpace.fixNode000(n1.tag)

### nodal masses:
n2.mass= xc.Matrix([[5.18,0,0],[0,0,0],[0,0,0]])  # node mass matrix.

### Define materials.
beamSection= typical_materials.defElasticSection2d(prep, "beamSection",3600,1080000,3225)

### Define ELEMENTS 
# define geometric transformation: performs a linear geometric transformation
# of beam stiffness and resisting force from the basic system to the
# global-coordinate system
lin= modelSpace.newLinearCrdTransf("lin")
elements= prep.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= beamSection.name
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))

## *** Loads ***
### define GRAVITY -------------------------------------------------------------
loadPatterns= prep.getLoadHandler.getLoadPatterns
gravityTS= loadPatterns.newTimeSeries("constant_ts","ts")
loadPatterns.currentTimeSeries= "ts"
lp0= loadPatterns.newLoadPattern("default","0")
# by the way we check that the loads cummulate
lp0.newNodalLoad(n2.tag,xc.Vector([0.0,-2000,0]))
loadPatterns.addToDomain(lp0.getName()) # Append load pattern to domain.

## Static analysis.
solProc= predefined_solutions.SimpleStaticLinear(FEcase)
if(solProc.solve()!=0):
    lmsg.error('Failed to solve for: '+lp0.name)
    quit()

prep.getDomain.setLoadConstant()
prep.getDomain.setTime(0.0)

### Define RECORDERS
dFree= []
recDFree= prep.getDomain.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([n2.tag]))
recDFree.callbackRecord= "dFree.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
rBase= []
recRBase= prep.getDomain.newRecorder("node_prop_recorder",None)
recRBase.setNodes(xc.ID([n1.tag]))
recRBase.callbackRecord= "rBase.append([self.getDomain.getTimeTracker.getCurrentTime,self.getReaction])"
recRBase.callbackSetup= "self.getDomain.calculateNodalReactions(True,1e-4)"
#Drift recorder not implemented yet. It can be emulated though.

### Eigen analysis.
solProc.clear()
solProc= predefined_solutions.FrequencyAnalysis(FEcase,systemPrefix='full_gen')
if(solProc.solve()!=0):
    lmsg.error('Eigen analysis failed.')
    quit()
eig1= solProc.analysis.getEigenvalue(1)
freq= math.sqrt(eig1)

dampRatio= 0.02
rayleigh= xc.RayleighDampingFactors(0.0,0.0,0.0,2.0*dampRatio/freq)
# print('damping factors: ', rayleigh)
prep.getDomain.setRayleighDampingFactors(rayleigh)

## DYNAMIC ground-motion analysis -------------------------------------------------------------
### create load pattern
G= 386.0
gm= loadPatterns.newLoadPattern("uniform_excitation","gm")
gm.dof= 0 #translation along the global X axis
mr= gm.motionRecord
hist= mr.history
hist.accel= loadPatterns.newTimeSeries("path_ts","accel")
hist.accel.setFactor(G)
exec(open(accelFilePath).read()) #define acceleration vector from file
hist.accel.setTimeIncr(0.005) #define time step of the acceleration data.
duration= mr.getDuration()
#print('duration: ', mr.getDuration())
#print('factor: ', hist.accel.getFactor(0.5))
#print('accel(0.5): ', mr.getAccel(0.5))
loadPatterns.addToDomain(gm.getName()) # Append load pattern to domain.

### Dynamic analysis.
solProc.clear()
dT= 0.01
numberOfSteps= int(duration/dT)
solProc= predefined_solutions.PlainLinearNewmark(FEcase, numSteps= numberOfSteps, timeStep= dT)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

ratio0= (eig1-25.02037)/25.02037
ratio1= (freq-5.002)/5.002
exec(open(openSeesResultsPath).read())

t= list()
Rx= list()
diffRx= list()
Rz= list()
diffRz= list()
ratioT= 0.0
for readed,computed in zip(openSeesRBase,rBase):
    t.append(computed[0])
    Rx.append(readed[1][0])
    diffRx.append(readed[1][0]-computed[1][0])
    Rz.append(readed[1][2])
    diffRz.append(readed[1][2]-computed[1][2])
    diffT= readed[0]-computed[0]
    ratioT+= diffT**2
    

RxArea= trapz(Rx, x=t)
errRx= trapz(diffRx, x=t)
RzArea= trapz(Rz, x=t)
errRz= trapz(diffRz, x=t)
# print('RxArea= ', RxArea, 'errRx= ',errRx)
# print('RzArea= ', RzArea,'errRz= ',errRz)

ratioT= math.sqrt(ratioT)
ratioRx= math.sqrt(errRx)/abs(RxArea)
ratioRz= math.sqrt(abs(errRz))/abs(RzArea)

'''
print('*** eig1: ', eig1)
print('*** ratio0: ', ratio0)
print('*** frequency: ', freq)
print( '*** ratio1: ', ratio1)
print( '*** ratioT: ', ratioT)
print( '*** ratioRx: ', ratioRx)
print( '*** ratioRz: ', ratioRz)
'''


t= list()
computedRx= list()
readedRx= list()
computedRz= list()
readedRz= list()
for readed,computed in zip(openSeesRBase,rBase):
    t.append(computed[0])
    computedRx.append(computed[1][0])
    readedRx.append(readed[1][0])
    computedRz.append(computed[1][2])
    readedRz.append(readed[1][2])
    
## Display results
# import matplotlib.pyplot as plt
# plt.plot(t, computedRz, 'ro', t, readedRz,'bs')
# plt.show()

#print( "Done!")
# output= open("./DFree.out",'w')
# for d in dFree:
#     output.write(str(d[0])+', '+str(d[1])+'\n')
# output.close()
# output= open("./RBase.out",'w')
# for r in rBase:
#     output.write(str(r[0])+', '+str(r[1])+'\n')
# output.close()

fname= os.path.basename(__file__)
if((abs(ratio0)<1e-8) and (ratio1<1e-5) and (ratioT<1e-10) and (ratioRx<0.08) and (ratioRz<0.01)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')


