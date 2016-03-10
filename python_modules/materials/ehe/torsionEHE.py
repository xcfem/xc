# -*- coding: utf-8 -*-
from __future__ import division
import xc_base
import geom

'''
Funciones para la comprobación a torsión según el artículo
 45.1 de EHE-08
'''

class TorsionParameters(object):
  """Torsion parameters"""
  h0= 0.0  # Espesor real de la pared.
  c= 0.0  # Recubrimiento de las reinforcement longitudinales.

  seccionHuecaEficaz= 0.0 # Contorno de la sección
  contornoSecc= geom.Poligono2d()  # Contorno de la sección
  lineaMedia=  geom.Poligono2d() # Poligono definido por la línea media de la sección hueca eficaz.
  lineaInt=  geom.Poligono2d() # Poligono definido por el contorno interiorde la sección hueca eficaz.
  seccionHuecaEficaz= geom.PoligonoConAgujeros2d() # Contorno de la sección
  def A(self):
    return self.contornoSecc.getArea()
  def u(self):
    return self.contornoSecc.getPerimetro()
  def he(self):
    return max(2*self.c,min(self.A()/self.u(),self.h0))
  def Ae(self):
    return self.lineaMedia.getArea()
  def ue(self):
    return self.lineaMedia.getPerimetro()

    


'''
Calcula los parámetros de torsión que se deducen
 de la sección hueca eficaz. No es válido si la sección no es convexa.
gmSectionName: Identificador de la definición geométrica de la sección.
nmbParamsTorsión: Identificador del registro que contiene los parámetros de cálculo
                de la resistencia a torsión.
h0: Espesor real de la pared.
c: Recubrimiento de las reinforcement longitudinales.

'''
def calcParamsSeccionHuecaEficaz(geomSeccion, h0, c):
  retval= TorsionParameters()
  retval.h0= h0
  retval.c= c
  retval.contornoSecc= geomSeccion.getContornoRegiones()
  he= retval.he()
  retval.lineaMedia= retval.contornoSecc.offset(-he/2)
  retval.lineaInt= retval.contornoSecc.offset(-he)
  retval.seccionHuecaEficaz.contorno(retval.contornoSecc)
  retval.seccionHuecaEficaz.addHole(retval.lineaInt)
  return retval

