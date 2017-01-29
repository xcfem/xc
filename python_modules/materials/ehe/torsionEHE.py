# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom

'''
Funciones para la comprobación a torsión según el artículo
 45.1 de EHE-08
'''

class TorsionParameters(object):
  '''Torsion parameters'''
  h0= 0.0  # Real wall thickess.
  c= 0.0  # Longitudinal reinforcement concrete cover.

  seccionHuecaEficaz= 0.0 # Cross section contour.
  crossSectionContour= geom.Poligono2d()  # Cross section contour.
  lineaMedia=  geom.Poligono2d() # Polygon defined by the midline of the effective hollow section.
  lineaInt=  geom.Poligono2d() # Polygon defined by the interior contour of the effective hollow section.
  seccionHuecaEficaz= geom.PoligonoConAgujeros2d() # Effective hollow section contour
  def A(self):
    return self.crossSectionContour.getArea()
  def u(self):
    return self.crossSectionContour.getPerimetro()
  def he(self):
    return max(2*self.c,min(self.A()/self.u(),self.h0))
  def Ae(self):
    return self.lineaMedia.getArea()
  def ue(self):
    return self.lineaMedia.getPerimetro()

def calcParamsSeccionHuecaEficaz(geomSeccion, h0, c):
  '''
  Calcula los parámetros de torsión que se deducen
   de la sección hueca eficaz. No es válido si la sección no es convexa.
  gmSectionName: Identificador de la definición geométrica de la sección.
  nmbParamsTorsión: Identificador del registro que contiene los parámetros de cálculo
                de la resistencia a torsión.
  h0: Espesor real de la pared.
  c: Recubrimiento de las reinforcement longitudinales.
  '''
  retval= TorsionParameters()
  retval.h0= h0
  retval.c= c
  retval.crossSectionContour= geomSeccion.getRegionsContour()
  he= retval.he()
  retval.lineaMedia= retval.crossSectionContour.offset(-he/2)
  retval.lineaInt= retval.crossSectionContour.offset(-he)
  retval.seccionHuecaEficaz.contour(retval.crossSectionContour)
  retval.seccionHuecaEficaz.addHole(retval.lineaInt)
  return retval
