# -*- coding: utf-8 -*-
''' Example taken from the page 258 of the book "Hormiǵon Armado. Jiménez Montoya (14a. edición) (url={https://books.google.ch/books?id=JWggPwAACAAJ}).
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


area= sectionGeometryTest.getAreaHomogenizedSection(Ec)
G= sectionGeometryTest.getCdgHomogenizedSection(Ec)
Iy= sectionGeometryTest.getIyHomogenizedSection(Ec)
Iz= sectionGeometryTest.getIzHomogenizedSection(Ec)




areaTeor= b*hf+(d-hf)*bw+n*5*areaBar
yG= G[0]
zG= G[1]
izTeor= 0.0081
iyTeor= 0.0073
zGTeor= 0.5
yGTeor= 0.57-0.24

ratio1= ((area-areaTeor)/areaTeor)
ratio2= ((Iz-izTeor)/izTeor)
ratio3= ((Iy-iyTeor)/iyTeor)
ratio4= ((zG-zGTeor)/zGTeor)
ratio5= ((yG-yGTeor)/yGTeor)

''' 
print "area= ",area
print "areaTeor= ",areaTeor
print "yG= ",yG
print "yGTeor= ",yGTeor
print "zG= ",zG
print "Iy= ",Iy
print "iyTeor= ",iyTeor
print "Iz= ",Iz
print "izTeor= ",izTeor
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
print "ratio5= ",ratio5
 '''

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-15) & (abs(ratio2)<1e-4) & (abs(ratio3)<1e-2) & (abs(ratio4)<1e-15) &  (abs(ratio5)<2e-2) :
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

