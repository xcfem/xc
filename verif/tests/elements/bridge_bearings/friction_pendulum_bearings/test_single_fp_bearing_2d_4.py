# -*- coding: utf-8 -*-
''' Dummy SingleFPBearing adapted from TestFPS2d_4.tcl

Purpose: this file tests the 2D SingleFPBearing or the
singleFPBearing element. It models an isolated five story
one bay building and the bearing element has finite length.
It also tests the different friction models.

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

# Problem type
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodeHandler)

silent= True # True

# Model arrangement.
#
# 7 +----------------+ 14
#   |                | 
#   |                | 
# 6 +----------------+ 13
#   |                | 
#   |                | 
# 5 +----------------+ 12
#   |                | 
#   |                | 
# 4 +----------------+ 11
#   |                | 
#   |                | 
# 3 +----------------+ 10
#   |                | 
#   |                | 
# 2 +----------------+ 9
#   +                +
#  1                  8
#

# 1. Define geometry for model
g= 32.174*12.0 # 12*ft/s^2= in/s^2
P= 3.0
mass= P/g
# 1.1. Define nodes (zero-height bearing at coordinates x=0, y=0)
massMatrix= xc.Matrix([[mass, 0.0, 0.0],[0.0, mass, 0.0], [0.0, 0.0, 0.0]])
nodeDict= dict()
count= 1
for x in [0.0, 144.0]:
    for y in [0.0, 10.0, 154, 298, 442, 586, 730]:
        newNode= modelSpace.newNode(x,y)
        if(y>0.0):
            newNode.mass= massMatrix
        nodeDict[count]= newNode
        count+= 1
        
# 2. Constraints.
modelSpace.fixNode('000', nodeDict[1].tag)
modelSpace.fixNode('000', nodeDict[8].tag)

# 3. Define materials.
mv= 6.0*mass
kv= 7500.0
zetaVertical= 0.02
cv= 2.0*zetaVertical*math.sqrt(kv*mv) # Vertical damping tangent.

## Vertical response.
vertResp= typical_materials.defElasticMaterial(preprocessor, name= "vertResp", E= kv, eta= cv)
## Rotational response.
rotResp= typical_materials.defElasticMaterial(preprocessor, name= "rotResp", E= 0.0)

scc= typical_materials.defElasticSection2d(preprocessor, "scc", A= 20.0, E= 29000.0, I= 400.0, linearRho= 1.0)

# 4. Define friction model (coefficient of friction = 0.163)
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= "frictionModel", mu= .163)

'''
# frictionModel VDependent tag muSlow muFast transRate
#frictionModel VDependent 1 0.085 0.163 0.77

# frictionModel VPDependent tag muSlow muFast0 A deltaMu alpha transRate
#frictionModel VPDependent 1 0.085 0.163 7.0686 0.05 0.08 0.77
'''

# 5. Define elements
Reff= 34.68
bearing1= fb.def_single_friction_pendulum_bearing_2d(modelSpace, n1= nodeDict[1], n2= nodeDict[2], frictionModel= frictionModel, vertResp= vertResp, rotResp= rotResp, kInit= 250.0, rEff=Reff)
bearing8= fb.def_single_friction_pendulum_bearing_2d(modelSpace, n1= nodeDict[8], n2= nodeDict[9], frictionModel= frictionModel, vertResp= vertResp, rotResp= rotResp, kInit= 250.0, rEff=Reff)
## Orientation are determined by the positions of the I and J nodes.

'''
# element singleFPBearing eleTag NodeI NodeJ frnMdlTag Reff kInit -P matTag -Mz matTag <-orient x1 x2 x3 y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m> <-iter maxIter tol>
#element singleFPBearing 1 1 2 1 34.68 250.0 -P 1 -Mz 2
#element singleFPBearing 2 8 9 1 34.68 250.0 -P 1 -Mz 2
'''
lin= modelSpace.newLinearCrdTransf("lin")
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(scc)

beams= list()
nI= 2
for nJ in [3, 4, 5, 6, 7]:
    newBeamA= modelSpace.newElement("ElasticBeam2d", [nodeDict[nI].tag, nodeDict[nJ].tag])
    beams.append(newBeamA)
    newBeamB= modelSpace.newElement("ElasticBeam2d", [nodeDict[nI+7].tag, nodeDict[nJ+7].tag])
    beams.append(newBeamB)
    newBeamC= modelSpace.newElement("ElasticBeam2d", [nodeDict[nI].tag, nodeDict[nI+7].tag])
    beams.append(newBeamC)
    nI= nJ
newBeam= modelSpace.newElement("ElasticBeam2d", [nodeDict[7].tag, nodeDict[14].tag])
beams.append(newBeam)

# 6. Define gravity loads
nodesOfInterest= list()
## Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
## Create nodal loads at nodes.
for key in nodeDict:
    n= nodeDict[key]
    pos= n.getInitialPos3d
    if(pos.y>0.0):
        glp.newNodalLoad(n.tag, xc.Vector([0,-P,0]))
        nodesOfInterest.append(n.tag)
modelSpace.addLoadCaseToDomain(glp.name)

# 7. Solution procedure.
numSteps= 10
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numSteps= numSteps, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
integrator= solProc.getIntegrator()
integrator.dLambda1= .1
analysis= solProc.getAnalysis()

# 8. Define recorders.
## Record n2, n3, n5 and n6 node displacements.
domain= modelSpace.getDomain()
nodeDisp= dict()
for nTag in nodesOfInterest:
    nodeDisp[nTag]= list()
recN2Disp= domain.newRecorder("node_prop_recorder",None)
recN2Disp.setNodes(xc.ID(nodesOfInterest))
recN2Disp.callbackRecord= "nodeDisp[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp))"
## Record forces at the bearings.
bearingsOfInterest= [bearing1.tag, bearing8.tag]
bearingForces= dict()
for eTag in bearingsOfInterest:
    bearingForces[eTag]= list()
bearingRecorder= domain.newRecorder("element_prop_recorder",None)
bearingRecorder.setElements(xc.ID(bearingsOfInterest))
bearingRecorder.callbackRecord= "bearingForces[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getNodeResistingForceIncInertia(1).getList()))"

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
analOk= analysis.analyze(24) # Compute 24 eigenvalues.
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
omega1= math.sqrt(eigenvalues[0])


# 11. Define dynamic loads.
## Read the excitation data.
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
exc1AccelFilePath= pth+'/../../../aux/load_patterns/ground_motions/SCS052.AT2'
exc2AccelFilePath= pth+'/../../../aux/load_patterns/ground_motions/SCSUP.AT2'

dt= .005 # Time step for the excitation sample.
scale= 1.0 #.max.=(1.7)
## Create acceleration load patterns.
exc1GM, exc1GMsz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "exc1GM", dof= 0, inputFileName= exc1AccelFilePath, dt= dt, cod_ts= 'exc1Accel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(exc1GM.name)
exc2GM, exc2GMsz= gmu.uniform_excitation_from_simple_record(modelSpace, name= "exc2GM", dof= 1, inputFileName= exc2AccelFilePath, dt= dt, cod_ts= 'exc2Accel', factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(exc2GM.name)

## Set the Rayleigh damping factors for nodes & elements.
zeta = 0.01
beta = 2.0*zeta/omega1
alphaM= 0.0 # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm=  beta # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)

# 12. Define new recorders.
## Record n2, n3, n5 and n6 node displacements, velocities and accelerations.
domain= modelSpace.getDomain()
nodeDisp2= dict()
recN2Disp2= domain.newRecorder("node_prop_recorder", None)
recN2Disp2.setNodes(xc.ID(nodesOfInterest))
for nTag in nodesOfInterest:
    nodeDisp2[nTag]= list()
recN2Disp2.callbackRecord= "nodeDisp2[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp))"
nodeVel2= dict()
recN2Vel2= domain.newRecorder("node_prop_recorder", None)
recN2Vel2.setNodes(xc.ID(nodesOfInterest))
for nTag in nodesOfInterest:
    nodeVel2[nTag]= list()
recN2Vel2.callbackRecord= "nodeVel2[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getVel))"
nodeAccel2= dict()
recN2Accel2= domain.newRecorder("node_prop_recorder", None)
recN2Accel2.setNodes(xc.ID(nodesOfInterest))
for nTag in nodesOfInterest:
    nodeAccel2[nTag]= list()
recN2Accel2.callbackRecord= "nodeAccel2[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getAccel))"
## Record forces at the bearing.
ti= dict()
bearingForces2= dict()
bearingBasicDeformations= dict()
bearingNormalForces= dict()
bearingVelocities= dict()
bearingFrictionForces= dict()
bearingCOFs= dict()
bearingsMotionRecorder= domain.newRecorder("element_prop_recorder",None)
bearingsMotionRecorder.setElements(xc.ID(bearingsOfInterest))
for eTag in bearingsOfInterest:
    ti[eTag]= list()
    bearingForces2[eTag]= list()
    bearingBasicDeformations[eTag]= list()
    bearingNormalForces[eTag]= list()
    bearingVelocities[eTag]= list()
    bearingFrictionForces[eTag]= list()
    bearingCOFs[eTag]= list()
callbackRecord= '''
time= self.getDomain.getTimeTracker.getCurrentTime
ti[self.tag].append(time)
self.getResistingForce()
force= self.getNodeResistingForceIncInertia(1).getList()
bearingForces2[self.tag].append(force)
ub= self.ub.getList()
bearingBasicDeformations[self.tag].append(ub)
fm= self.frictionModels[0]
N= fm.trialN
bearingNormalForces[self.tag].append(N)
V= fm.trialVel
bearingVelocities[self.tag].append(V)
ff= fm.frictionForce
bearingFrictionForces[self.tag].append(ff)
COF= fm.mu
bearingCOFs[self.tag].append(COF)
'''
bearingsMotionRecorder.callbackRecord= callbackRecord

# 13. Perform the dynamic analysis.
numberOfSteps= max(exc1GMsz, exc2GMsz)
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
for magnitudeKey in ref_results:
    ref_values= ref_results[magnitudeKey]
    values= results[magnitudeKey]
    for bearingKey in ref_values:
        element_ref_values= ref_values[bearingKey]
        element_values= values[int(bearingKey)]
        error+= (len(element_values)-len(element_ref_values))**2
        if(error<tol):
            for v, v_ref in zip(element_values, element_ref_values):
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
    # Get nodes motion.
    for nTag in nodesOfInterest:
        t, accel= zip(*nodeAccel2[nTag])
        t, vel= zip(*nodeVel2[nTag])
        t, disp= zip(*nodeDisp2[nTag])
        for dof in [0, 1, 2]:
            accel_dof= [a[dof] for a in accel]
            vel_dof= [v[dof] for v in vel]
            disp_dof= [d[dof] for d in disp]
            fig, axes = plt.subplots(3, 1, figsize=(9, 9), sharex=True)
            axes[0].plot(t, accel_dof, lw=0.7, color="tab:blue")
            axes[0].set_ylabel("Acceleration")
            axes[0].set_title("Node "+str(nTag)+" motion DOF: "+str(dof))
            axes[1].plot(t, vel_dof, lw=0.7, color="tab:orange")
            axes[1].set_ylabel("Velocity")
            axes[2].plot(t, disp_dof, lw=0.7, color="tab:green")
            axes[2].set_ylabel("Displacement")
            axes[2].set_xlabel("Time (s)")
            fig.tight_layout()
            plt.show()
    # Get bearing results.
    for eTag in bearingsOfInterest:
        t= ti[eTag]
        forces= bearingForces2[eTag]
        deformations= bearingBasicDeformations[eTag]
        normalForces= bearingNormalForces[eTag]
        velocities= bearingVelocities[eTag]
        frictionForces= bearingFrictionForces[eTag]
        COFs= bearingCOFs[eTag]
        colors= ["tab:red", "tab:green", "tab:blue"]
        magnitudes= [forces, deformations]
        titles= ['Bearing forces', 'Bearing deformations']
        units= ['ozf', '']
        for values, title in zip(magnitudes, titles):
            fig, axes = plt.subplots(3, 1, figsize=(9, 9), sharex=True)
            sz= len(values[0])
            for dof in range(0, sz):
                values_dof= [v[dof] for v in values]
                axes[dof].plot(t, values_dof, lw=0.7, color=colors[dof])
                axes[dof].set_ylabel("DOF: "+str(dof))
                if(dof==0):
                    axes[0].set_title(title+' (bearing: '+str(eTag)+')')
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

