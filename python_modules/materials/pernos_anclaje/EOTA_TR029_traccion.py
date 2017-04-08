# -*- coding: utf-8 -*-

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
import math

def axialResistanceSteelFailure(As, fuk):
  '''Valor característico de la resistencia de un perno
   a tracción por fallo del acero según el artículo 5.2.2.2 de EOTA TR029.
   As: Área de la sección transversal del perno (m2).
   fuk: Valor característico de la carga de rotura a tracción (Pa).
  '''
  return As*fuk

def axialInitialResistancePullOut(d, hEf, tauRk):
    ''' Valor inicial de la resistencia de un perno a tracción por fallo combinado de arrancamiento y extracción de cono de hormigón según la expresión 5.2a del artículo 5.2.2.3 a) de EOTA TR029.

    :param d: Anchor diameter (m).
    :param hEf: Profundidad efectiva del anclaje (m).
    :param tauRk: Valor característico de la tensión de adherencia (debe tomarse del documento ETA que resulte aplicable) (Pa).
    '''   
    return math.pi*d*hEf*tauRk

def axialInitialResistanceConeFailure(k1, fck_cube, hEf):
  ''' Valor inicial de la resistencia de un perno a tracción por
   extracción de cono de hormigón según la expresión 5.3a del artículo 5.2.2.4 a) de EOTA TR029.
   :param k1: 7.2 para hormigón fisurado y 10.1 para hormigón no fisurado.
   :param fck_cube: Resistencia característica del hormigón medida en probeta cúbica (Pa).
   :param hEf: Profundidad efectiva del anclaje (m).
   '''
  return k1*math.sqrt(fck_cube/1e6)*pow(hEf*1e3,1.5)

def getScrNp(d, hEf, tauRkUcr):
  ''' Lado del área de influencia (distancia crítica entre pernos) de un anclaje individual según el artículo 5.2.2.3 b) (ecuación 5.2c) de EOTA TR029.

   :param d: Diámetro del perno (m).
   :param hEf: Profundidad efectiva del anclaje (m).
   :param tauRkUcr: Valor característico de la tensión de adherencia con hormigón no fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
  '''   
  return min(20e3*d*math.sqrt(tauRkUcr/7.5e6),3*hEf)

def getCcrNp(d, hEf, tauRkUcr):
  '''  Semi-lado del área de influencia (distancia crítica al borde) de un anclaje individual según el artículo 5.2.2.3 b) (ecuación 5.2d) de EOTA TR029.

   :param d: Diámetro del perno (m).
   :param hEf: Profundidad efectiva del anclaje (m).
   :param tauRkUcr: Valor característico de la tensión de adherencia con hormigón no fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
  '''   
  return getScrNp(d,hEf,tauRkUcr)/2

def getA0pN(d,posPerno, hEf, tauRkUcr):
    '''
     Polígono que representa el área de influencia de un anclaje individual según el artículo 5.2.2.3 b) (figura 5.1) de EOTA TR029.

     :param d: Diámetro del perno (m).
     :param posPerno: Posición del del perno.
     :param hEf: Profundidad efectiva del anclaje (m).
     :param tauRkUcr: Valor característico de la tensión de adherencia con hormigón no fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
    '''   
    semiLadoA0pN= getCcrNp(d,hEf,tauRkUcr)
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0pN,posPerno.y-semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0pN,posPerno.y-semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0pN,posPerno.y+semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0pN,posPerno.y+semiLadoA0pN))
    return retval

def getFactor2pN(A0pN, ApN):
    '''
    Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte según el artículo 5.2.2.3 b).

    :param A0pN: Área de influencia del anclaje (o grupo de anclajes).
    :param ApN: Area of the intersection of the anchor influence area and the concrete support contour.
    '''
    return ApN/A0pN

def getFactor1N(C, CcrN):
    '''
    Factor que introduce en el cálculo la influencia en la distribución de tensiones de la distancia al borde de la pieza soporte, según la expresión 5.2e del artículo 5.2.2.3 c).

     :param C: Distancia al borde del perno.
     :param CcrN: Semi-lado del área de influencia (distancia crítica al borde).
    '''
    return min(0.7+0.3*C/CcrN,1)

def getScrN(hEf):
  '''
  Lado del área de influencia de un anclaje individual
   según el artículo 5.2.2.4 b) (ecuación 5.3b) del
   artículo 5.2.2.4 b) de EOTA TR029.

   :param hEf: Profundidad efectiva del anclaje (m).
  '''
  return 3*hEf

def getA0cN(posPerno, hEf):
    '''
    Polígono que representa el área de influencia de un anclaje individual según el artículo 5.2.2.4 b) (figura 5.4a) de EOTA TR029.

    :param posPerno: Posición del del perno.
    :param hEf: Profundidad efectiva del anclaje (m).
    '''
    semiLadoA0cN= getScrN(hEf)/2
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0cN,posPerno.y-semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0cN,posPerno.y-semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0cN,posPerno.y+semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0cN,posPerno.y+semiLadoA0cN))
    return retval

def getFactor2cN(A0cN, AcN):
  '''
  Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte según el artículo 5.2.2.4.

  :param A0cN: Área de influencia del anclaje (o grupo de anclajes).
  :param AcN: Area of the intersection of the anchor influence area and the concrete support contour.
  '''
  return AcN/A0cN

# Splitting

def getCcrSpHiltiHY150(h, hEf):
  '''
  Semi-lado del área de influencia (distancia crítica al borde)
     de un anclaje individual según el artículo
     la tabla 7 de la ETA-05/0051 (página 19).

   :param h: Profundidad de la pieza (m).
   :param hEf: Profundidad efectiva del anclaje (m).
  '''
  if(h>=2*hEf):
    return hef 
  elif(h>1.3*hEf):
    return 4.6*hEf-1.8*h 
  else:
    return 2.26*hEf

def getA0spN(posPerno, CcrSp):
    '''
    Polígono que representa el área de influencia de un anclaje individual
    según el artículo 5.2.2.6 b) de EOTA TR029.

    :param posPerno: Posición del del perno.
    :param CcrSp: Distancia al borde crítica por splitting (m).
    '''
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-CcrSp,posPerno.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x+CcrSp,posPerno.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x+CcrSp,posPerno.y+CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x-CcrSp,posPerno.y+CcrSp))
    return retval

def getFactor2spN(A0spN, AspN):
  ''' Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte según el artículo 5.2.2.6 b).

   :param A0spN: Área de influencia del anclaje (o grupo de anclajes).
   :param AspN: Area of the intersection of the anchor influence area and the concrete support contour.
  '''
  return AspN/A0spN
