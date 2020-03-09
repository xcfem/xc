# -*- coding: utf-8 -*-
# Home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc

feProblem= xc.FEProblem()
prep= feProblem.getPreprocessor

numIntegratorsA= len(prep.getBeamIntegratorHandler)

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

numIntegratorsB= len(prep.getBeamIntegratorHandler)
ratio1= numIntegratorsB-14-numIntegratorsA

#print 'numIntegrators= ', numIntegrators

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
