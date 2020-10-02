# -*- coding: utf-8 -*-
from __future__ import print_function
from __future__ import division

import xc_base
import geom
import xc
import math
import os
from model import predefined_spaces
from materials import typical_materials
#from postprocess import output_handler

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

R= 2.0
cos45= math.cos(math.radians(45))
sin45= cos45
E= 30e6 # Young modulus (psi)
nu= 0.3 # Poisson's ratio
rho= 0.0 # Density

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeHandler
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)

elast= typical_materials.defElasticMaterial(preprocessor, "elast",3000)
elasticMembranePlateSectionTest= typical_materials.defElasticMembranePlateSection(preprocessor, "elasticMembranePlateSectionTest",E,nu,rho,0.25)

seedElemHandler= preprocessor.getElementHandler.seedElemHandler
seedElemHandler.defaultMaterial= elasticMembranePlateSectionTest.name
elem= seedElemHandler.newElement("ShellMITC4",xc.ID([0,0,0,0]))

pt1= modelSpace.newKPoint(R,0.0,0.0)
pt2= modelSpace.newKPoint((R*cos45),(R*sin45),0.0)
pt3= modelSpace.newKPoint(0.0,R,0.0)
pt4= modelSpace.newKPoint(R,0.0,0.5)
pt5= modelSpace.newKPoint((R*cos45),(R*sin45),0.5)
pt6= modelSpace.newKPoint(0.0,R,0.5)
pt7= modelSpace.newKPoint(R,0.0,1.0)
pt8= modelSpace.newKPoint((R*cos45),(R*sin45),1.0)
pt9= modelSpace.newKPoint(0.0,R,1.0)

surfaces= preprocessor.getMultiBlockTopology.getSurfaces
s1= surfaces.newQuadSurfaceGridPts([[pt1.tag,pt2.tag,pt3.tag],[pt4.tag,pt5.tag,pt6.tag]])
s1.nDivI= 5; s1.nDivJ= 1

s2= surfaces.newQuadSurfaceGridPts([[pt4.tag,pt5.tag,pt6.tag],[pt7.tag,pt8.tag,pt9.tag]])
s2.nDivI= 5; s2.nDivJ= 1

xcTotalSet= modelSpace.getTotalSet()

feProblem.setVerbosityLevel(0) #Dont print(warning messages about element seed.)
for s in xcTotalSet.surfaces:
    s.genMesh(xc.meshDir.I)
feProblem.setVerbosityLevel(1) #print(warnings again )


nNodes= preprocessor.getSets.getSet("total").getNodes.size

ratio1= abs(nNodes-18)/18.0

''' 
print("nNod= ",nNodes)
print("ratio1= ",ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if(ratio1<=5e-15):
  print("test ",fname,": ok.")
else:
  lmsg.error(fname+' ERROR.')
  
# Graphic stuff.
#oh= output_handler.OutputHandler(modelSpace)
#oh.displayBlocks()
#oh.displayFEMesh()
#oh.displayLocalAxes()
