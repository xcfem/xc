# -*- coding: utf-8 -*-
''' Test integrator options. Home made test.'''
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc

# Create FE problem.
feProblem= xc.FEProblem()
prep= feProblem.getPreprocessor

# Get current number of integrators.
numIntegratorsA= len(prep.getBeamIntegratorHandler)

# Define a bunch of integrators.
lobattoBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('Lobatto','Lobatto01')
legendreBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('Legendre','Legendre01')
radauBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('Radau','Radau01')
newtonCotesBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('NewtonCotes','NewtonCotes01')
trapezoidalBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('Trapezoidal','Trapezoidal01')
compositeSimpsonBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('CompositeSimpson','CompositeSimpson01')
userDefinedBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('UserDefined','UserDefined01')
fixedLocationBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('FixedLocation','FixedLocation01')
lowOrderBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('LowOrder','LowOrder01')
midDistanceBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('MidDistance','MidDistance01')
hingeMidpointBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('HingeMidpoint','HingeMidpoint01')
hingeRadauBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('HingeRadau','HingeRadau01')
hingeRadauTwoBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('HingeRadauTwo','HingeRadauTwo01')
hingeEndpointBeamIntegration= prep.getBeamIntegratorHandler.newBeamIntegrator('HingeEndpoint','HingeEndpoint01')

# Get current number of integrators.
numIntegratorsB= len(prep.getBeamIntegratorHandler)
ratio1= numIntegratorsB-14-numIntegratorsA

# print('numIntegratorsA= ', numIntegratorsA)
# print('numIntegratorsB= ', numIntegratorsB)
# print('ratio1= ', ratio1)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1==0):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
