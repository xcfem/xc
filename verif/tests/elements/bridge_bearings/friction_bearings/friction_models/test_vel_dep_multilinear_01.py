#!/usr/bin/env python
''' Dumb test of the VelDepMultiLinear friction model.

see https://opensees.berkeley.edu/wiki/index.php/Multi-Linear_Velocity_Dependent_Friction
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2026, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import uuid
import xc
from model import friction_models as fm


# Define FE problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

# Define a multi-linear velocity dependent friction model
velocities= [0.0, 0.1, 2.0, 8.0, 10.0]
frictions= [0.163, 0.085, 0.150, 0.163, 0.163]

vf_points= list()
for v, f in zip(velocities, frictions):
    vf_points.append((v, f))
    

frictionModel= fm.def_velocity_dependent_multi_linear_friction_model(preprocessor, name= 'None', vf_points= vf_points)

# Check results.
vf_pts= frictionModel.velocityFrictionPoints

err= 0.0
for vf, vfR in zip(vf_pts, vf_points):
    err+= (vf[0]-vfR[0])**2
    err+= (vf[1]-vfR[1])**2
err= math.sqrt(err)

# print(vf_pts)


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (err<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

