# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
import sys
import math

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador

puntos= mdlr.getCad.getPoints
puntos.defaultTag= 1
pt= puntos.newPntFromPos3d(geom.Pos3d(0,0,0))
pt= puntos.newPntFromPos3d(geom.Pos3d(1,1,1))
pt= puntos.newPntFromPos3d(geom.Pos3d(2,2,2))

setTotal= mdlr.getSets.getSet("total")
ptsSet= setTotal.getPoints
numPoints= ptsSet.size
count= 0
#Point iterator
for p in ptsSet:
  count+= 1

cumple= (count == numPoints)
  

import os
fname= os.path.basename(__file__)
if cumple:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."



