# -*- coding: utf-8 -*-
from __future__ import division 
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc_base
import geom
import xc

from model.geometry import grid_model as gm

FEcase= xc.FEProblem()
preprocessor=FEcase.getPreprocessor

#Test elliptical coordinates in grid-model
#Data
xCent=1 #x center of the elliptical coordinate system
yCent=2 #y center of the elliptical coordinate system
semi_x0=5  # X semi-axis of the initial ellipse
semi_y0=2  # Y semi-axis of the initial ellipse
semi_x1=1  # X semi-axis of the final ellipse
semi_y1=3  # Y semi-axis of the final ellipse


semiAxisXList=[semi_x0] #list of X-semi-axes of ellipses 
angList=[i*22.5 for i in range(16)]  # list of angular coordinates
zList=[0] # list of z coordinates
semiAxisYList=[semi_y0] #list of Y-semi-axes of ellipses 

grid=gm.GridModel(prep=preprocessor,xList=semiAxisXList, yList=angList, zList=zList, xCentCoo=xCent, yCentCoo=yCent, semiYellipList=semiAxisYList)

grid.generateEllipZPoints()
pCent=geom.Pos3d(xCent,yCent,0)

pnt0=grid.getPntXYZ((semi_x0,0,0))
ratio0=pnt0.getPos.dist(pCent)-semi_x0
pnt90=grid.getPntXYZ((semi_x0,90,0))
ratio1=pnt90.getPos.dist(pCent)-semi_y0
pnt180=grid.getPntXYZ((semi_x0,180,0))
ratio2=pnt180.getPos.dist(pCent)-semi_x0
pnt270=grid.getPntXYZ((semi_x0,270,0))
ratio3=pnt270.getPos.dist(pCent)-semi_y0

grid.movePointsEllipse(ijkRange=gm.IJKRange((0,0,0),(0,len(angList)-1,0)), semiAxX=semi_x1, semiAXY=semi_y1)
pnt0=grid.getPntXYZ((semi_x0,0,0))
ratio4=pnt0.getPos.dist(pCent)-semi_x1
pnt90=grid.getPntXYZ((semi_x0,90,0))
ratio5=pnt90.getPos.dist(pCent)-semi_y1
pnt180=grid.getPntXYZ((semi_x0,180,0))
ratio6=pnt180.getPos.dist(pCent)-semi_x1
pnt270=grid.getPntXYZ((semi_x0,270,0))
ratio7=pnt270.getPos.dist(pCent)-semi_y1

ratio8=0
for ang in angList:
    p=grid.getPntXYZ((semi_x0,ang,0)).getPos
    ratio8+=(p.x-xCent)**2/semi_x1**2+(p.y-yCent)**2/semi_y1**2-1

#Checks
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if ((ratio0+ratio1+ratio2+ratio3)<=1e-15) & ((ratio4+ratio5+ratio6+ratio7)<=1e-15) & (ratio8<=1e-12) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
