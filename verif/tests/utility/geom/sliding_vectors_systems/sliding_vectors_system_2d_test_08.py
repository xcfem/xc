# -*- coding: utf-8 -*-
''' Trivial verification test for sliding vector system 
    load distribution.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom
import math

depths= [0, -1, -2, -3, -4, -5, -6, -7, -8, -9]
loadValues= list()
depthEqRef= 0.0 # equilibrium
for d in depths:
    v= 100*d
    loadValues.append(v)
    depthEqRef+= v*d
depthEqRef/=sum(loadValues)

positions= list()
forces= list()
for d, f in zip(depths, loadValues):
    positions.append(geom.Pos2d(0, d))
    forces.append(geom.Vector2d(f, 0))

svs= geom.SlidingVectorsSystem2d()
for p, f in zip(positions, forces):
    svs+= geom.SlidingVector2d(p, f)

R= svs.getResultant()
Rref= geom.Vector2d(sum(loadValues),0.0)
ratio1= (R-Rref).getModulus()/Rref.getModulus()
zeroMomentLine= svs.zeroMomentLine()
depthEq= geom.Line2d(positions[0], positions[-1]).getIntersection(zeroMomentLine)[0].y
ratio2= abs(depthEq-depthEqRef)/abs(depthEqRef)

# Check SVS distribution.
distributedForces= svs.distribute(positions)
err= 0.0
for f1, f2 in zip(forces, distributedForces):
    err+= (f1-f2).getModulus2()
err= math.sqrt(err)

'''
print('load values: ', loadValues)
print('svs= ', svs)
print('R= ', R)
print('ratio1= ', ratio1)
print('zero moment line:', zeroMomentLine)
print('depthEq= ', depthEq)
print('depthEqRef= ', depthEqRef)
print('ratio2= ', ratio2)
print('distributed forces: ', distributedForces)
print('err= ', err)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-6 and abs(ratio2)<1e-6 and err<1e-6:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

