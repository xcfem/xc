# -*- coding: utf-8 -*-
from __future__ import print_function
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials

fc= -250 # Concrete compressive strength.
l= 1 # Distance between nodes
epsc0= -2e-3 # Maximum compressive strain for section in compression.
fcu= fc/1.2 # Strain under concrete compressive stress.
epsU= -3.5e-3 # Maximum compressive strain in concrete.
F= 1e2 # Force to apply.

# Points of the stress-strain function
x_modelo= [-0.00012, -0.00024, -0.00036, -0.00048, -0.0006, -0.00072, -0.00084, -0.00096, -0.00108, -0.0012, -0.00132, -0.00144, -0.00156, -0.00168, -0.0018, -0.00192, -0.00204, -0.00216, -0.00228, -0.0024, -0.00252, -0.00264, -0.00276, -0.00288, -0.003, -0.00288, -0.00276, -0.00264, -0.00252, -0.0024, -0.00228, -0.00216, -0.00204, -0.00192, -0.0018, -0.00168, -0.00156, -0.00144, -0.00132, -0.0012, -0.00108, -0.0012, -0.00132, -0.00144, -0.00156, -0.00168, -0.0018, -0.00192, -0.00204, -0.00216, -0.00228, -0.0024, -0.00252, -0.00264, -0.00276, -0.00288, -0.003, -0.00312, -0.00324, -0.00336, -0.00348, -0.00336, -0.00324, -0.00312, -0.003, -0.00288, -0.00276, -0.00264, -0.00252, -0.0024, -0.00228, -0.00216, -0.00204, -0.00192, -0.0018, -0.00168, -0.00156, -0.00144]

y_modelo= [-29.1, -56.4, -81.9, -105.6, -127.5, -147.6, -165.9, -182.4, -197.1, -210, -221.1, -230.4, -237.9, -243.6, -247.5, -249.6, -248.888888889, -245.555555556, -242.222222222, -238.888888889, -235.555555556, -232.222222222, -228.888888889, -225.555555556, -222.222222222, -208.599404002, -194.976585781, -181.353767561, -167.73094934, -154.10813112, -140.485312899, -126.862494679, -113.239676458, -99.6168582375, -85.994040017, -72.3712217965, -58.748403576, -45.1255853555, -31.502767135, -17.8799489144, -4.25713069391, -17.8799489144, -31.502767135, -45.1255853555, -58.748403576, -72.3712217965, -85.994040017, -99.6168582375, -113.239676458, -126.862494679, -140.485312899, -154.10813112, -167.73094934, -181.353767561, -194.976585781, -208.599404002, -222.222222222, -218.888888889, -215.555555556, -212.222222222, -208.888888889, -197.227782957, -185.566677025, -173.905571093, -162.244465162, -150.58335923, -138.922253298, -127.261147366, -115.600041434, -103.938935502, -92.2778295705, -80.6167236386, -68.9556177068, -57.2945117749, -45.6334058431, -33.9722999112, -22.3111939794, -10.6500880476]

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
horm= typical_materials.defConcrete01(preprocessor, "horm",epsc0,fc,fcu,epsU)

''' 
print("fpc= ",fpc)
print("epsc0= ",epsc0)
print("fpcu= ",fpcu)
print("epscu= ",epscu)
print("CminStrain= ",CminStrain)
print("CendStrain= ",CendStrain)
print("Cstrain= ",Cstrain)
print("CStress= ",Cstress)
print("Ctangent= ",Ctangent)
print("CunloadSlope= ",CunloadSlope)
print("TminStrain= ",TminStrain)
print("TendStrain= ",TendStrain)
print("Tstrain= ",Tstrain)
print("TStress= ",Tstress)
print("Ttangent= ",Ttangent)
print("TunloadSlope= ",TunloadSlope)
 '''


''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementHandler
elements.defaultMaterial= horm.name
elements.dimElem= 2 # Dimension of element space
#  sintaxis: Spring[<tag>] 
spring= elements.newElement("Spring",xc.ID([1,2]))
    
# Constraints
constraints= preprocessor.getBoundaryCondHandler
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,1,0.0) # Node 2

# Loads definition
loadHandler= preprocessor.getLoadHandler
lPatterns= loadHandler.getLoadPatterns
ts= lPatterns.newTimeSeries("trig_ts","ts")
ts.factor= 1
ts.tStart= 0
ts.tFinish= 2
ts.period= 1
ts.shift= 0
lPatterns.currentTimeSeries= "ts"
#Load case definition
lp0= lPatterns.newLoadPattern("default","0")
lp0.newNodalLoad(2,xc.Vector([-F,0]))

#We add the load case to domain.
lPatterns.addToDomain(lp0.name)
x= []
y= []
recorder= feProblem.getDomain.newRecorder("element_prop_recorder",None)
recorder.setElements(xc.ID([0]))
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
ctest= solutionStrategy.newConvergenceTest("energy_inc_conv_test")
ctest.tol= 1e-9
ctest.maxNumIter= 10 # Convergence Test: maximum number of iterations that will be performed before "failure to converge" is returned
ctest.printFlag= 0 # Convergence Test: flag used to print(information on convergence (optional))
                   # 1: print(information on each= step)
integ= solutionStrategy.newIntegrator("displacement_control_integrator",xc.Vector([]))
integ.nod= 2
integ.dof= 0
integ.dU1= -0.00012
soe= solutionStrategy.newSystemOfEqn("sparse_gen_col_lin_soe")
solver= soe.newSolver("super_lu_solver")
analysis= solu.newAnalysis("static_analysis","ldctrl","")
result= analysis.analyze(25)

integ.dU1= 0.00012 #Unload
result= analysis.analyze(16)

integ.dU1= -0.00012 #Reload
result= analysis.analyze(20)

integ.dU1= 0.00012 #Unload
result= analysis.analyze(17)

#diff= ley-ley_modelo
diff_x= []
diff_y= []
def subtract(x,y): return x-y
diff_x= map(subtract,x,x_modelo)
diff_y= map(subtract,y,y_modelo)

ratio1= 0
for d in diff_x:
  ratio1= ratio1+d**2
ratio3= math.sqrt(ratio1)
ratio2= 0
for d in diff_y:
  ratio2= ratio2+d**2
ratio4= math.sqrt(ratio2)

#print("x= ",x)
#print("diff_x= ",diff_x)
#print("ratio3= ",ratio3)
#print("y= ",y)
#print("y_modelo= ",y_modelo)
#print("diff_y= ",diff_y)
#print("ratio4= ",ratio4)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-17) & (ratio2<1e-8)):
  print('test '+fname+': ok.')
else:
  lmsg.error(fname+' ERROR.')
