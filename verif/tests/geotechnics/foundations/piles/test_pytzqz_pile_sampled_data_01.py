# -*- coding: utf-8 -*-
'''Check the Pile class when using sampled data instead of layers to represent
   the properties of soil.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

silent= True

import os
import math
import json
import geom
import xc
import numpy as np
from scipy.constants import g
from model import predefined_spaces
from materials.ec2 import EC2_materials
from geotechnics.foundations import soil_sampled_data
from geotechnics.foundations import sampled_data_pytzqz_pile
from solution import rayleigh_damping as rd
from solution import predefined_solutions
from solution import transient
from solution import hht
from actions.quake import ground_motion_utils as gmu


feProblem= xc.FEProblem()
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
# Problem type
modelSpace= predefined_spaces.StructuralMechanics3D(nodes) # Defines the

# Problem geometry.
pileLengths= {'P-4': 23.5, 'P-5': 32.0, 'P-6': 33.5, 'P-7': 39.0, 'P-8': 38.0, 'P-9': 40.0, 'P-10': 40.0, 'P-11': 38.0, 'P-12': 40.0, 'P-13': 40.0, 'P-14': 34.0, 'P-15': 34.0, 'P-16': 34.0, 'P-17': 36.0, 'P-18': 36.0, 'P-19': 36.0, 'P-20': 38.0, 'P-21': 36.0, 'P-22': 39.0, 'P-23': 39.0, 'P-24': 33.0, 'P-25': 33.0, 'P-26': 31.0, 'P-27': 31.0, 'P-28': 31.0+1e-3, 'P-29': 33.0, 'P-30': 33.0, 'P-31': 27.0, 'P-32': 27.0, 'E-2': 24.0}
pileHeadZ= 3.15 # z coordinate of the pile heads.
selectedPiles= ['P-11']#, 'P-6', 'E-2']

## k-points.
headPoints= dict()
tipPoints= dict()
x= 0.0
y= 0.0
for i, pileCode in enumerate(selectedPiles):
    length= pileLengths[pileCode]
    headPoints[pileCode]= modelSpace.newKPoint(x, y, pileHeadZ)
    tipPoints[pileCode]= modelSpace.newKPoint(x, y, pileHeadZ-length)
    x+= 20.0
    if (i % 2)==0: # even
        y= 50.0
    else: # odd
        y= -50.0
    

## Lines.
pileLines= dict()
for pileCode in selectedPiles:
    headPoint= headPoints[pileCode]
    tipPoint= tipPoints[pileCode]
    pileLines[pileCode]= modelSpace.newLine(headPoint, tipPoint)

# Load soil data.
pgpSet= soil_sampled_data.SoilSampledDataSet()

# Read the soil data.
pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
soilDataFilePath= pth+'/../../../aux/soil_data/pile_geotechnical_parameters.json'
pgpSet.readFromFile(soilDataFilePath)

# Create pile objects.
piles= dict()
## Materials
concrete= EC2_materials.C35
for pileCode in selectedPiles:
    # Get the soil data for this pile.
    soilData= pgpSet.getSoilSampledData(pileCode)
    # Set the ground level.
    line= pileLines[pileCode]
    groundLevel= max(line.getP1().getPos.z, line.getP2().getPos.z)
    soilData= pgpSet.getSoilSampledData(pileCode)
    soilData.groundLevel= groundLevel
    
    # Create the pile.
    newPile= sampled_data_pytzqz_pile.PyTzQzCircularPile(E= concrete.getEcm(), diameter= 1.8, soilData= soilData, pileSet= line)
    piles[pileCode]= newPile
    
# Generate the mesh.
## Set material.
samplePile= piles[selectedPiles[0]]
xcMaterial= samplePile.defElasticShearSection3d(preprocessor)
modelSpace.setDefaultMaterial(xcMaterial)
## Set coordinate transformation.
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))
modelSpace.setDefaultCoordTransf(lin)
## Set seed element.
modelSpace.newSeedElement('ElasticBeam3d')
elementSize= 1.0 # m
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        line= pile.pileSet
        line.setElemSize(elementSize)
        line.genMesh(xc.meshDir.I)

# Create constraints.
soilSprings= dict()
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        soilSprings[pileCode]= pile.generateSpringsPile(fixZRotationOfTipNode= True)

# Set top node masses.
mass= 1e6/g
templateMatrix= xc.Matrix([[1,0,0,0,0,0],
                           [0,1,0,0,0,0],
                           [0,0,1,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0],
                           [0,0,0,0,0,0]])
topNodeMassMatrix= mass*templateMatrix
pileTopNodes= list()
for pileCode in piles:
    pile= piles[pileCode]
    if(pile):
        ln= pile.pileSet
        nHead= ln.getP1().getNode()
        nHead.setMassMatrix(topNodeMassMatrix)
        pileTopNodes.append(nHead)
    
# Gravity load.
gLC= modelSpace.newLoadPattern(name= 'gLC')
modelSpace.setCurrentLoadPattern(gLC.name)
accel= xc.Vector([0, 0, g])
for n in pileTopNodes:
    n.createInertiaLoad(accel)
modelSpace.addLoadCaseToDomain(gLC.name) # Add to domain.

## Solve for gravity.
solProc= predefined_solutions.PlainNewtonRaphsonBandGen(feProblem, convergenceTestTol= 1e-4, maxNumIter= 20, printFlag= 0)
solProc.setup()
analysis= solProc.getAnalysis()
result= analysis.analyze(1)
if(result!=0):
    lmsg.error("Can't solve.")
    exit(1)
    
# Perform an eigenvalue analysis-
analysis= predefined_solutions.spectra_frequency_analysis(feProblem)
analOk= analysis.analyze(30) # Compute 20 eigenvalues.
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
    
## Set the Rayleigh damping factors for nodes & elements.
freq1= 1.168 # Lower frequency (Hz)
freq2= 1.823 # Upper frequency (Hz)
target_zeta= 0.05  # 5% target structural damping
alph, beta = rd.compute_rayleigh_damping(freq1, freq2, target_zeta)
domain= modelSpace.getDomain()
alphaM= alph # mass proportional damping.
betaK= 0.0 # current stiffness proportional damping.
betaKinit= 0.0 # initial stiffness proportional damping.
betaKcomm= beta # commited stiffness proportional damping.
rayleigh= xc.RayleighDampingFactors(alphaM, betaK, betaKinit, betaKcomm)
domain.setRayleighDampingFactors(rayleigh)

# Create recorders.
tagsPileTopNodes= [n.tag for n in pileTopNodes]
domain= modelSpace.getDomain()
targetDOF= 2
cDisp= list()
recDisp= domain.newRecorder("node_prop_recorder",None)
recDisp.setNodes(xc.ID(tagsPileTopNodes))
recDisp.callbackRecord= "cDisp.append((self.getDomain.getTimeTracker.getCurrentTime,self.getDisp[targetDOF]))"
cAccel= list()
recAccel= domain.newRecorder("node_prop_recorder",None)
recAccel.setNodes(xc.ID(tagsPileTopNodes))
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
xGM, xGMSize= gmu.get_uniform_excitation_from_accel_values(modelSpace, name= motionName, dof= targetDOF, accelValues= accelerationValues, dt= timeStep, cod_ts= cod_ts, factor= scale, vel0= 0.0)
modelSpace.addLoadCaseToDomain(xGM.name)

## Perform the analysis.
domain.setTime(0.0) # initialize time.
alpha_hht = 0.9 # Introduces moderate numerical damping
dt= timeStep
numberOfSteps= int(xGMSize*timeStep/dt)
solProc= hht.HHT(prb= feProblem, name= 'hht', timeStep= dt, maxNumIter= 20, convergenceTestTol= .05, printFlag= 0, numSteps= numberOfSteps, numberingMethod= 'rcm', convTestType= 'relative_total_norm_disp_incr_conv_test', soeType= 'mumps_soe', solverType= 'mumps_solver', alpha= alpha_hht, solutionAlgorithmType= 'newton_raphson_soln_algo', constraintHandlerType= 'plain')
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
ratio1= abs(uxMax-0.050468338234872755)/0.050468338234872755
ratio2= abs(tUxMax-26.3)/26.3
ratio3= abs(axMax/g-0.2628336061022945)/0.2628336061022945
ratio4= abs(tAxMax-1.92)/1.92

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
