# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

fy= 2600 # Yield stress of the steel.
E= 2.1e6 # Young modulus of the steel.
l= 1 # Distance between nodes
epsy= fy/E # Yield strain.
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

# Points of the stress-strain function
x_modelo= [0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016,0.0014,0.0012,0.001,0.0008,0.0006,0.0004,0.0002,8.13151629364e-20,-0.0002,-0.0004,-0.0006,-0.0008,-0.001,-0.0012,-0.0014,-0.0016,-0.0014,-0.0012,-0.001,-0.0008,-0.0006,-0.0004,-0.0002,3.7947076037e-19,0.0002,0.0004,0.0006,0.0008,0.001,0.0012,0.0014,0.0016]
y_modelo= [420,839.99999756,1259.9983975,1679.84023129,2094.43925811,2439.74485068,2575.02268013,2597.10357884,2177.10361083,1757.10718257,1337.16070607,917.509155896,498.955465729,83.4836121405,-324.877516243,-719.184353981,-1089.3087798,-1423.15134693,-1709.72230813,-1942.88083256,-2123.30032297,-2257.38132823,-2354.30837053,-2423.31773771,-2003.32887686,-1583.61049005,-1165.29416968,-750.941478428,-344.863824637,46.9169088244,417.211303572,758.719915171,1065.35896571,1333.3932764,1561.95005251,1752.78930425,1909.55078241,2036.84608339,2139.48812589,2221.98168325]


# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeLoader

# Problem type
modelSpace= predefined_spaces.SolidMechanics2D(nodes)


nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXY(0,0)
nod= nodes.newNodeXY(l,0.0)

# Materials definition
mat= typical_materials.defSteel02(preprocessor, "steel",E,fy,0.001,0.0)
  
''' We define nodes at the points where loads will be applied.
    We will not compute stresses so we can use an arbitrary
    cross section of unit area.'''
    
# Elements definition
elements= preprocessor.getElementLoader
elements.defaultMaterial= "steel"
elements.dimElem= 2 # Dimension of element space
elements.defaultTag= 1 #Tag for the next element.
spring= elements.newElement("Spring",xc.ID([1,2]));
    
# Constraints
constraints= preprocessor.getConstraintLoader
#
spc= constraints.newSPConstraint(1,0,0.0) # Node 1
spc= constraints.newSPConstraint(1,1,0.0)
spc= constraints.newSPConstraint(2,1,0.0) # Node 2

# Loads definition
cargas= preprocessor.getLoadLoader
casos= cargas.getLoadPatterns
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

nodes= preprocessor.getNodeLoader{2
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
ctest.maxNumIter= 10 # Convergence Test: maximum number of iterations that will be performed before "failure to converge" is returned
ctest.printFlag= 0 # Convergence Test: flag used to print information on convergence (optional)
                   # 1: print information on each= step
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


#resta= ley-ley_modelo
resta_x= []
resta_y= []
def substract(x,y): return x-y
resta_x= map(substract,x,x_modelo)
resta_y= map(substract,y,y_modelo)

ratio1= 0
for d in resta_x:
  ratio1= ratio1+d**2
ratio3= math.sqrt(ratio1)
ratio2= 0
for d in resta_y:
  ratio2= ratio2+d**2
ratio4= math.sqrt(ratio2)

#print "x= ",x
#print "resta_x= ",resta_x
#print "ratio3= ",ratio3
#print "y= ",y
#print "y_modelo= ",y_modelo
#print "resta_y= ",resta_y
#print "ratio4= ",ratio4


import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-17) & (ratio2<1e-7)):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
