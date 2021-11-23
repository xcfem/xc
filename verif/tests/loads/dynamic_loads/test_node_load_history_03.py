# -*- coding: utf-8 -*-
'''Example from Stimac-Grandic, Ivana. (2015). Serviceability verification of pedestrian bridges under pedestrian loading. Tehnicki vjesnik - Technical Gazette. 22. 527-537. 10.17559/TV-20131030105641.'''


from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2021, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from misc_utils import log_messages as lmsg
from actions import loads
from rough_calculations import ng_fixed_fixed_beam as sb


gravity= 9.81 # m/s2

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
span= 23.0
pt1= modelSpace.newKPoint(0,0)
pt2= modelSpace.newKPoint(span,0)

ln= modelSpace.newLine(pt1, pt2)
ln.nDiv= 50

# Material
beamDepth= 0.4
beamWidth= 4
beamArea= beamDepth*beamWidth
beamE= 1.05*3.2e10 # Elastic modulus (dynamic value).
beamIz= 0.213 # Modulus of inertia (weak axis).
beamIy= 2.13 # Modulus of inertia (strong axis).
dampingRatio= 1.3/100 
sectionProperties= xc.CrossSectionProperties2d()
sectionProperties.A= beamArea;
sectionProperties.E= beamE
sectionProperties.I= beamIz
sectionProperties.linearRho= 4160.0
section= typical_materials.defElasticSectionFromMechProp2d(preprocessor, "section",sectionProperties)


# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 2 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= section.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

ln.genMesh(xc.meshDir.I)

nA= pt1.getNode()
nC= ln.getNearestNode(geom.Pos3d(span/2.0,0.0,0.0))
nB= pt2.getNode()

# Constraints
modelSpace.fixNode000(nA.tag)
modelSpace.fixNode000(nB.tag)

### Eigen analysis.
sBeam= sb.FixedFixedBeam(E= beamE, I= beamIz, l= span)
beam_fv= sBeam.getFundamentalBendingFrequencies(1, linearRho= sectionProperties.linearRho)[0]

## Load definition.
### Load value.
A= 180 # amplitude of the pulsating force.
tStart= 0.0
v= 0.9*beam_fv
tEnd= 3*span/v
duration= tEnd-tStart
dT= 1/(25*beam_fv) # time step.
numSteps= int(duration/dT)+1
period= 1/beam_fv
beam_w= 2.0*math.pi*beam_fv

## Damping
rayleigh= xc.RayleighDampingFactors(0.0,0.0,0.0,2.0*dampingRatio/beam_w)
# print('damping factors: ', rayleigh)
# print('current damping ratio: ', rayleigh.getCommitedDampingRatio(beam_w))
preprocessor.getDomain.setRayleighDampingFactors(rayleigh)


#### Create time series and load pattern.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
## Definition of time series:
pulse= lPatterns.newTimeSeries("trig_ts","ts")
pulse.factor= 1.0
pulse.Start= tStart
pulse.tFinish= tEnd
pulse.period= period
pulse.shift= 0.0
lp= lPatterns.newLoadPattern("default","pulseLoad")
lp.newNodalLoad(nC.tag, xc.Vector([0,-A,0]))

### Add load cases to domain.
modelSpace.addLoadCaseToDomain(lp.name)
    
# Define RECORDERS
cDisp= list()
recDisp= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID([nC.tag]))
recDisp.callbackRecord= "cDisp.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
cAccel= list()
recAccel= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID([nC.tag]))
recAccel.callbackRecord= "cAccel.append([self.getDomain.getTimeTracker.getCurrentTime,self.getAccel])"

### Dynamic analysis.
preprocessor.getDomain.setTime(0.0)
solProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= numSteps, timeStep= dT)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Extract results.
ti= list()
Dzi= list()
Azi= list()
for r,a in zip(cDisp,cAccel):
    ti.append(r[0])
    Dzi.append(r[1][1])
    Azi.append(a[1][1])
    
# Get peak acceleration.
peakAccel= 0.0
timePeakAccel= -1.0
for row in cAccel:
    t= row[0]
    accel= row[1][1]
    if(accel>peakAccel):
        peakAccel= accel
        timePeakAccel= t
        
ratio1= abs(peakAccel-0.17)/0.17

''' 
print('span: ', span, 'm')
print('peak acceleration: ', peakAccel, 'm/s2')
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-3:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display results
# import matplotlib.pyplot as plt
# plt.plot(ti, Azi)
# plt.show()
