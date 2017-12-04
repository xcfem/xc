# -*- coding: utf-8 -*-
# nverborrea= 0
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

points= preprocessor.getCad.getPoints
pt= points.newPntIDPos3d(1,geom.Pos3d(0.0,0.0,0.0))
pt= points.newPntIDPos3d(2,geom.Pos3d(CooMax,CooMax,CooMax))
points.defaultTag= 100

lines= preprocessor.getCad.getLines
lines.defaultTag= 1
l1= lines.newLine(1,2)
l1.nDiv= NumDiv
l1.divide()

trfs= preprocessor.getCad.getGeometricTransformations
transl= trfs.newTransformation("translation")
transl.setVector(geom.Vector3d(dx,dy,dz))

# Movemos todo
setTotal= preprocessor.getSets.getSet("total")
setTotal.transforms(transl)

''' 
pnts

for_each
  { print codigo,",",pos.x,",",pos.y,",",pos.z } 
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
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
