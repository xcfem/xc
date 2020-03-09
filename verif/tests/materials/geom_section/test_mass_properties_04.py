# -*- coding: utf-8 -*-
'''Example from the page 258 of the book "Hormiǵon Armado. Jiménez Montoya (14a. edición) (url={https://books.google.ch/books?id=JWggPwAACAAJ}).
'''
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
n= 7.365
Ec= Es/n
nuc= 0.3
Gc= Ec/(2*(1+nuc))
b= 1.0 # Effective width of flange.
bw= 0.25 # Width of the web.
hf= 0.08 # Flange thickness.
d= 0.57 # Effective depth.
diamBar= 22e-3 # Rebar diameter.
areaBar= math.pi*(diamBar/2)**2 # Rebars area.

feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
concrete= typical_materials.defElasticMaterial(preprocessor, 'concrete',Ec)
steel= typical_materials.defElasticMaterial(preprocessor, "steel",Es)
sectionGeometryTest= preprocessor.getMaterialHandler.newSectionGeometry("sectionGeometryTest")
regions= sectionGeometryTest.getRegions

flange= regions.newQuadRegion('concrete')# Flange
flange.pMin= geom.Pos2d(d-hf,0.0)
flange.pMax= geom.Pos2d(d,b)
web= regions.newQuadRegion('concrete')# Web
web.pMin= geom.Pos2d(0.0,b/2-bw/2)
web.pMax= geom.Pos2d(d-hf,b/2+bw/2)

reinforcement= sectionGeometryTest.getReinfLayers
reinforcementA= reinforcement.newStraightReinfLayer("steel")
reinforcementA.numReinfBars= 5
reinforcementA.barArea= areaBar
reinforcementA.p1= geom.Pos2d(0.0,b/2-bw/2+0.05)
reinforcementA.p2= geom.Pos2d(0.0,b/2+bw/2-0.05)

elasticSection= typical_materials.defElasticSection3d(preprocessor, "elasticSection",0.0,Ec,Gc,0.0,0.0,1.0)
elasticSection.sectionGeometry("sectionGeometryTest")
sectionProperties= elasticSection.sectionProperties
area= sectionProperties.A
Iy= sectionProperties.Iy
Iz= sectionProperties.Iz

areaTeor= b*hf+(d-hf)*bw+n*5*areaBar
izTeor= 0.0081
iyTeor= 0.0073

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((Iz-izTeor)/izTeor)
ratio3= ((Iy-iyTeor)/iyTeor)

''' 
print "area= ",area
print "areaTeor= ",areaTeor
print "Iy= ",Iy
print "iyTeor= ",iyTeor
print "Iz= ",Iz
print "izTeor= ",izTeor
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
 '''

import os
from miscUtils import log_messages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-2) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
