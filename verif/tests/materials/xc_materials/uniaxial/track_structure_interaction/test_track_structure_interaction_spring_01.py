# -*- coding: utf-8 -*-
''' Non-linear spring used to study the track-rail interaction (ballast/connection).
'''
import xc
import math
import os
from model import predefined_spaces
from materials import track_structure_interaction

# Material for non-linear springs rail-deck or rail-embankment friction
u0= 2e-3  # (m) displacement between elastic and plastic zones
k= 12e3   # (N/m) resistance of sleeper in ballast (unloaded track), moderate maintenande
# list of tuples defining the (strain, stress) or (displacement, force) points.)
frictionStrainStressCurve=[(u0,k), (1,k)]
# Model definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

frict_mat= track_structure_interaction.def_ballast_friction_material(
    preprocessor= preprocessor,
    u0= u0,
    k= k,
    tributaryLength= 1.0,
    name= 'frict_mat',
    smallSlopeFactor= 1.0) # zero slope in plastic range.

sampleDisplacements= [0.5e-3, 1e-3, 2e-3, 3e-3, 2e-3, 1e-3, 0.5e-3, 0, -0.5e-3, -1e-3, -2e-3, -3e-3, -2e-3, -1e-3, -0.5e-3, 0, 0.5e-3, 1e-3, 1.8e-3]

testDisplacements= [0.0]+sampleDisplacements

# Compute forces.
forces= list()
for sd in testDisplacements:
    frict_mat.setTrialStrain(sd, 0.0)
    frict_mat.commitState()
    force= frict_mat.getStress()
    #err+= (sampleStress-stress)**2
    forces.append(force)

referenceForces= [0, 3000.0000000000005, 6000.0, 12000.0, 12000.0, 6000.0, 0, -3000.0, -6000.0, -9000.0, -12000.0, -12000.0, -12000.0, -6000.0, 0, 3000.0000000000005, 6000.0, 9000.0, 12000.0, 12000.0]

err= 0.0
for f, rf in zip(forces, referenceForces):
    err+= (rf-f)**2
err= math.sqrt(err)

print(err)

from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(err<1e-8):
    print("test "+fname+": ok.")
else:
    lmsg.error(fname+' ERROR.')

# # # FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title(fname)
# plt.xlabel('displacement')
# plt.ylabel('force')
# plt.plot(testDisplacements, forces, color= 'blue')
# plt.show()
