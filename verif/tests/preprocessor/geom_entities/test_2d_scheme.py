# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import math
import os

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

abscissae= [0.0,5.0,10.0,15.0,20.0,25.0]
numPorticos= len(abscissae)
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor

points= preprocessor.getMultiBlockTopology.getPoints
nets= preprocessor.getMultiBlockTopology.get2DNets
porticoLateral= nets.new2DNet()
porticoLateral.dim(2,numPorticos)
ordenada= 8.2
centroideTeor= geom.Pos3d(abscissae[numPorticos-1]/2.0, 0, ordenada/2.0)
i= 1
for x in abscissae:
  pntA= points.newPntFromPos3d(geom.Pos3d(x,0.0,0.0))
  porticoLateral.setPnt(1,i,pntA.tag)
  pntB= points.newPntFromPos3d(geom.Pos3d(x,0.0,ordenada))
  porticoLateral.setPnt(2,i,pntB.tag)
  i+= 1

centroide= porticoLateral.getCentroid()


ratio1= centroideTeor.dist(centroide)

''' 
print("centroide= ",centroide)
print("centroideTeor= ",centroideTeor)
print("ratio1= ",ratio1)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
