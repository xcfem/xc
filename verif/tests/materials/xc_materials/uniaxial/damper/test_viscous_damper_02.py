# -*- coding: utf-8 -*-
'''Test of ViscousDamper class.

Inspired on the example from Michal H. Scott:
https://openseesdigital.com/2025/01/12/minimal-damper-example/
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import math
import json
import xc
from model import predefined_spaces
from materials import typical_materials
from materials import damper_materials
from actions.quake import ground_motion_utils as gmu
from solution import predefined_solutions

silent= True # If false, display results.

# Frame dimensions
L= 5000
h= 3000

# Seismic mass
W= 1000
g= 9810
m= W/g

# Natural period
Tn= 0.7
pi= 3.14159
wn= 2*pi/Tn
K= m*wn**2

# Structural damping
zeta= 0.02
C= 2*m*wn*zeta

# Damper properties
Kd= 25 # Elastic stiffness of linear spring to model the axial flexibility of a viscous damper (e.g. combined stiffness of the supporting brace and internal damper portion) 
Cd= 20.7452 # Damping coefficient.
ad= 0.35 # Velocity exponent.

# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes)

n1= modelSpace.newNode(0.0)
n2= modelSpace.newNode(0.0)
n2.mass= xc.Matrix([[m]])

# Constraints.
modelSpace.fixNode('0', n1.tag)

elastic= typical_materials.defElasticMaterial(preprocessor, name= "elastic", E= K, eta= C)
modelSpace.setDefaultMaterial(elastic)
modelSpace.setElementDimension(1)
zl1= modelSpace.newElement("ZeroLength", [n1.tag,n2.tag])

viscousDamper= damper_materials.def_viscous_damper(modelSpace, name= 'viscousDamper', k= Kd, c= Cd, alpha= ad)
modelSpace.setDefaultMaterial(viscousDamper)
damper= modelSpace.newElement("ZeroLength", [n1.tag,n2.tag])
xVector= xc.Vector([L, h, 0.0]).Normalized()
yVector= xc.Vector([-xVector[1], xVector[0], 0.0])
damper.setupVectors(xVector, yVector)

# Ground motion load.
## Read the excitation data.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
accelFilePath= pth+'/../../../../aux/load_patterns/ground_motions/TakY.th'

gm, gmSz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "gm", dof= 0, inputFileName= accelFilePath, dt= .01, cod_ts= 'accel', factor= 0.5*g, vel0= 0.0)
modelSpace.addLoadCaseToDomain(gm.name)

# Define recorders.
## Record n2 node displacements, velocities and accelerations.
domain= modelSpace.getDomain()
n2Disp2= list()
recN2Disp2= domain.newRecorder("node_prop_recorder", None)
recN2Disp2.setNodes(xc.ID([n2.tag]))
recN2Disp2.callbackRecord= "n2Disp2.append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp))"
n2Vel2= list()
recN2Vel2= domain.newRecorder("node_prop_recorder", None)
recN2Vel2.setNodes(xc.ID([n2.tag]))
recN2Vel2.callbackRecord= "n2Vel2.append((self.getDomain.getTimeTracker.getCurrentTime,self.getVel))"
n2Accel2= list()
recN2Accel2= domain.newRecorder("node_prop_recorder", None)
recN2Accel2.setNodes(xc.ID([n2.tag]))
recN2Accel2.callbackRecord= "n2Accel2.append((self.getDomain.getTimeTracker.getCurrentTime,self.getAccel))"
## Record forces at the damper.
ti= list()
damperForces= list()
damperDeformations= list()
damperRecorder2= domain.newRecorder("element_prop_recorder",None)
damperRecorder2.setElements(xc.ID([damper.tag]))
callbackRecord= '''
time= self.getDomain.getTimeTracker.getCurrentTime
ti.append(time)
damperMaterial= self.getMaterials()[0]
damperStrain= damperMaterial.getStrain()
damperStress= damperMaterial.getStress()
damperForces.append(damperStress)
damperDeformations.append(damperStrain)
'''
damperRecorder2.callbackRecord= callbackRecord


# Perform the dynamic analysis.
numberOfSteps= gmSz
transientSolProc= predefined_solutions.PlainNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= .01, gamma= 0.5, beta= 0.25, maxNumIter= 25, convergenceTestTol= 1e-12, printFlag= 0)
# transientSolProc= predefined_solutions.PlainLinearNewmark(feProblem, numSteps= numberOfSteps, timeStep= .01, gamma= 0.5, beta= 0.25, maxNumIter= 25, convergenceTestTol= 1e-12, printFlag= 0)
# transientSolProc= predefined_solutions.PlainNewmarkKrylovNewton(feProblem, numSteps= numberOfSteps, timeStep= .01, gamma= 0.5, beta= 0.25, maxNumIter= 25, convergenceTestTol= 1e-12, printFlag= 0, convTestType= 'energy_incr_conv_test', soeType= 'umfpack_gen_lin_soe', solverType= 'umfpack_gen_lin_solver')
if(transientSolProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()
    
results= {'damperDeformations':damperDeformations, 'damperForces':damperForces}

refFilePath= pth+"/../../../../aux/reference_files/"
fname= os.path.basename(__file__)
jsonFileName= refFilePath+'/ref_'+fname.replace('.py', '.json')

# # Save output as reference.
# with open(jsonFileName, 'w') as f:
#     json.dump(results, f)

# Check results.
## Load reference values.
with open(jsonFileName, 'r') as f:
     ref_results= json.load(f)
error= 0.0
tol= 1e-8
for key in ref_results:
    ref_values= ref_results[key]
    values= results[key]
    error+= (len(values)-len(ref_values))**2
    if(error<tol):
        for v, v_ref in zip(values, ref_values):
          error+=(v-v_ref)**2
error= math.sqrt(error)

if(not silent):
    print('error= ', error)
    
from misc_utils import log_messages as lmsg
if error<tol:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

if(not silent):
    import matplotlib
    # matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    # Get node 2 motion.
    t, accel= zip(*n2Accel2)
    t, vel= zip(*n2Vel2)
    t, disp= zip(*n2Disp2)

    accel_dof= [a[0] for a in accel]
    vel_dof= [v[0] for v in vel]
    disp_dof= [d[0] for d in disp]
    fig, axes= plt.subplots(3, 1, figsize=(9, 9), sharex=True)
    axes[0].plot(t, accel_dof, lw=0.7, color="tab:blue")
    axes[0].set_ylabel("Acceleration")
    axes[0].set_title("Node 2 motion.")
    axes[1].plot(t, vel_dof, lw=0.7, color="tab:orange")
    axes[1].set_ylabel("Velocity")
    axes[2].plot(t, disp_dof, lw=0.7, color="tab:green")
    axes[2].set_ylabel("Displacement")
    axes[2].set_xlabel("Time (s)")
    fig.tight_layout()
    plt.show()
    # Get damper results.
    t= ti
    plt.figure(figsize=(15,3))
    plt.plot(damperDeformations, damperForces)
    plt.title('Damper force')
    plt.ylabel('Damper force (kN)')
    plt.xlabel('Elongation (mm)')
    plt.grid()
    plt.yticks(fontsize= 14)
    plt.xticks(fontsize= 14)
    # plt.axis('equal')
    #plt.xlim([0.0, values[-1]]);
    plt.show()
