# -*- coding: utf-8 -*-
''' Dummy FlatSiderSimple2d adapted from TestSlider2d_0.tcl

Purpose: this file tests the 3D flatSliderBearing or the
singleFPBearing element. It models an isolated one story
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
P= 9.0
mass= P/g
templateMatrix= xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])
nodeMassMatrix= mass*templateMatrix


# Define geometry for model
# -------------------------
xi= 3*[0.0]+3*[144.0]+3*[0.0]+3*[144.0]+2*[72.0]
yi= 6*[0.0]+6*[144.0]+2*[72.0]
zi= 4*[0.0, 10.0, 154.0]+[10.0, 154.0]
count= 1
nodeDict= dict()
for x, y, z in zip(xi, yi, zi):
    newNode= modelSpace.newNode(x,y,z)
    if(z>0.0 and x!=72.0):
        newNode.mass= nodeMassMatrix
    nodeDict[count]= newNode
    count+= 1

# 2. Constraints.
for key in [1, 4, 7, 10]:
    modelSpace.fixNode('000_000', nodeDict[key].tag)
for key in [13, 14]:
    modelSpace.fixNode('FF0_000', nodeDict[key].tag)

# Set the multi-point constraints
# rigidDiaphragm perpDir mNodeTag sNodeTags
# rigidDiaphragm(3, 13, 2, 5, 8, 11)
for key in [2, 5, 8, 11]:
    modelSpace.newEqualDOF(nodeDict[13].tag, nodeDict[key].tag, dofs= xc.ID([0,1]))
# rigidDiaphragm(3, 14, 3, 6, 9, 12)
for key in [3, 6, 9, 12]:
    modelSpace.newEqualDOF(nodeDict[14].tag, nodeDict[key].tag, dofs= xc.ID([0,1]))
    
# 3. Define materials.
mv= 2.0*mass
kv= 7500.0
zetaVertical= 0.02
cv= 2.0*zetaVertical*math.sqrt(kv*mv) # Vertical damping tangent.
## Vertical response.
vertResp= typical_materials.defElasticMaterial(preprocessor, name= "vertResp", E= kv, eta= cv)
## Rotational response.
rotResp= typical_materials.defElasticMaterial(preprocessor, name= "rotResp", E= 0.0)

scc= typical_materials.defElasticSection3d(preprocessor, "scc", A= 20.0, E= 29000.0, G= 11154.0, J= 100.0, Iy= 400.0, Iz= 400.0, linearRho= 1.0)

# 4. Define friction model (coefficient of friction = 0.163)
frictionModel= fm.def_coulomb_friction_model(preprocessor, name= "frictionModel", mu= .163)

'''
# frictionModel VDependent tag muSlow muFast transRate
#frictionModel VDependent 1 0.085 0.163 0.77

# frictionModel VPDependent tag muSlow muFast0 A deltaMu alpha transRate
#frictionModel VPDependent 1 0.085 0.163 7.0686 0.05 0.08 0.77
'''

# 5. Define elements
bearingDict= dict()
count= 1
for (keyN1, keyN2) in [(1, 2), (4, 5), (7, 8), (10, 11)]:
    nI= nodeDict[keyN1]
    nJ= nodeDict[keyN2]
    bearingDict[count]= fb.def_flat_slider_bearing_3d(modelSpace, n1= nI, n2= nJ, frictionModel= frictionModel, vertResp= vertResp, rotRespX= rotResp, rotRespY= rotResp, rotRespZ= rotResp, kInit= 250.0, x= xc.Vector([1,0,0]))
    count+= 1

'''
# element singleFPBearing eleTag NodeI NodeJ frnMdlTag Reff kInit -P matTag -T matTag -My matTag -Mz matTag <-orient <x1 x2 x3> y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m> <-iter maxIter tol>
#element singleFPBearing 1  1  2 1 34.68 250.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element singleFPBearing 2  4  5 1 34.68 250.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element singleFPBearing 3  7  8 1 34.68 250.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element singleFPBearing 4 10 11 1 34.68 250.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0

# element RJWatsonEqsBearing eleTag NodeI NodeJ frnMdlTag kInit k2 k3 mu -P matTag -Mz matTag <-orient x1 x2 x3 y1 y2 y3> <-shearDist sDratio> <-doRayleigh> <-mass m> <-iter maxIter tol>
#element RJWatsonEqsBearing 1  1  2 1 250.0 0.519 0.0 3.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element RJWatsonEqsBearing 2  4  5 1 250.0 0.519 0.0 3.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element RJWatsonEqsBearing 3  7  8 1 250.0 0.519 0.0 3.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
#element RJWatsonEqsBearing 4 10 11 1 250.0 0.519 0.0 3.0 -P 1 -T 2 -My 2 -Mz 2 -orient 1 0 0
'''
lin1= modelSpace.newLinearCrdTransf("lin1", xzVector= xc.Vector([1, 0, 0]))
lin2= modelSpace.newLinearCrdTransf("lin2", xzVector= xc.Vector([0, 1, 0]))
lin3= modelSpace.newLinearCrdTransf("lin3", xzVector= xc.Vector([0, -1, 0]))
modelSpace.setDefaultMaterial(scc)
beamDict= dict()
count= 1
for (keyN1, keyN2, cTrf) in [(2, 3, lin2), (5, 6, lin2), (8, 9, lin2), (11, 12, lin2), (2, 5, lin3), (8, 11, lin3), (2, 8, lin1), (5, 11, lin1), (3, 6, lin3), (9, 12, lin3), (3, 9, lin1), (6, 12, lin1)]:
    nI= nodeDict[keyN1]
    nJ= nodeDict[keyN2]
    modelSpace.setDefaultCoordTransf(cTrf)
    beamDict[count]= modelSpace.newElement("ElasticBeam3d", [nI.tag, nJ.tag])
    count+= 1
    
# 6. Define gravity loads
## Create a Plain load pattern with a Linear TimeSeries
lts= modelSpace.newTimeSeries(name= 'lts', tsType= 'linear_ts')
glp= modelSpace.newLoadPattern(name= 'glp', setCurrent= True)
## Create nodal loads at nodes.
for key in [2, 5, 8, 11, 3, 6, 9, 12]:
    n= nodeDict[key]
    glp.newNodalLoad(n.tag, xc.Vector([0,0,-P,0,0,0]))
modelSpace.addLoadCaseToDomain(glp.name)

# 7. Solution procedure.
numSteps= 10
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, maxNumIter= 10, convergenceTestTol= 1e-12, printFlag= 0, numSteps= numSteps, numberingMethod= 'rcm', convTestType= 'norm_disp_incr_conv_test')
solProc.setup()
integrator= solProc.getIntegrator()
integrator.dLambda1= .1
analysis= solProc.getAnalysis()

# 8. Define recorders.
## Record node displacements.
nodesOfInterest= list()
for key in [2, 3, 5, 6, 8, 9, 11, 12]:
    n= nodeDict[key]
    nodesOfInterest.append(n.tag) 
domain= modelSpace.getDomain()
nodeDisp= dict()
for nTag in nodesOfInterest:
    nodeDisp[nTag]= list()
recN2Disp= domain.newRecorder("node_prop_recorder",None)
recN2Disp.setNodes(xc.ID(nodesOfInterest))
recN2Disp.callbackRecord= "nodeDisp[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp))"
## Record forces at the bearing.
bearingsOfInterest= [v.tag for v in list(bearingDict.values())]
bearingForces= dict()
for eTag in bearingsOfInterest:
    bearingForces[eTag]= list()
bearingRecorder= domain.newRecorder("element_prop_recorder",None)
bearingRecorder.setElements(xc.ID(bearingsOfInterest))
bearingRecorder.callbackRecord= "bearingForces[self.tag].append((self.getDomain.getTimeTracker.getCurrentTime,self.getNodeResistingForceIncInertia(1).getList()))"

# 9. Perform the gravity analysis.
analOK= analysis.analyze(10)
if(analOK!= 0):
    lmsg.error("Can't solve.")
    exit(1)
## Set the gravity loads to be constant & reset the time in the domain
modelSpace.setLoadConstant(t= 0.0)
if(not silent):
    print('Gravity analysis completed.')
#remove recorders
domain.removeRecorders()

# 10. Perform an eigenvalue analysis-
analysis= predefined_solutions.frequency_analysis(feProblem)
analOk= analysis.analyze(14) # Compute 14 eigenvalues.
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
zeta = 0.01
beta = 2.0*zeta/omega1
alphaM= 0.0 # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm=  beta # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)

# 12. Define new recorders.
## Record node displacements, velocities and accelerations.
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
numberOfSteps= max(xGMsz, yGMsz, zGMsz)
transientSolProc= predefined_solutions.PlainNewmarkKrylovNewton(feProblem, numSteps= numberOfSteps, timeStep= dt, gamma= 0.5, beta= 0.25, maxNumIter= 25, convergenceTestTol= 1e-12, printFlag= 0)
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

if error<tol:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

if(not silent):
    # Display FE model.
    from postprocess import output_handler
    oh= output_handler.OutputHandler(modelSpace)
    oh.displayFEMesh()#setsToDisplay= [columnSet, pileSet])
    # oh.displayDispRot(itemToDisp='uX', defFScale= 100.0)
    # oh.displayLocalAxes()
    
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
        colors= 2*["tab:red", "tab:green", "tab:blue"]
        magnitudes= [forces, deformations]
        titles= ['Bearing forces', 'Bearing deformations']
        units= ['ozf', '']
        for values, title in zip(magnitudes, titles):
            fig, axes = plt.subplots(6, 1, figsize=(9, 9), sharex=True)
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


