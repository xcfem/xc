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
E= shape.steelType.E
I= shape.Iz()
scc= shape.defElasticShearSection2d(preprocessor, "scc")

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
modelSpace.fixNode000(nA.tag)
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
for n in ln.nodes:
    nodeList.append((n.tag, n.getInitialPos3d.x))
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
cDisp= []
recDFree= preprocessor.getDomain.newRecorder("node_prop_recorder",None)
recDFree.setNodes(xc.ID([nC.tag]))
recDFree.callbackRecord= "cDisp.append([self.getDomain.getTimeTracker.getCurrentTime,self.getDisp])"

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

refDzi= [-1.7054796450066278e-05, -5.471910432699628e-05, -7.565949898397003e-05, -8.985798791517609e-05, -0.00012548317673915556, -0.00015143158392711325, -0.00016317359190020134, -0.00019610558260968973, -0.00022655741289145037, -0.00023716893717976636, -0.0002920439237507381, -0.0003788028350036642, -0.0004220097797808747, -0.00046714942423985926, -0.000554823396828664, -0.0006065498804174254, -0.00064412297963812, -0.0007295048815519471, -0.0007901469161087688, -0.0008235764650331952, -0.0009135958845231068, -0.0010058392881257617, -0.0010503672809299193, -0.0011307315706068862, -0.0012304490075866097, -0.0012786337615588003, -0.0013486976984119251, -0.0014529854486475726, -0.0015075524974522867, -0.001568613336598597, -0.0016690377862798892, -0.001725103474773294, -0.0017717625874633194, -0.0018717431053754522, -0.0019376263092116964, -0.0019778898308683264, -0.0020725879476980466, -0.0021492375169370206, -0.0021863762956051405, -0.0022723888360179703, -0.0023403542699648454, -0.002339085309298258, -0.0023928298447327694, -0.002470503700474237, -0.0024758278738760157, -0.0025130320955878134, -0.002598986167501425, -0.0026125784315233916, -0.002636452712191358, -0.0027234428696933047, -0.002718938391929791, -0.002671220968043569, -0.002718499598673364, -0.0027322779228901076, -0.002679053815763032, -0.002711199001235227, -0.002743965125423166, -0.0026892300602399744, -0.0027050794196581537, -0.00275100872650887, -0.0026769585567945043, -0.0026169157729241904, -0.002639176468974512, -0.0025827055285338016, -0.002505428524726229, -0.0025248852548575107, -0.0024866864191633357, -0.002398813402418796, -0.002407498996520201, -0.002389058806230131, -0.0022784183348528255, -0.0022252570754703743, -0.0022013905056095804, -0.0020965626895547776, -0.0020266320714817368, -0.0020106093838821054, -0.0019156398437642386, -0.001830956562097689, -0.00181571951380162, -0.001736010247291802, -0.0016265854557252783, -0.0015757682062046044, -0.0014962198984589188, -0.0013823211663571242, -0.0013228824671156693, -0.0012561808005152086, -0.001139025885279584, -0.001070988558764282, -0.001012840641826728, -0.000898839666649327, -0.0008135111584549237, -0.0007462238659846255, -0.0006297348089166454, -0.0005359363786480947, -0.00047134620747174235, -0.00036295677815302536, -0.0002580235352198032, -0.00019623402083861002, -9.485982250059825e-05, 1.6898288557602373e-05]

error= 0.0
for expected, computed in zip(refDzi, Dzi):
    error+= (expected-computed)**2
error= math.sqrt(error)

''' 
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
