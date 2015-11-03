# -*- coding: utf-8 -*-

import xc_base
import geom
import xc
import math
import os

abcisas= [0.0,5.0,10.0,15.0,20.0,25.0]
numPorticos= len(abcisas)
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor

puntos= preprocessor.getCad.getPoints
nets= preprocessor.getCad.get3DNets
porticoLateral= nets.new3DNet()
porticoLateral.dim(1,2,numPorticos)
ordenada= 8.2
centroideTeor= geom.Pos3d(abcisas[numPorticos-1]/2.0, 0, ordenada/2.0)
i= 1
for x in abcisas:
  pntA= puntos.newPntFromPos3d(geom.Pos3d(x,0.0,0.0))
  porticoLateral.setPnt(1,1,i,pntA.tag)
  pntB= puntos.newPntFromPos3d(geom.Pos3d(x,0.0,ordenada))
  porticoLateral.setPnt(1,2,i,pntB.tag)
  i+= 1

centroide= porticoLateral.getCentroid()


ratio1= centroideTeor.distPos3d(centroide)

''' 
print "centroide= ",centroide
print "centroideTeor= ",centroideTeor
print "ratio1= ",ratio1
   '''

import os
fname= os.path.basename(__file__)
if(ratio1<1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
