# -*- coding: utf-8 -*-
# home made test

fy= 2600 # Yield stress of the steel.
E= 2.1e6 # Young modulus of the steel.
l= 1 # Distance between nodes
epsy= fy/E # Yield strain
D= 1.5*epsy # Prescribed displacement.
F= 1.05*E*epsy # Force to apply.
Nsteps= 10 # number of analysis steps.

import math
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Points of the stress-strain function.
x_modelo= [0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016,0.0014,0.0012,0.001,0.0008,0.0006,0.0004,0.0002,8.13152e-20,-0.0002,-0.0004,-0.0006,-0.0008,-0.001,-0.0012,-0.0014,-0.0016,-0.0014,-0.0012,-0.001,-0.0008,-0.0006,-0.0004,-0.0002,-8.13152e-20,0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016]
y_modelo= [420,840,1260,1680,2100,2520,2600.34,2600.76,2180.76,1760.76,1340.76,920.76,500.76,80.76,-339.24,-759.24,-1179.24,-1599.24,-2019.24,-2439.24,-2599.5,-2599.92,-2600.34,-2600.76,-2180.76,-1760.76,-1340.76,-920.76,-500.76,-80.76,339.24,759.24,1179.24,1599.24,2019.24,2439.24,2599.5,2599.92,2600.34,2600.76]


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)

# Materials definition
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= "steel"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for the next element.
spring= elements.newElement("Spring",xc.ID([1,2]));

# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,1,0.0) # Node 2

# Loads definition
cargas= preprocessor.getLoadHandler
casos= cargas.getLoadPatterns
#time series for the load pattern:
ts= casos.newTimeSeries("trig_ts","ts")
ts.factor= 1
ts.tStart= 0
ts.tFinish= 2
ts.period= 1
ts.shift= 0
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([F,0]))

#We add the load case to domain.
casos.addToDomain("0")

x= []
y= []
recorder= feProblem.getDomain.newRecorder("element_prop_recorder",None);
recorder.setElements(xc.ID([1]))
recorder.callbackRecord= "x.append(self.getMaterial().getStrain()); y.append(self.getN())"
recorder.callbackRestart= "print \"Restart method called.\""

''' 
\prop_recorder

nodes= preprocessor.getNodeHandler{2}
            \callback_record

                
d= .getDisp[0]
print (d*1000)

\callback_restart{print("Restart method called."}



'''
# Solution procedure
solu= feProblem.getSoluProc
solCtrl= solu.getSoluControl
solModels= solCtrl.getModelWrapperContainer
sm= solModels.newModelWrapper("sm")
numberer= sm.newNumberer("default_numberer")
numberer.useAlgorithm("rcm")
cHandler= sm.newConstraintHandler("transformation_constraint_handler")
analysisAggregations= solCtrl.getAnalysisAggregationContainer
analysisAggregation= analysisAggregations.newAnalysisAggregation("ldctrl","sm")
solAlgo= analysisAggregation.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= analysisAggregation.newConvergenceTest("energy_inc_conv_test")
ctest.tol= 1e-9
ctest.maxNumIter= 10
ctest.printFlag= 0
integ= analysisAggregation.newIntegrator("displacement_control_integrator",xc.Vector([]))
integ.nod= 2
integ.dof= 0
integ.dU1= 0.0002
soe= analysisAggregation.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","ldctrl","")
result= analysis.analyze(8)

integ.dU1= -0.0002 #Unload
result= analysis.analyze(16)

integ.dU1= 0.0002 #Reload
result= analysis.analyze(16)

nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeHandler
nod2= nodes.getNode(2)
deltax= nod2.getDisp[0] 
deltay= nod2.getDisp[1] 
nod1= nodes.getNode(1)
R= nod1.getReaction[0] 

elements= preprocessor.getElementHandler

elem1= elements.getElement(1)
elem1.getResistingForce()
Ax= elem1.getMaterial().getStrain() # Spring elongation



ratio1= abs((F+R)/F)
ratio2= abs((deltax-Ax)/Ax)
#diff= ley-ley_modelo
diff_x= []
diff_y= []
def substract(x,y): return x-y
diff_x= map(substract,x,x_modelo)
diff_y= map(substract,y,y_modelo)

ratio3= 0
for d in diff_x:
  ratio3= ratio3+d**2
ratio3= math.sqrt(ratio3)
ratio4= 0
for d in diff_y:
  ratio4= ratio4+d**2
ratio4= math.sqrt(ratio4)

''' 
print "R= ",R
print "dx= ",deltax
print "dy= ",deltay
print "Ax= ",Ax
print "ratio1= ",(ratio1)
print "ratio2= ",(ratio2)
print "x= ",x
print "diff_x= ",diff_x
print "ratio3= ",ratio3
print "y= ",y
print "y_modelo= ",y_modelo
print "diff_y= ",diff_y
print "ratio4= ",ratio4
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio3<1e-12) & (ratio4<1e-11)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
