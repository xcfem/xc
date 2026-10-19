# -*- coding: utf-8 -*-
''' Dummy test of a ey_basic material adapted to represent the friction of a
    shallow foundation with the soil that supports it.

    The pressure in the contact is considered constant, so it does not represent
    accurately the situations where this pressure varies varies significantly
    during the analysis.
'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import xc
from model import predefined_spaces
from materials import soil_structure_interaction as ssi
from actions.quake import ground_motion_utils as gmu

# Define soil model.
phi= math.pi/6.0
kv= 1e9 # Vertical stiffness (N/m)
kh= 0.1*kv # Horizontal stiffness (N/m)

# Define the tributary area corresponding to the spring simulating
# the soil friction.
verticalPressure= 200e3 # N/m2
tributaryArea= 1.5 # 1.5 m2
verticalForce= verticalPressure*tributaryArea

# FE problem definition
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
## Problem type
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.SolidMechanics1D(nodes)

## Nonlinear spring material.
# The pressure in the contact is considered constant, so it does not represent
# accurately the situations where this pressure varies varies significantly
# during the analysis.
yieldForce= verticalForce*math.tan(phi) # constant.

frictionMaterial= ssi.def_shallow_foundation_friction_material(preprocessor= preprocessor, matName= 'frictionMaterial', initialStiffness= kh, yieldForce= yieldForce)

# Create strains loop.
displacements= list()
for i in range(0, 200):
    displacements.append(i/1e5)
tmp= list(reversed(displacements))[1:]
displacements.extend(tmp)
tmp= [-s for s in displacements][1:]
displacements.extend(tmp)

uXExtrema= gmu.MinMaxTracker()
fXExtrema= gmu.MinMaxTracker()
forces= list()
for i, disp in enumerate(displacements):
    t= float(i)
    uXExtrema.update(disp, t, 0)
    frictionMaterial.setTrialStrain(disp, 0.0)
    frictionMaterial.commitState()
    force= frictionMaterial.getStress()
    fXExtrema.update(force, t, 0)
    forces.append(force)

uXmin= uXExtrema.min
err= (uXmin--0.00199)**2
tUXmin= uXExtrema.t_min
err+= (tUXmin-597)**2
uXmax= uXExtrema.max
err+= (uXmax-0.00199)**2
tUXmax= uXExtrema.t_max
err+= (tUXmax-199)**2
fXmin= fXExtrema.min
err+= (fXmin--yieldForce)**2
tFXmin= fXExtrema.t_min
err+= (tFXmin-546)**2
fXmax= fXExtrema.max
err+= (fXmax-yieldForce)**2
tFXmax= fXExtrema.t_max
err+= (tFXmax-174)**2
err= math.sqrt(err)

'''
print('Minimum displacement: ', uXmin, ' at time: ', tUXmin)
print('Maximum displacement: ', uXmax, ' at time: ', tUXmax)
print('Minimum soil friction: ', fXmin, ' at time: ', tFXmin)
print('Maximum soil friction: ', fXmax, ' at time: ', tFXmax)
print('err= ', err)
'''

# #  FIGURES & REPORTS
# import matplotlib.pyplot as plt
# plt.title("Force-displacement diagram")
# plt.xlabel('D (mm)')
# plt.ylabel('F (kN)')
# plt.plot([d*1e3 for d in displacements], [f * 1e-3 for f in forces])
# plt.grid()
# plt.show()

import os
fname= os.path.basename(__file__)
if(err<1e-12):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
