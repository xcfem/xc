# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


pOld= geom.Pos3d(4.0,4.0,4.0)
zAxis= geom.Line3d(geom.Pos3d(0.0,0.0,0.0), geom.Pos3d(0.0,0.0,100.0))
rot= xc.Rotation(geom.Rotation3d(zAxis,math.pi))

pNew= rot.getTrfPos(pOld)

ratio1= math.sqrt((pNew.x+pOld.x)**2+(pNew.y+pOld.y)**2+(pNew.z-pOld.z)**2)

#print(pOld, pNew, ratio1)

import os
fname= os.path.basename(__file__)
if(ratio1<1e-15):
    print("test ",fname,": ok.")
else:
    print("test ",fname,": ERROR.")
 
