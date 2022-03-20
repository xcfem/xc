# -*- coding: utf-8 -*-
''' Home made test.'''

from __future__ import print_function
'''«getPointLocalCoordFromGlobal» function verification.'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from model import predefined_spaces
import math
from materials import typical_materials


L= 20 # Bar length.

# Problem type

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor   
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
n1= nodes.newNodeXY(0,0)
n2= nodes.newNodeXY(L*math.sqrt(2)/2,L*math.sqrt(2)/2)

    
# Geometric transformations
lin= modelSpace.newLinearCrdTransf("lin")
# Materials
section= typical_materials.defElasticSection2d(preprocessor, "section",1,1,1)

# Elements definition
elements= preprocessor.getElementHandler
elements.defaultTransformation= lin.name
elements.defaultMaterial= section.name
#  sintaxis: beam2d_02[<tag>] 
beam2d= elements.newElement("ElasticBeam2d",xc.ID([n1.tag,n2.tag]))
coordTransf= beam2d.getCoordTransf
centroideG= coordTransf.getPointGlobalCoordFromBasic(0.5)
posCentroideG= geom.Pos2d(centroideG[0],centroideG[1])
centroideL= coordTransf.getPointLocalCoordFromGlobal(centroideG)
posCentroideL= geom.Pos2d(centroideL[0],centroideL[1])
centroideB= coordTransf.getPointBasicCoordFromGlobal(centroideG)


centroideGTeor= geom.Pos2d(L*math.sqrt(2)/4,L*math.sqrt(2)/4) # global coordinates of the centroid.
ratio1= posCentroideG.dist(centroideGTeor)
centroideLTeor= geom.Pos2d(10,0) # local coordinates of the centroid.
ratio2= posCentroideL.dist(centroideLTeor)
centroideBTeor= 0.5 # Basic coordinates of the centroid.
ratio3= abs(centroideB-centroideBTeor)

''' 
print("global coordinates of the centroid: ",centroideG)
print("ratio1: ",ratio1)
print("local coordinates of the centroid: ",centroideL)
print("ratio2: ",ratio2)
print("basic coordinates of the centroid: ",centroideB)
print("ratio3: ",ratio3)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if((ratio1<1e-15) & (ratio2<1e-14) & (ratio3<1e-15)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
