# -*- coding: utf-8 -*-
from __future__ import print_function
# Home made test

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

Es= 2.1e11
nu= 0.3
Gs= 0.01*Es/(2*(1+nu))
radius= 1.2/2.0
thickness= 10.3e-3
RExt= radius+thickness

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
steel= typical_materials.defElasticMaterial(preprocessor, "steel",Es)
geomTuberia= preprocessor.getMaterialHandler.newSectionGeometry("geomTuberia")
regions= geomTuberia.getRegions
steelRegion= regions.newCircularRegion("steel")

steelRegion.nDivCirc= 1000
steelRegion.centro= geom.Pos2d(0.0,0.0)
steelRegion.extRad= radius+thickness
steelRegion.intRad= radius
steelRegion.initAngle= 0.0
steelRegion.finalAngle= 2*math.pi

area= steelRegion.getArea()
Iy= steelRegion.getIy()
Iz= steelRegion.getIz()
Pyz= steelRegion.getPyz()


G= geomTuberia.getCenterOfMassHomogenizedSection(Es)

areaTeor= math.pi*(RExt**2-radius**2)
ratio1= ((area-areaTeor)/areaTeor)
ITeor= math.pi/4*(RExt**4-radius**4)
ratio2= ((Iy-ITeor)/ITeor)
ratio3= ((Iz-ITeor)/ITeor)
ratio4= G.Norm()

''' 
print("area= ",area)
print("areaTeor= ",areaTeor)
print("ratio1= ",ratio1)
print("Iy= ",Iy)
print("ITeor= ",ITeor)
print("ratio2= ",ratio2)
print("Iz= ",Iz)
print("ITeor= ",ITeor)
print("ratio3= ",ratio3)
print("G= ",G)
print("ratio4= ",ratio4)
print("Pyz= ",Pyz)
   '''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-5) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-4) & (abs(ratio4)<1e-13) & (abs(Pyz)<1e-15):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
