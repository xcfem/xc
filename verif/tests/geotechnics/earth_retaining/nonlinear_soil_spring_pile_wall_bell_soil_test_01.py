# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example "Numerical 3" from the
    page 23 of the document:

    https://abhashacharya.com.np/wp-content/uploads/2021/05/Sheet-Piles-with-Numericals.pdf
'''

import math
from geotechnics import earth_pressure
from scipy.constants import g
from scipy.optimize import fsolve
import numpy as np
from materials.sections.fiber_section import def_column_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from geotechnics.earth_retaining import pile_wall as pw
from misc_utils import log_messages as lmsg

# Problem geometry. 
H= 6.0
alphaAngle= 0.0 #angle of the back of the retaining wall (radians).
backFillSlope= 0.0 # slope of the backfill (radians).

# Soil
qu= 70e3 # unconfined compressive strength.
Cu= qu/2.0

wallFrictionAngle= 0.0 # friction angle between soil an back of retaining wall (radians).

# Solve the analytical problem to compute reference values.

ep= earth_pressure.BellSoil(phi= math.radians(0.0), c= Cu, rho= 17e3/g, rhoSat= 17e3/g)

# Tension crack depth.
h0= ep.getCoulombTensionCrackDepth(sg_v= 0.0)

## Horizontal active pressure.
sg_v= ep.soil.gamma()*H
eah= ep.getActivePressure(sg_v= sg_v, alphaAngle= alphaAngle, deltaAngle= wallFrictionAngle)
Pa= 1/2.0*eah*(H-h0)

## Moment residual.
def moment_residual(D, z):
    return Pa*(D+(H-h0)/3)+1/2.0*8*Cu*z*z/3-(4*Cu-ep.soil.gamma()*H)*D*D/2.0

def force_residual(D, z):
    return Pa+1/2.0*8*Cu*z-(4*Cu-ep.soil.gamma()*H)*D

def func(x):

    return [force_residual(x[0], x[1]),

            moment_residual(x[0],x[1])]

root= fsolve(func, [H, H/2.0])
isClose= np.isclose(func(root), [0.0, 0.0])
solutionOK= (isClose[0] and isClose[1])
if(not solutionOK):
    lmsg.error("Can't solve equations.")
    exit(1)
D= root[0]
z= root[1]

# Build the pila wall model.
ep.Kh= 30e6
### Soil strata.
L1= 6.0 #5.0 # Excavation depth (m)
L= L1+1.4*D # Total lenght (m)
soilLayersDepths= [0.0, L]
soilLayers= [ep, ep]

## Pile material.
concr= EHE_materials.HA30
steel= EHE_materials.B500S
diameter= 450e-3 # Cross-section diameter [m]
pileSection= def_column_RC_section.RCCircularSection(name='test',Rext= diameter/2.0, concrType=concr, reinfSteelType= steel)

## Pile wall.
pileWall= pw.PileWall(pileSection= pileSection, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepths= [L1], pileSpacing= 1.0, waterTableDepth= [None, None])

# Mesh generation
pileWall.genMesh(elemSize= 0.1)

# Solve 
reactionCheckTolerance= 1e-5
# lmsg.setLevel(lmsg.INFO) print excavation level at each step.
pileWall.solve(excavationSide= 'left', reactionCheckTolerance= reactionCheckTolerance, krylov= True, logDepth= False)

# Get results.
results= pileWall.getResultsDict()

outputTable= pw.get_results_table(resultsDict= results)

# Compute maximum bending moment, and maximum and minimum values of soil reaction.
MMin= 6.023e23
MMax= -MMin
RMax= MMax
RMin= MMin
for nodeTag in results:
    nodeResults= results[nodeTag]
    depth= nodeResults['depth']
    M= nodeResults['M']
    MMin= min(MMin, M)
    MMax= max(MMax, M)
    R= nodeResults['Rx']
    RMin= min(RMin, R)
    RMax= max(RMax, R)

elemSize= pileWall.getElemSize()
RMaxRef= (4*Cu-ep.soil.gamma()*H)*elemSize
ratio1= abs(RMax-RMaxRef)/RMaxRef

'''
print('MMax= ', MMax/1e3, 'kN.m')
print('MMin= ', MMin/1e3, 'kN.m')
print('RMax= ', RMax/1e3, 'kN')
print('RMaxRef= ', RMaxRef/1e3, 'kN')
print('ratio1= ', ratio1)
'''

# Matplotlib output.
# pw.plot_results(resultsDict= results, title= 'Test based on the example 3 of the notes of Abhash Acharya.')

# # VTK Graphic output.
# from postprocess import output_handler
# oh= output_handler.OutputHandler(pileWall.modelSpace)
# # oh.displayFEMesh(setsToDisplay= [pileSet])
# #oh.displayLocalAxes()
# #oh.displayLoads()
# oh.displayReactions(reactionCheckTolerance= reactionCheckTolerance)
# oh.displayDispRot('uX', defFScale= 10.0)
# oh.displayIntForcDiag('M')
# oh.displayIntForcDiag('V')

import os
fname= os.path.basename(__file__)
if abs(ratio1)<.01:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

