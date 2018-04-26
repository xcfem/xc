# -*- coding: utf-8 -*-
''' Checks the assembling of the tangent stiffness matrix of the section.
   Home made. '''
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

width= 0.2 # Cross section width expressed in meters.
depth= 0.4 # Cross section width expressed in meters.
E= 2.1e6 # Young modulus of the material en kp/cm2.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
# Define materials

ela= typical_materials.defElasticMaterial(preprocessor, "ela",E)

# Section geometry
# setting up
geomScc= preprocessor.getMaterialHandler.newSectionGeometry("geomScc")
#filling with regions
regions= geomScc.getRegions
#generation of a quadrilateral region with the specified size and number of
#divisions for the cells (fibers) generation
regEla= regions.newQuadRegion("ela")
regEla.nDivIJ= 11
regEla.nDivJK= 11
regEla.pMin= geom.Pos2d(-depth/2.0,-width/2.0)
regEla.pMax= geom.Pos2d(depth/2.0,width/2.0)

scc= preprocessor.getMaterialHandler.newMaterial("fiber_section_3d","scc")
fiberSectionRepr= scc.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomScc")
scc.setupFibers()

ts= scc.getTangentStiffness()
EIz= ts(1,1)
EIy= ts(2,2)
EA= ts(0,0)



EIzTeor= 1/12.0*width*depth**3*E
ratio1= (EIz-EIzTeor)/EIzTeor
EIyTeor= 1/12.0*depth*width**3*E
ratio2= (EIy-EIyTeor)/EIyTeor
EATeor= width*depth*E
ratio3= (EA-EATeor)/EATeor

# print "ratio1= ", ratio1
# print "ratio2= ", ratio2
# print "EATeor= ", EATeor
# print "EA= ", EA
# print "ratio3= ", ratio3

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2) & (abs(ratio2)<1e-2) & (abs(ratio3)<1e-6):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
