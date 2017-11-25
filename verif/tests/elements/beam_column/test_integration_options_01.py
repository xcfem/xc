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

prb= xc.ProblemaEF()
prep= prb.getPreprocessor

numIntegratorsA= len(prep.getBeamIntegratorLoader)

lobattoBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('Lobatto','Lobatto01')
legendreBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('Legendre','Legendre01')
radauBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('Radau','Radau01')
newtonCotesBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('NewtonCotes','NewtonCotes01')
trapezoidalBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('Trapezoidal','Trapezoidal01')
compositeSimpsonBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('CompositeSimpson','CompositeSimpson01')
userDefinedBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('UserDefined','UserDefined01')
fixedLocationBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('FixedLocation','FixedLocation01')
lowOrderBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('LowOrder','LowOrder01')
midDistanceBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('MidDistance','MidDistance01')
hingeMidpointBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('HingeMidpoint','HingeMidpoint01')
hingeRadauBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('HingeRadau','HingeRadau01')
hingeRadauTwoBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('HingeRadauTwo','HingeRadauTwo01')
hingeEndpointBeamIntegration= prep.getBeamIntegratorLoader.newBeamIntegrator('HingeEndpoint','HingeEndpoint01')

numIntegratorsB= len(prep.getBeamIntegratorLoader)
ratio1= numIntegratorsB-14-numIntegratorsA

#print 'numIntegrators= ', numIntegrators

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-15:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
