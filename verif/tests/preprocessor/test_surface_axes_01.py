# -*- coding: utf-8 -*-

import xc_base
import geom
import xc

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/borrar.log" # Ignore warning messages
feProblem.errFileName= "/tmp/borrar.err" # Ignore warning messages
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntIDPos3d(2,geom.Pos3d(1.0,0.0,0.0))
pt3= points.newPntIDPos3d(3,geom.Pos3d(1.0,1.0,0.0))
pt4= points.newPntIDPos3d(4,geom.Pos3d(0.0,1.0,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
surfaces.defaultTag= 1
s1= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt3.tag,pt4.tag)

iVector= s1.getIVector
jVector= s1.getJVector
kVector= s1.getKVector
localAxes= s1.getLocalAxes()

ratio= (iVector-geom.Vector3d(1.0,0.0,0.0)).getModulo()**2
ratio+= (jVector-geom.Vector3d(0.0,1.0,0.0)).getModulo()**2
ratio+= (kVector-geom.Vector3d(0.0,0.0,1.0)).getModulo()**2
testMatrix= xc.Matrix([[1,0,0],[0,1,0],[0,0,1]])
ratio+= (localAxes-testMatrix).OneNorm()

'''
print 'iVector= ', iVector
print 'jVector= ', jVector
print 'kVector= ', kVector
print 'localAxes= ', localAxes
print ratio
'''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio<=1e-15) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
