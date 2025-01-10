# -*- coding: utf-8 -*-
''' Verification of the removeBackwardSegment method.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import geom

def test_remove_backward_vertices(xi):
    ''' Test the  removeBackwardSegment method.

    :param xi: x coordinates of the vertex sequence.
    '''
    points= list()
    for x in xi:
        points.append(geom.Pos2d(x,0))
    pline= geom.Polyline2d(points)
    nv1A= pline.getNumVertices()
    pline.removeBackwardSegments(-0.5)
    nv1B= pline.getNumVertices()
    return nv1A-nv1B
    
removedVertices1= test_remove_backward_vertices([0, 1, -1, 2, 3])

removedVertices2= test_remove_backward_vertices([2, 1, 0, 3, 4, -1, -2, -3, -4, -5])

pol3= geom.Polyline2d([geom.Pos2d(11.0738, 20.218), geom.Pos2d(-22.9508, 15.2622), geom.Pos2d(-54.3391, 9.84483), geom.Pos2d(-54.0169,9.90043)])
nv3A= pol3.getNumVertices()
pol3.logFileName= "/tmp/erase.log" # Ignore warning messages
pol3.removeBackwardSegments(-0.5)
pol3.logFileName= "clog" # Display warnings if any.
nv3B= pol3.getNumVertices()
removedVertices3= nv3A-nv3B

ok= (removedVertices1==1) and ((removedVertices2==2) and (removedVertices3==1))

#print(removedVertices1, removedVertices2, removedVertices3)

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ok:
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')

