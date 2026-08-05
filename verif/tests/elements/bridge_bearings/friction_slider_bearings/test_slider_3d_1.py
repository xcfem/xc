# -*- coding: utf-8 -*-
''' Dummy FlatSiderSimple2d adapted from TestSlider2d_0.tcl

Purpose: this file tests the 3D flatSliderBearing or the
singleFPBearing element. It models a rigid isolated mass
and the bearing element has finite length. It also tests the
different friction models.

Original Copyright:
# Written: Andreas Schellenberg (andreas.schellenberg@gmail.com)
# Created: 02/09
'''

import os
import json
import math
import xc
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from model import friction_models as fm
from materials import friction_bearings as fb
from actions.quake import ground_motion_utils as gmu
from misc_utils import log_messages as lmsg

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodeHandler)

silent= True

# 1. Define geometry for model
g= 32.174*12.0 # 12*ft/s^2= in/s^2
P= 18.0
mass= P/g
templateMatrix=  xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])

# 1.1. Define nodes (zero-height bearing at coordinates x=0, y=0)
n1= modelSpace.newNode(0,0,0)
n2= modelSpace.newNode(0,0,10.0)
n2.mass= mass*templateMatrix

# 2. Constraints.
modelSpace.fixNode('000_000', n1.tag)
modelSpace.fixNode('FFF_000', n2.tag)

# 3. Define materials.
mv= 1.0*mass
kv= 7500.0
zetaVertical= 0.02
cv= 2.0*zetaVertical*math.sqrt(kv*mv)
## Vertical response.
vertResp= typical_materials.defElasticMaterial(preprocessor, name= "vertResp", E= kv, eta= cv)
## Rotational response.
rotResp= typical_materials.defElasticMaterial(preprocessor, name= "rotResp", E= 0.0)

# 4. Define friction model (coefficient of friction = 0.163)
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= "frictionModel", mu= .163)

'''
# frictionModel VDependent tag muSlow muFast transRate
#frictionModel VDependent 1 0.085 0.163 0.77

# frictionModel VPDependent tag muSlow muFast0 A deltaMu alpha transRate
#frictionModel VPDependent 1 0.085 0.163 7.0686 0.05 0.08 0.77
'''
# 5. Define elements
bearing= fb.def_flat_slider_bearing_3d(modelSpace, n1= n1, n2= n2, frictionModel= frictionModel, vertResp= vertResp, rotRespX= rotResp, rotRespY= rotResp, rotRespZ= rotResp, kInit= 250.0, x= xc.Vector([1,0,0]))

'''
# element singleFPBearing eleTag NodeI NodeJ frnMdlTag Reff kInit -P matTag -T matTag -My matTag -Mz matTag <-orient <x1 x2 x3> y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m> <-iter maxIter tol>
#element singleFPBearing 1 1 2 1 34.68 250.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0

# element RJWatsonEqsBearing eleTag NodeI NodeJ frnMdlTag kInit k2 k3 mu -P matTag -Mz matTag <-orient x1 x2 x3 y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m> <-iter maxIter tol>
#element RJWatsonEqsBearing 1 1 2 1 250.0 0.519 0.0 3.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
'''

# 6. Define gravity loads
## Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
## Create nodal loads at node 2.
glp.newNodalLoad(n2.tag, xc.Vector([0,0,-P,0,0,0]))
modelSpace.addLoadCaseToDomain(glp.name)

# 7. Solution procedure.
numSteps= 10
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numSteps= numSteps, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
integrator= solProc.getIntegrator()
integrator.dLambda1= .1
analysis= solProc.getAnalysis()

# 8. Define recorders.
## Record n2 node displacements.
domain= modelSpace.getDomain()
n2Disp= list()
recN2Disp= domain.newRecorder("node_prop_recorder",None)
recN2Disp.setNodes(xc.ID([n2.tag]))
recN2Disp.callbackRecord= "n2Disp.append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp))"
## Record forces at the bearing.
bearingForces= list()
bearingRecorder= domain.newRecorder("element_prop_recorder",None)
bearingRecorder.setElements(xc.ID([bearing.tag]))
bearingRecorder.callbackRecord= "bearingForces.append((self.getDomain.getTimeTracker.getCurrentTime,self.getNodeResistingForceIncInertia(1).getList()))"

# 9. Perform the gravity analysis.
analysis.analyze(10)
## Set the gravity loads to be constant & reset the time in the domain
modelSpace.setLoadConstant(t= 0.0)
if(not silent):
    print('Gravity analysis completed.')
#remove recorders
domain.removeRecorders()

# 10. Perform an eigenvalue analysis-
analysis= predefined_solutions.frequency_analysis(feProblem)
analOk= analysis.analyze(3) # Compute 3 eigenvalues.
eigenvalues= analysis.getEigenvalues()
eigenvaluesTable= list([['Eigenvalues at start of transient'],
                        ["lambda","omega","period","frequency"]])
for lambdA in eigenvalues:
    omega= math.sqrt(lambdA)
    period= 2*math.pi/omega
    freq= 1/period
    eigenvaluesTable.append(["{:.3e}".format(lambdA), "{:.4f}".format(omega), "{:.4f}".format(period), "{:.4f}".format(freq)])
if(not silent):
    import tabulate
    print(tabulate.tabulate(eigenvaluesTable))

# 11. Define dynamic loads.
## Read the excitation data.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
xAccelFilePath= pth+'/../../../aux/load_patterns/ground_motions/SCS052.AT2'
yAccelFilePath= pth+'/../../../aux/load_patterns/ground_motions/SCS142.AT2'
zAccelFilePath= pth+'/../../../aux/load_patterns/ground_motions/SCSUP.AT2'
dt= .005 # Time step for the excitation sample.
scale= 1.0 #.max.=(1.1)
## Create acceleration load patterns.
xGM, xGMsz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "xGM", dof= 0, inputFileName= xAccelFilePath, dt= dt, cod_ts= 'xAccel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(xGM.name)
yGM, yGMsz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "yGM", dof= 1, inputFileName= yAccelFilePath, dt= dt, cod_ts= 'yAccel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(yGM.name)
zGM, zGMsz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "zGM", dof= 2, inputFileName= zAccelFilePath, dt= dt, cod_ts= 'zAccel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(zGM.name)

## Set the Rayleigh damping factors for nodes & elements.
alphaM= 0.05 # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm=  0.0 # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)

# 12. Define new recorders.
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
## Record forces at the bearing.
ti= list()
bearingForces2= list()
bearingBasicDeformations= list()
bearingNormalForces= list()
bearingVelocities= list()
bearingFrictionForces= list()
bearingCOFs= list()
bearingRecorder2= domain.newRecorder("element_prop_recorder",None)
bearingRecorder2.setElements(xc.ID([bearing.tag]))
callbackRecord= '''
time= self.getDomain.getTimeTracker.getCurrentTime
ti.append(time)
self.getResistingForce()
force= self.getNodeResistingForceIncInertia(1).getList()
bearingForces2.append(force)
ub= self.ub.getList()
bearingBasicDeformations.append(ub)
fm= self.frictionModels[0]
N= fm.trialN
bearingNormalForces.append(N)
V= fm.trialVel
bearingVelocities.append(V)
ff= fm.frictionForce
bearingFrictionForces.append(ff)
COF= fm.mu
bearingCOFs.append(COF)
'''
bearingRecorder2.callbackRecord= callbackRecord

# 13. Perform the dynamic analysis.
numberOfSteps= max(xGMsz, yGMsz, zGMsz)
transientSolProc= predefined_solutions.PlainNewmarkNewtonRaphson(feProblem, numSteps= numberOfSteps, timeStep= dt, gamma= 0.5, beta= 0.25, maxNumIter= 25, convergenceTestTol= 1e-12, printFlag= 0)
if(transientSolProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()


# 14. Store bearing results in a dictionary.
results= {'t': ti,
          'forces': bearingForces2,
          'deformations': bearingBasicDeformations,
          'normalForces': bearingNormalForces,
          'velocities': bearingVelocities,
          'frictionForces': bearingFrictionForces,
          'COFs': bearingCOFs}
refFilePath= pth+"/../../../aux/reference_files/"
fname= os.path.basename(__file__)
jsonFileName= refFilePath+'/ref_'+fname.replace('.py', '.json')
# # Save output as reference.
# with open(jsonFileName, 'w') as f:
#     json.dump(results, f)

# 15. Check results.
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
            if(isinstance(v, float)):
                error+=(v-v_ref)**2
            else:
                for vi, vi_ref in zip(v, v_ref):
                    error+=(vi-vi_ref)**2
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
    for dof in [0, 1, 2, 3, 4, 5]:
        accel_dof= [a[dof] for a in accel]
        vel_dof= [v[dof] for v in vel]
        disp_dof= [d[dof] for d in disp]
        fig, axes = plt.subplots(3, 1, figsize=(9, 9), sharex=True)
        axes[0].plot(t, accel_dof, lw=0.7, color="tab:blue")
        axes[0].set_ylabel("Acceleration")
        axes[0].set_title("Node 2 motion DOF: "+str(dof))
        axes[1].plot(t, vel_dof, lw=0.7, color="tab:orange")
        axes[1].set_ylabel("Velocity")
        axes[2].plot(t, disp_dof, lw=0.7, color="tab:green")
        axes[2].set_ylabel("Displacement")
        axes[2].set_xlabel("Time (s)")
        fig.tight_layout()
        plt.show()
    # Get bearing results.
    t= ti
    forces= bearingForces2
    deformations= bearingBasicDeformations
    normalForces= bearingNormalForces
    velocities= bearingVelocities
    frictionForces= bearingFrictionForces
    COFs= bearingCOFs
    colors= 2*["tab:red", "tab:green", "tab:blue"]
    magnitudes= [forces, deformations]
    titles= ['Bearing forces', 'Bearing deformations']
    units= ['ozf', '']
    for values, title in zip(magnitudes, titles):
        fig, axes = plt.subplots(nrows= 6, ncols=1, figsize=(9, 9), sharex=True)
        sz= len(values[0])
        for dof in range(0, sz):
            values_dof= [v[dof] for v in values]
            axes[dof].plot(t, values_dof, lw=0.7, color=colors[dof])
            axes[dof].set_ylabel("DOF: "+str(dof))
            if(dof==0):
                axes[0].set_title(title)
            axes[dof].set_xlabel("Time (s)")
        fig.tight_layout()
        plt.show()
    magnitudes= [velocities, normalForces, frictionForces, COFs]
    titles= ['Bearing velocities', 'Bearing normal forces', 'Bearing friction forces', 'Bearing COF']
    units= ['in/s', 'ozf', 'ozf', '']
    for values, title, unit in zip(magnitudes, titles, units):
        plt.figure(figsize=(15,3))
        plt.plot(t, values)
        plt.title(title)
        plt.ylabel(unit, {'size':14})
        plt.xlabel('Time (s)', {'fontstyle':'italic','size':13})
        plt.grid()
        plt.yticks(fontsize= 14)
        plt.xticks(fontsize= 14)
        #plt.xlim([0.0, values[-1]]);
        plt.show()

