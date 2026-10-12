# -*- coding: utf-8 -*-
'''Check the damping effect of the PySimple1, TzSimple1 and QzSimple1 materials.

Inspired on the example 14.4.1 of OpenSees (see https://openseespydoc.readthedocs.io/en/stable/src/pile.html) by Pavan Chigullapally from University of Auckland, Auckland.
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

silent= True

import os
import math
import xc
import numpy as np
from model import predefined_spaces
from model.sets import sets_mng 
from materials import soil_structure_interaction as ssi
from materials.ec2 import EC2_materials
from materials.sections.fiber_section import def_column_RC_section
from materials import typical_materials
from geotechnics.foundations import pile
from solution import predefined_solutions
from solution import transient
from solution import hht
from actions.quake import ground_motion_utils as gmu
from scipy.constants import g
from misc_utils import log_messages as lmsg

# Pile geometry and mesh
L1 = 1.0 # length of pile head (above ground surface) (m)
L2 = 20.0 # length of embedded pile (below ground surface) (m)
pileDiameter = 0.5 # pile diameter
eleSize= 0.25 # pile element length
# Mass on the top node.
N=-1e6
mass= abs(N)/g
templateMatrix= xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])
columnTopNodeMassMatrix= mass*templateMatrix

# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodeHandler= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodeHandler)

# K-points.
pileTipPt= modelSpace.newKPoint(0.0, 0.0, 0.0) # pile tip.
columnBottomPt= modelSpace.newKPoint(0.0, 0.0, L2)
columnTopPt= modelSpace.newKPoint(0.0, 0.0, L1+L2) # pile head.

# Lines.
pileLine= modelSpace.newLine(pileTipPt, columnBottomPt)
columnLine= modelSpace.newLine(columnBottomPt, columnTopPt)

# Create mesh.
## Pile section.
## Materials.
concrete= EC2_materials.C35
reinfSteel= EC2_materials.S500B
### RC section.
rcSection= def_column_RC_section.RCCircularSection(name= 'rcSection', sectionDescr= 'RC section', concrType= concrete, reinfSteelType= reinfSteel, Rext= pileDiameter/2.0)
pileSection= rcSection.defElasticShearSection3d(preprocessor, reductionFactor= 5.0)

# Coordinate transfomration.
xz_vector= xc.Vector([0,-1,0])
lin= modelSpace.newLinearCrdTransf("lin", xz_vector)
## Create elements.
modelSpace.setDefaultCoordTransf(lin)
modelSpace.setDefaultMaterial(pileSection)
modelSpace.newSeedElement("ElasticBeam3d")
for l in [pileLine, columnLine]:
    l.setElemSize(eleSize)
    l.genMesh(xc.meshDir.I)
pileSet= modelSpace.defSet('pileSet', lines= [pileLine, columnLine])
pileSet.fillDownwards()

# Soil properties
soilRho= 20e3/g # soil density kg/m3
soilPhi= math.radians(30.0) # soil internal friction angle (radians)
soilG= 150000000.0 # soil shear modulus at pile tip (Pa).

# select pult definition method for p-y curves
# API (default) --> 1
# Brinch Hansen --> 2
puSwitch= 1 

# variation in coefficent of subgrade reaction with depth for p-y curves
# API linear variation (default)   --> 1
# modified API parabolic variation --> 2
kSwitch= 1

# effect of ground water on subgrade reaction modulus for p-y curves
# above gwt --> 1
# below gwt --> 2
gwtSwitch = 1

## Create springs along the pile shaft.
tributaryLengths= sets_mng.get_tributary_lengths(xcSet= pileLine, initialGeometry= True)

modelSpace.setElementDimension(3)
springNodes= list()
springElements= list()
for n in pileLine.nodes:
    nPos= n.getInitialPos3d
    pyDepth= L2-nPos.z
    tributaryLength=  tributaryLengths[n.tag]
    pult, y50= pile.get_py_param(pyDepth, soilRho, soilPhi, pileDiameter, tributaryLength, puSwitch, kSwitch, gwtSwitch)
    pyMatName= 'pyS_'+str(n.tag)
    pyMat= ssi.def_pysimple1_material(preprocessor,
                                      matName= pyMatName,
                                      soilType= 2, # Sand.
                                      pult= pult,
                                      Y50= y50,
                                      c= 0.0,
                                      Cd= 0.0)

    sigV= soilRho*g*pyDepth # Vertical stress.
    tult, z50= pile.get_tz_param(soilPhi, pileDiameter, sigV, tributaryLength)
    tzMatName= 'tzS:'+str(n.tag)
    tzMat= ssi.def_tzsimple1_material(preprocessor,
                                      matName= tzMatName,
                                      tzType= 2, # Mosher relation.
                                      tult= tult, # Ultimate skin friction capacity.
                                      z50= z50, # 50% capacity reached at 0.05 units of displacement
                                      c= 0.0)
    # Create spring.
    fixNode= modelSpace.duplicateNode(n)
    modelSpace.fixNode('000_000', fixNode.tag)
    modelSpace.setDefaultMaterial(pyMat)
    pileShaftSpring= modelSpace.newElement("ZeroLength", [fixNode.tag, n.tag])
    pileShaftSpring.clearMaterials()
    pileShaftSpring.setMaterial(0, pyMat.name)
    pileShaftSpring.setMaterial(1, pyMat.name)
    pileShaftSpring.setMaterial(2, tzMat.name)
    springNodes.append(fixNode)
    springElements.append(pileShaftSpring)

springElementSet= modelSpace.defSet('springElementSet', elements= springElements)
springElementSet.fillDownwards()

## Create spring at the pile tip.
### q-z spring material
    
pileTipNode= pileTipPt.getNode()
### Fix vertical rotation of tip node.
modelSpace.fixNode('FFF_FF0', pileTipNode.tag)
### Compute tip node depth.
pileTipNodePos= pileTipNode.getInitialPos3d
pileTipNodeDepth= L2-pileTipNodePos.z
### vertical effective stress at pile tip, no water table (depth is embedded pile length)
sigVq= soilRho*g * pileTipNodeDepth # (N)
qult, z50q = pile.get_qz_param(phi= soilPhi, b= pileDiameter, sigV= sigVq, G= soilG)
# op.uniaxialMaterial('QzSimple1', 101, 2, qult, z50q) #, 0.0, 0.0
# op.uniaxialMaterial('TzSimple1', 101, 2, qult, z50q, 0.0)
qzMat= ssi.def_qzsimple1_material(preprocessor,
                                  matName= 'qzS1_'+str(pileTipNode.tag),
                                  qzType= 2, # 2 = Sand (Vijayvergiya, 1977 behavior).
                                  qult= qult, # Ultimate tip capacity.
                                  z50= z50q, # 50% capacity reached at 0.005 units of displacement
                                  suction= 0.0, # No suction during tension/uplift
                                  c= 0.0) # No viscous damping for this static test
pileTipFixNode= modelSpace.duplicateNode(pileTipNode)
modelSpace.fixNode('000_000', pileTipFixNode.tag)
modelSpace.setDefaultMaterial(qzMat)
modelSpace.setElementDimension(3)
pileTipSpring= modelSpace.newElement("ZeroLength", [pileTipFixNode.tag, pileTipNode.tag])
# Vertical direction.
pileTipSpring.setupVectors(xc.Vector([0,0,1]),xc.Vector([0,-1,0]))
pileTipSpring.clearMaterials()
pileTipSpring.setMaterial(0, qzMat.name)

# Gravity load.
columnTopNode= columnTopPt.getNode()
columnTopNode.mass= columnTopNodeMassMatrix
gLC= modelSpace.newLoadPattern(name= 'gLC')
modelSpace.setCurrentLoadPattern(gLC.name)
accel= xc.Vector([0, 0, g])
columnTopNode.createInertiaLoad(accel)
pileSet.createInertiaLoads(accel)
modelSpace.addLoadCaseToDomain(gLC.name) # Add to domain.

## Solve for gravity.
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-4, maxNumIter= 20, printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)

## Set the Rayleigh damping factors for nodes & elements.
domain= modelSpace.getDomain()
alphaM= .05 # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm=  0.0 # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)

# Perform an eigenvalue analysis-
analysis= predefined_solutions.frequency_analysis(feProblem)
analOk= analysis.analyze(6) # Compute 6 eigenvalues.
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

# Create recorders.
domain= modelSpace.getDomain()
targetDOF= 0
cDisp= list()
recDisp= domain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID([columnTopNode.tag]))
recDisp.callbackRecord= "cDisp.append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp[targetDOF]))"
cAccel= list()
recAccel= domain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID([columnTopNode.tag]))
recAccel.callbackRecord= "cAccel.append((self.getDomain.getTimeTracker.getCurrentTime,self.getAccel[targetDOF]))"
## Loading El Centro EQ data (North-south component)
pth= os.path.dirname(__file__)
# print("pth= ", pth)
if(not pth):
    pth= '.'
accelFilePath= pth+'/../../../aux/load_patterns/ground_motions/elCentro.txt'
el_centro_raw= np.loadtxt(accelFilePath)
timeValues= el_centro_raw[:,0]
accelerationValues= list(el_centro_raw[:,1]*g)
timeStep= .02 # Time step for the excitation sample.
scale= 1.0
## Define ground motion as uniform excitation.
motionName= 'el_centro'
cod_ts= motionName+'_accel'
xGM, xGMSize= gmu.get_uniform_excitation_from_accel_values(modelSpace, name= motionName, dof= targetDOF, accelValues= accelerationValues, dt= timeStep, cod_ts= cod_ts, factor= g*scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(xGM.name)

## Perform the analysis.
domain.setTime(0.0) # initialize time.
alpha_hht = 0.9 # Introduces moderate numerical damping
dt= timeStep
numberOfSteps= int(2*xGMSize*timeStep/dt)
solProc= hht.HHT(prb= feProblem, name= 'hht', timeStep= dt, maxNumIter= 20, convergenceTestTol= .05, printFlag= 0, numSteps= numberOfSteps, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'band_gen_lin_soe', solverType= 'band_gen_lin_lapack_solver', alpha= alpha_hht, solutionAlgorithmType= 'newton_raphson_soln_algo', constraintHandlerType= 'plain')
# solProc= transient.PlainNewmarkNewtonRaphson(prb= feProblem, numSteps= numberOfSteps, timeStep= timeStep, maxNumIter= 20, convTestType= 'energy_incr_conv_test', printFlag= 0)
if(solProc.solve()!=0):
    lmsg.error('Dynamic analysis failed.')
    quit()

# Compute maximum and minimum displacement and acceleration.
uXExtrema= gmu.MinMaxTracker()
for i, (t, ux) in enumerate(cDisp):
    uXExtrema.update(ux, t, i)
aXExtrema= gmu.MinMaxTracker()
for i, (t, ax) in enumerate(cAccel):
    aXExtrema.update(ax, t, i)

if(abs(aXExtrema.min)>abs(aXExtrema.max)):
    axMax= aXExtrema.min
    tAxMax= aXExtrema.t_min
else:
    axMax= aXExtrema.max
    tAxMax= aXExtrema.t_max

if(abs(uXExtrema.min)>abs(uXExtrema.max)):
    uxMax= uXExtrema.min
    tUxMax= uXExtrema.t_min
else:
    uxMax= uXExtrema.max
    tUxMax= uXExtrema.t_max

uxMax= max(abs(uXExtrema.min), abs(uXExtrema.max))

# Display results
if(not silent):
    import matplotlib.pyplot as plt
    ti, Dxi= zip(*cDisp)
    plt.ylabel("Displacement (m)")
    plt.xlabel("Time (s)")
    plt.plot(ti, Dxi)
    plt.show()
    ti, Axi= zip(*cAccel)
    plt.ylabel("Acceleration ($m/s^2$)")
    plt.xlabel("Time (s)")
    plt.plot(ti, Axi)
    plt.show()

    # Graphic stuff.
    # from postprocess import output_handler
    # oh= output_handler.OutputHandler(modelSpace)
    # oh.displayFEMesh()
    # oh.displayNodeValueDiagram(itemToDisp= 'k_x', caption= 'Spring stiffness x direction', defaultDirection= 'X', defaultValue= 0.0)
    # oh.displayNodeValueDiagram(itemToDisp= 'k_y', caption= 'Spring stiffness y direction', defaultDirection= 'Y', defaultValue= 0.0)
    # oh.displayDispRot(itemToDisp='uZ')
    # oh.displayIntForcDiag(itemToDisp='Vy',setToDisplay=pile)
    # oh.displayIntForcDiag(itemToDisp='Mz',setToDisplay=pile)
    
ratio1= abs(uxMax-2.4508807988231696)/2.4508807988231696
ratio2= abs(tUxMax-5.02)/5.02
ratio3= abs(axMax/g+3.15444615191563)/3.15444615191563
ratio4= abs(tAxMax-4.56)/4.56

if(not silent):
    print('time increment: ', dt, 's')
    print('maximum displacement: ', uxMax, 'm', ' at time: ', tUxMax, 's')
    print('ratio1= ', ratio1)
    print('ratio2= ', ratio2)
    print('maximum acceleration: ', axMax/g, 'g', ' at time: ', tAxMax, 's')
    print('ratio3= ', ratio3)
    print('ratio4= ', ratio4)
    
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15 and abs(ratio2)<1e-12 and abs(ratio3)<1e-15 and abs(ratio4)<1e-12:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
