# -*- coding: utf-8 -*-
# home made test
'''«getPointLocalCoordFromGlobal» function verification.'''

from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc
from model import predefined_spaces
from materials import typical_materials
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

L= 20 # Bar length.

# Problem type

centroidG= None # Global coordinates of the centroid.
centroidL= None # Local coordinates of the centroid.
centroidB= None # Basic coordinates of the centroid.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0,0,0)
nod= nodes.newNodeXYZ(L*math.sqrt(3)/3,L*math.sqrt(3)/3,L*math.sqrt(3)/3)

# Materials
section= typical_materials.defElasticSection3d(preprocessor, "section",1,1,1,1,1,1)
    
lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,-1,0]))    
# Elements definition
elements= preprocessor.getElementHandler

elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
elements.defaultTag= 1 #Tag for next element.
beam3d= elements.newElement("ElasticBeam3d",xc.ID([1,2]))

centroidG= beam3d.getCoordTransf.getPointGlobalCoordFromBasic(0.5)
posCentroidG= geom.Pos3d(centroidG[0],centroidG[1],centroidG[2])
centroidL= beam3d.getCoordTransf.getPointLocalCoordFromGlobal(centroidG)
posCentroidL= geom.Pos2d(centroidL[0],centroidL[1])
centroidB= beam3d.getCoordTransf.getPointBasicCoordFromGlobal(centroidG)

centroidGTeor= geom.Pos3d(L*math.sqrt(3)/6,L*math.sqrt(3)/6,L*math.sqrt(3)/6) # Centroid en globales.
ratio1= posCentroidG.distPos3d(centroidGTeor)
centroidLTeor= geom.Pos2d(10,0) # Local coordinates of the centroid.
ratio2= posCentroidL.distPos2d(centroidLTeor)
centroidBTeor= 0.5 # Basic coordinates of the centroid.
ratio3= abs(centroidB-centroidBTeor)

''' 
print("global coordinates of the centroid: ",centroidG)
print("ratio1: ",ratio1)
print("local coordinates of the centroid: ",centroidL)
print("ratio2: ",ratio2)
print("basic coordinates of the centroid: ",centroidB)
print("ratio3: ",ratio3)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-14) & (ratio3<1e-15)):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
