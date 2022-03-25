# -*- coding: utf-8 -*-
from __future__ import print_function
# feProblem.setVerbosityLevel(0)
dx= 1
dy= 2
dz= 3
import xc_base
import geom
import xc
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


NumDiv= 4
CooMax= NumDiv
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPoint(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPoint(geom.Pos3d(CooMax,CooMax,CooMax))

lines= preprocessor.getMultiBlockTopology.getLines
l1= lines.newLine(pt1.tag,pt2.tag)
l1.nDiv= NumDiv
points.defaultTag= 100 # Next point will have tag= 100
l1.divide()

transl= xc.Translation(geom.Translation3d(geom.Vector3d(dx,dy,dz)))

# Movemos todo
setTotal= preprocessor.getSets.getSet("total")
setTotal.transforms(transl)

''' 
pnts

for_each
  { print(code,",",pos.x,",",pos.y,",",pos.z } )
'''



cumple= True
pnt101= points.get(101)
pos= pnt101.getPos
cumple= (abs(pos.x-(1.0+dx))<1e-5) & (cumple) 
cumple= (abs(pos.y-(1.0+dy))<1e-5) & (cumple) 
cumple= (abs(pos.z-(1.0+dz))<1e-5) & (cumple) 

pnt102= points.get(102)
pos= pnt102.getPos
cumple= (abs(pos.x-(2.0+dx))<1e-5) & (cumple) 
cumple= (abs(pos.y-(2.0+dy))<1e-5) & (cumple) 
cumple= (abs(pos.z-(2.0+dz))<1e-5) & (cumple) 

pnt103= points.get(103)
pos= pnt103.getPos
cumple= (abs(pos.x-(3.0+dx))<1e-5) & (cumple) 
cumple= (abs(pos.y-(3.0+dy))<1e-5) & (cumple) 
cumple= (abs(pos.z-(3.0+dz))<1e-5) & (cumple) 

pnt104= points.get(104)
pos= pnt104.getPos
cumple= (abs(pos.x-(4.0+dx))<1e-5) & (cumple) 
cumple= (abs(pos.y-(4.0+dy))<1e-5) & (cumple) 
cumple= (abs(pos.z-(4.0+dz))<1e-5) & (cumple) 


import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if cumple:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
