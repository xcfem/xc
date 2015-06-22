# -*- coding: utf-8 -*-
''' Verifica la formación de la matriz de rigidez tangente de la sección.
   Elaboración propia. '''
import xc_base
import geom
import xc
from model import predefined_spaces
from solution import predefined_solutions
from materials import typical_materials

ancho= 0.2 # Ancho de la sección expresado en metros.
canto= 0.4 # Ancho de la sección expresado en metros.
E= 2.1e6 # Módulo de Young del material en kp/cm2.
prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Definimos materiales

ela= typical_materials.defElasticMaterial(mdlr,"ela",E)
geomScc= mdlr.getMaterialLoader.newSectionGeometry("geomScc")
regiones= geomScc.getRegions
regEla= regiones.newQuadRegion("ela")
regEla.nDivIJ= 11
regEla.nDivJK= 11
regEla.pMin= geom.Pos2d(-canto/2.0,-ancho/2.0)
regEla.pMax= geom.Pos2d(canto/2.0,ancho/2.0)

scc= mdlr.getMaterialLoader.newMaterial("fiber_section_3d","scc")
fiberSectionRepr= scc.getFiberSectionRepr()
fiberSectionRepr.setGeomNamed("geomScc")
scc.setupFibers()

ts= scc.getTangentStiffness()
EIz= ts(1,1)
EIy= ts(2,2)
EA= ts(0,0)



EIzTeor= 1/12.0*ancho*canto**3*E
ratio1= (EIz-EIzTeor)/EIzTeor
EIyTeor= 1/12.0*canto*ancho**3*E
ratio2= (EIy-EIyTeor)/EIyTeor
EATeor= ancho*canto*E
ratio3= (EA-EATeor)/EATeor

# print "ratio1= ", ratio1
# print "ratio2= ", ratio2
# print "EATeor= ", EATeor
# print "EA= ", EA
# print "ratio3= ", ratio3

import os
fname= os.path.basename(__file__)
if (abs(ratio1)<1e-2) & (abs(ratio2)<1e-2) & (abs(ratio3)<1e-6):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
