# -*- coding: utf-8 -*-
'''Beam under movable load.'''

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from scipy import interpolate
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials.ec3 import EC3_materials # Steel shapes.
from misc_utils import log_messages as lmsg
from actions import loads
from rough_calculations import simple_beam_oscillation as sbo
from rough_calculations import ng_simple_beam as sb

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)

# Problem geometry
span= 12
pt1= modelSpace.newKPoint(0,0)
pt2= modelSpace.newKPoint(span,0)

ln= modelSpace.newLine(pt1, pt2)
ln.nDiv= 10

# Material
steelType= EC3_materials.S235JR # steel type
shape= EC3_materials.IPEShape(steel= steelType, name= 'IPE_200')
scc= shape.defElasticShearSection2d(preprocessor, "scc")
linearRho= shape.getRho()
E= shape.steelType.E
I= shape.Iz()
sBeam= sb.SimpleBeam(E= E, I= I, l= span)
f0= sBeam.getFundamentalBendingFrequencies(1, linearRho)[0]
w0= 2*math.pi*f0

# Mesh generation

## Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")

## Seed element
seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.dimElem= 2 # Bars defined in a three dimensional space.
seedElemHandler.defaultMaterial= scc.name
seedElemHandler.defaultTransformation= lin.name
beam2d= seedElemHandler.newElement("ElasticBeam2d",xc.ID([0,0]))

ln.genMesh(xc.meshDir.I)

nA= pt1.getNode()
nC= ln.getNearestNode(geom.Pos3d(span/2,0.0,0.0))
nB= pt2.getNode()

# Constraints
modelSpace.fixNode00F(nA.tag)
modelSpace.fixNodeF0F(nB.tag)

## Load definition.
### Load value.
F= 700
v= 2.25*0.8 # load speed.
tIni= 0.0
tFin= span/v
incT= (tFin-tIni)/100 # time step.
### Compute node list.
nodeList= list()
nodeTagList= list()
for n in ln.nodes:
    nodeList.append((n.tag, n.getInitialPos3d.x))
    nodeTagList.append(n.tag)
nodeList.sort(key=lambda tup: tup[1])  # sorts in place
movableLoad= loads.MovableLoad(P= F, v= v, t0= tIni, nodes= nodeList)
ti, loadHistory= movableLoad.getNodeLoadHistory(tBegin= tIni, tEnd= tFin, step= incT)
dT= ti[1]-ti[0]
assert(dT==incT)
#### Create time series and load pattern.
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
timeSeries= dict()
loadPatterns= dict()
for nTag in loadHistory:
    tsName= 'ts'+str(nTag)
    tmp= lPatterns.newTimeSeries("path_ts",tsName)
    lh= loadHistory[nTag] # load history for the node.
    tmp.path= xc.Vector(lh)
    tmp.setTimeIncr(dT)
    timeSeries[nTag]= tmp
    lPatterns.currentTimeSeries= tsName
    lpName= 'lp'+str(nTag)
    lp= lPatterns.newLoadPattern("default",lpName)
    lp.newNodalLoad(nTag, xc.Vector([0,-1,0]))
    loadPatterns[lpName]= lp
    
### Add load cases to domain.
for lpName in loadPatterns:
    modelSpace.addLoadCaseToDomain(lpName)
    
# Define RECORDERS
cDisp= list()
recDisp= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID([nC.tag]))
recDisp.callbackRecord= "cDisp.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"
cAccel= list()
recAccel= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID(nodeTagList))
recAccel.callbackRecord= "cAccel.append([self.tag,self.getDomain.getTimeTracker.getCurrentTime,self.getAccel])"

### Dynamic analysis.
preprocessor.getDomain.setTime(0.0)
duration= ti[-1]-ti[0]
numberOfSteps= int(duration/dT)

solProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= numberOfSteps, timeStep= dT)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Extract results.
ti= list()
Dzi= list()
for r in cDisp:
    ti.append(r[0])
    Dzi.append(r[1][1])

refDzi= [-8.655765122871876e-05, -0.0003607940520973079, -0.0006447454649367808, -0.00074031694990428, -0.0008187089170482844, -0.0010825426729272763, -0.0013749756883145788, -0.0014807550798580657, -0.0015515210664503497, -0.0018046827198923547, -0.002098591261588511, -0.0021922073043804213, -0.00223093859696444, -0.0024672525364557037, -0.0027670871106816423, -0.0028743375891555653, -0.002905941870381363, -0.003129950929709538, -0.003434914339114823, -0.003555771092610673, -0.003570474302186269, -0.0037343504897542792, -0.00399524327109545, -0.004116273912944429, -0.004129132774533355, -0.004280500215102777, -0.004543537570018246, -0.004675871636414097, -0.00468878049594235, -0.0048269478556941135, -0.00507154176541252, -0.00514716020723557, -0.005090873492773404, -0.0051939909337512784, -0.005440397796474632, -0.005530415230592619, -0.005473595356427745, -0.0055620883989985, -0.00580812441269268, -0.0059133277999100274, -0.005826455522458052, -0.005813983156069621, -0.005968240685323751, -0.006055489979925377, -0.0059728603277990925, -0.005949494545875047, -0.006097792918293844, -0.006196793692710153, -0.006119642970418863, -0.006085694437583945, -0.006188715488409139, -0.006200007763534871, -0.006029148789330393, -0.005942556662184777, -0.006041656511232933, -0.006065206571044753, -0.005900521660236561, -0.005800360910373953, -0.0058941106151997325, -0.0059296887965083755, -0.005741730152838283, -0.005542141648350627, -0.0055392592554426665, -0.005553448949376697, -0.005375810870145386, -0.005168004179887042, -0.0051538824912553535, -0.005176640360470655, -0.005009433689565727, -0.004795052823884197, -0.00474869859405187, -0.004711325894659976, -0.004484714890865948, -0.0042427609294159105, -0.004185635163838352, -0.004157137113365512, -0.003940470691936755, -0.0036909437848487367, -0.00362316674308035, -0.0036015674104918813, -0.0033847465646008783, -0.003081301593216601, -0.00295425874413255, -0.0029247541306351398, -0.002721834603374649, -0.0024140167450741536, -0.002274201725187184, -0.002247005383154169, -0.002058442471192029, -0.0017475896328282778, -0.0015891175888455885, -0.0015395905068172112, -0.0013401783835396202, -0.0010222126880209737, -0.0008502547406626767, -0.0008026108828539827, -0.0006157047293014601, -0.0002976698317769209, -0.00011192375900609823, -6.496331054699388e-05]

error= 0.0
for expected, computed in zip(refDzi, Dzi):
    error+= (expected-computed)**2
error= math.sqrt(error)

# Get peak acceleration.
peakAccel= 0.0
tagNodePeakAccel= -1
timePeakAccel= -1.0
xNodePeakAccel= -1.0
for row in cAccel:
    tag= row[0]
    t= row[1]
    accel= row[2][1]
    x= preprocessor.getDomain.getMesh.getNode(tag).getInitialPos3d.x
    if(accel>peakAccel):
        peakAccel= accel
        tagNodePeakAccel= tag
        timePeakAccel= t
        xNodePeakAccel= x
psi= 0.0
peakAccelRefA= sbo.aceleracionCargaAislada(P= F, m= linearRho, L= span, w0= w0, psi= psi, V= v, x= xNodePeakAccel, t= timePeakAccel)
peakAccelRefB= sbo.aceleracionExtremaCargaAislada(P= F, m= linearRho, L= span, w0= w0, psi= psi, V= v, x= xNodePeakAccel, tIni= tIni, tFin= tFin)


''' 
print('span: ', span, 'm')
print('w0= ', w0, 'rad/s')
print('psi= ', psi*100.0, '%')
print('v= ', v, 'm/s')
print('x= ', x, 'm')
print('peak acceleration in node: ', tagNodePeakAccel, '(x= ', xNodePeakAccel, ') at time: ', timePeakAccel, ' s with value a_peak= ', peakAccel, 'm/s2')
print('peakAccelRefA= ', peakAccelRefA, 'm/s2')
print('peakAccelRefB= ', peakAccelRefB, 'm/s2')
print("error= ",error)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(error)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

# # Display results
# import matplotlib.pyplot as plt
# plt.plot(ti, Dzi)
# plt.show()
