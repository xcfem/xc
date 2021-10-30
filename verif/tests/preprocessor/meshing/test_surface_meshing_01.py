# -*- coding: utf-8 -*-
from __future__ import print_function

import xc_base
import geom
import xc
import math
import os
import sys
from model import predefined_spaces
from materials import typical_materials
# from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

CooMaxX= 2
CooMaxY= 1
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density
old_stderr= sys.stderr

numNodes= 0
numElem= 0
# Problem type
feProblem= xc.FEProblem()
feProblem.logFileName= "/tmp/erase.log" # Ignore warning messages
feProblem.errFileName= "/tmp/erase.err" # Ignore error messages
preprocessor= feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler

modelSpace= predefined_spaces.SolidMechanics2D(nodes)
# Materials definition
elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)



elast2d= typical_materials.defElasticIsotropicPlaneStress(preprocessor, "elast2d",E,nu,rho)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elast2d.name
elem= seedElemHandler.newElement("FourNodeQuad",xc.ID([0,0,0,0]))



points= preprocessor.getMultiBlockTopology.getPoints
pt1= points.newPntFromPos3d(geom.Pos3d(0.0,0.0,0.0))
pt2= points.newPntFromPos3d(geom.Pos3d(CooMaxX/2,0.0,0.0))
pt3= points.newPntFromPos3d(geom.Pos3d(CooMaxX,0.0,0.0))
pt4= points.newPntFromPos3d(geom.Pos3d(0.0,CooMaxY,0.0))
pt5= points.newPntFromPos3d(geom.Pos3d(CooMaxX/2,CooMaxY,0.0))
pt6= points.newPntFromPos3d(geom.Pos3d(CooMaxX,CooMaxY,0.0))

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfacePts(pt1.tag,pt2.tag,pt5.tag,pt4.tag)
s1.nDivI= 1
s1.nDivJ= 1

s1.genMesh(xc.meshDir.I)
total= preprocessor.getSets.getSet("total")




surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s2= surfaces.newQuadSurfacePts(pt2.tag,pt3.tag,pt6.tag,pt5.tag)
s2.nDivI= 2
s2.nDivJ= 2



s2.genMesh(xc.meshDir.I)

numNodes= total.getNodes.size
numElem= total.getElements.size



''' 
print("Number of nodes: ",numNodes)
print("Number of elements: ",numElem)
   '''

ratio1= abs(numNodes-8)
ratio2= abs(numElem-3)

feProblem.errFileName= "cerr" #Display errors if any.
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (ratio1<=1e-10) & (ratio2<=1e-10) :
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
    
#Graphic stuff.
# oh= output_handler.OutputHandler(modelSpace)

# oh.displayBlocks()#setToDisplay= )
# oh.displayFEMesh()#setsToDisplay=[])
# oh.displayLocalAxes()
# oh.displayLoads()
# oh.displayReactions()
# oh.displayDispRot(itemToDisp='uX')
# oh.displayDispRot(itemToDisp='uY')
# oh.displayDispRot(itemToDisp='uZ')
