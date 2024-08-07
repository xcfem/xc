# -*- coding: utf-8 -*-
'''ezdxf module test.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import os
import ezdxf
import geom

def getPolygon(e):
    retval= geom.Polygon2d()
    with e.points('xyseb') as points:
        for p in points:
            retval.appendVertex(geom.Pos2d(p[0], p[1]))
    return retval

pth= os.path.dirname(__file__)
if(not pth):
    pth= "."
dxfFilePath= pth+'/ezdxf_test_01.dxf' 
doc= ezdxf.readfile(dxfFilePath)
msp= doc.modelspace()

# Iterate through entities
area= 0.0
for e in msp:
    if(e.dxftype() == 'LWPOLYLINE'):
        plg= getPolygon(e)
        area+= plg.getArea()

ratio= abs(area-4.70503703524)/4.70503703524

'''
print(area)
'''

fname= os.path.basename(__file__)
if (ratio<1e-5):
    print('test: '+fname+': ok.')
else:
    lmsg.error('test: '+fname+' ERROR.')
