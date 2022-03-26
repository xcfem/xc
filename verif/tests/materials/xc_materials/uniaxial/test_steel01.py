# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function

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
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Points of the stress-strain function.
x_reference= [0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016,0.0014,0.0012,0.001,0.0008,0.0006,0.0004,0.0002,8.13152e-20,-0.0002,-0.0004,-0.0006,-0.0008,-0.001,-0.0012,-0.0014,-0.0016,-0.0014,-0.0012,-0.001,-0.0008,-0.0006,-0.0004,-0.0002,-8.13152e-20,0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016]
y_reference= [420,840,1260,1680,2100,2520,2600.34,2600.76,2180.76,1760.76,1340.76,920.76,500.76,80.76,-339.24,-759.24,-1179.24,-1599.24,-2019.24,-2439.24,-2599.5,-2599.92,-2600.34,-2600.76,-2180.76,-1760.76,-1340.76,-920.76,-500.76,-80.76,339.24,759.24,1179.24,1599.24,2019.24,2439.24,2599.5,2599.92,2600.34,2600.76]


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)

n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(l,0.0)

# Materials definition
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)
    
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= steel.name
elements.dimElem= 2 # Dimension of element space
spring= elements.newElement("Spring",xc.ID([n1.tag, n2.tag]))

# Constraints
constraints= preprocessor.getBoundaryCondHandler
spc= constraints.newSPConstraint(n1.tag, 0,0.0) # Node 1
spc= constraints.newSPConstraint(n1.tag, 1,0.0)
spc= constraints.newSPConstraint(n2.tag, 1,0.0) # Node 2

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
#time series for the load pattern:
ts= lPatterns.newTimeSeries("trig_ts","ts")
ts.factor= 1
ts.tStart= 0
ts.tFinish= 2
ts.period= 1
ts.shift= 0
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(n2.tag, xc.Vector([F,0]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)

x= list()
y= list()
recorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
recorder.setElements(xc.ID([spring.tag]))
recorder.callbackRecord= "x.append(self.getMaterial().getStrain()); y.append(self.getN())"
recorder.callbackRestart= "print(\"Restart method called.\")"

''' 
\prop_recorder

nodes= preprocessor.getNodeHandler{2}
            \callback_record

                
d= .getDisp[0]
print(d*1000)

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
solutionStrategies= solCtrl.getSolutionStrategyContainer
solutionStrategy= solutionStrategies.newSolutionStrategy("ldctrl","sm")
solAlgo= solutionStrategy.newSolutionAlgorithm("newton_raphson_soln_algo")
ctest= solutionStrategy.newConvergenceTest("energy_incr_conv_test")
ctest.tol= 1e-9
ctest.maxNumIter= 10
ctest.printFlag= 0
integ= solutionStrategy.newIntegrator("displacement_control_integrator",xc.Vector([]))
integ.nod= n2.tag
integ.dof= 0
integ.dU1= 0.0002
soe= solutionStrategy.newSystemOfEqn("band_spd_lin_soe")
solver= soe.newSolver("band_spd_lin_lapack_solver")
analysis= solu.newAnalysis("static_analysis","ldctrl","")
result= analysis.analyze(8)

integ.dU1= -0.0002 #Unload
result= analysis.analyze(16)

integ.dU1= 0.0002 #Reload
result= analysis.analyze(16)

nodes.calculateNodalReactions(True,1e-7)
deltax= n2.getDisp[0] 
deltay= n2.getDisp[1] 
R= n1.getReaction[0] 

elements= preprocessor.getElementHandler

spring.getResistingForce()
Ax= spring.getMaterial().getStrain() # Spring elongation



ratio1= abs((F+R)/F)
ratio2= abs((deltax-Ax)/Ax)
#diff= law - reference_law
diff_x= []
diff_y= []
def subtract(x,y): return x-y
diff_x= map(subtract,x,x_reference)
diff_y= map(subtract,y,y_reference)

ratio3= 0
for d in diff_x:
  ratio3= ratio3+d**2
ratio3= math.sqrt(ratio3)
ratio4= 0
for d in diff_y:
  ratio4= ratio4+d**2
ratio4= math.sqrt(ratio4)

''' 
print("R= ",R)
print("dx= ",deltax)
print("dy= ",deltay)
print("Ax= ",Ax)
print("ratio1= ",(ratio1))
print("ratio2= ",(ratio2))
print("x= ",x)
print("diff_x= ",diff_x)
print("ratio3= ",ratio3)
print("y= ",y)
print("y_reference= ",y_reference)
print("diff_y= ",diff_y)
print("ratio4= ",ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio3<1e-12) & (ratio4<1e-11)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
