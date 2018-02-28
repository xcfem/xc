# -*- coding: utf-8 -*-

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"


b= 0.4
h= 0.8
nDivIJ= 2
nDivJK= 2
y0= 0
z0= 0
L= 1 # Element length expressed in meters.

# Problem type
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0)

lin= modelSpace.newLinearCrdTransf("lin",xc.Vector([0,1,0]))

# Materials definition
fy= 275e6 # Yield stress of the steel.
E= 210e9 # Young modulus of the steel.
steel= typical_materials.defSteel01(preprocessor, "steel",E,fy,0.001)

# Secciones
quadFibersGeom= preprocessor.getMaterialLoader.newSectionGeometry("quadFibersGeom")
y1= h/2.0
z1= b/2.0
regiones= quadFibersGeom.getRegions
steel= regiones.newQuadRegion("steel")
steel.nDivIJ= nDivIJ
steel.nDivJK= nDivJK
steel.pMin= geom.Pos2d(y0-y1,z0-z1)
steel.pMax= geom.Pos2d(y0+y1,z0+z1)
quadFibers= preprocessor.getMaterialLoader.newMaterial("fiber_section_3d","quadFibers")
fiberSectionRepr= quadFibers.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("quadFibersGeom")
quadFibers.setupFibers()
fibers= quadFibers.getFibers()
A= fibers.getArea


# Elements definition
elements= preprocessor.getElementLoader
elements.defaultTransformation= "lin"
elements.defaultMaterial= "quadFibers"
beam3d= elements.newElement("ForceBeamColumn3d",xc.ID([1,2]));



fuerte= beam3d.getVDirStrongAxisGlobalCoord(0,True) # initialGeometry= True
debil= beam3d.getVDirWeakAxisGlobalCoord(0,True) # initialGeometry= True
ratio1= ((fuerte[0])**2+(fuerte[2])**2)
ratio2= ((debil[0])**2+(debil[1])**2)


''' 
print "fuerte: ",fuerte
print "debil: ",debil
print "ratio1= ",ratio1
print "ratio2= ",ratio2
   '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

