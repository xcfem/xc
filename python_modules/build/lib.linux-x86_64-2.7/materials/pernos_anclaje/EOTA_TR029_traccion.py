# -*- coding: utf-8 -*-

import xc_base
import geom
import math

'''
Valor característico de la resistencia de un perno
   a tracción por fallo del acero según el artículo 5.2.2.2 de EOTA TR029.
   As: Área de la sección transversal del perno (m2).
   fuk: Valor característico de la carga de rotura a tracción (Pa).
'''
def axialResistanceSteelFailure(As, fuk):
    return As*fuk

'''
Valor inicial de la resistencia de un perno a tracción por fallo
   combinado de arrancamiento y extracción de cono de hormigón
   según la expresión 5.2a del artículo 5.2.2.3 a) de EOTA TR029.
   d: Diámetro del perno (m).
   hEf: Profundidad efectiva del anclaje (m).
   tauRk: Valor característico de la tensión de adherencia (debe tomarse del
          documento ETA que resulte aplicable) (Pa).
'''   
def axialInitialResistancePullOut(d, hEf, tauRk):
    return math.pi*d*hEf*tauRk

'''
Valor inicial de la resistencia de un perno a tracción por
   extracción de cono de hormigón según la expresión 5.3a del artículo 5.2.2.4 a)
   de EOTA TR029.
   k1: 7.2 para hormigón fisurado y 10.1 para hormigón no fisurado.
   fck_cube: Resistencia característica del hormigón medida en probeta cúbica (Pa).
   hEf: Profundidad efectiva del anclaje (m).
'''
def axialInitialResistanceConeFailure(k1, fck_cube, hEf):
    return k1*math.sqrt(fck_cube/1e6)*pow(hEf*1e3,1.5)

'''
Lado del área de influencia (distancia crítica entre pernos) de un anclaje individual
   según el artículo 5.2.2.3 b) (ecuación 5.2c) de EOTA TR029.
   d: Diámetro del perno (m).
   hEf: Profundidad efectiva del anclaje (m).
   tauRkUcr: Valor característico de la tensión de adherencia con hormigón no
             fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
'''   
def getScrNp(d, hEf, tauRkUcr):
    return min(20e3*d*math.sqrt(tauRkUcr/7.5e6),3*hEf)

'''
Semi-lado del área de influencia (distancia crítica al borde)
   de un anclaje individual según el artículo
   5.2.2.3 b) (ecuación 5.2d) de EOTA TR029.

   d: Diámetro del perno (m).
   hEf: Profundidad efectiva del anclaje (m).
   tauRkUcr: Valor característico de la tensión de adherencia con hormigón no
             fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
'''   
def getCcrNp(d, hEf, tauRkUcr):
    return getScrNp(d,hEf,tauRkUcr)/2

'''
Polígono que representa el área de influencia de un anclaje individual
   según el artículo 5.2.2.3 b) (figura 5.1) de EOTA TR029.
   d: Diámetro del perno (m).
   posPerno: Posición del del perno.
   hEf: Profundidad efectiva del anclaje (m).
   tauRkUcr: Valor característico de la tensión de adherencia con hormigón no
             fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
'''   
def getA0pN(d,posPerno, hEf, tauRkUcr):
    semiLadoA0pN= getCcrNp(d,hEf,tauRkUcr)
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0pN,posPerno.y-semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0pN,posPerno.y-semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0pN,posPerno.y+semiLadoA0pN))
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0pN,posPerno.y+semiLadoA0pN))
    return retval

'''
Factor que introduce en el cálculo la influencia
   de la distancia al borde de la pieza soporte según el artículo 5.2.2.3 b).
   A0pN: Área de influencia del anclaje (o grupo de anclajes).
   ApN: Área de la intersección del área de influencia del anclaje con el
        contorno de la pieza soporte.
'''
def getFactor2pN(A0pN, ApN):
    return ApN/A0pN

'''
Factor que introduce en el cálculo la influencia
   en la distribución de tensiones de la distancia al borde
   de la pieza soporte, según la expresión 5.2e del artículo 5.2.2.3 c).
   C: Distancia al borde del perno.
   CcrN: Semi-lado del área de influencia (distancia crítica al borde).
'''
def getFactor1N(C, CcrN):
    return min(0.7+0.3*C/CcrN,1)

'''
Lado del área de influencia de un anclaje individual
   según el artículo 5.2.2.4 b) (ecuación 5.3b) del
   artículo 5.2.2.4 b) de EOTA TR029.
   hEf: Profundidad efectiva del anclaje (m).
'''
def getScrN(hEf):
    return 3*hEf

'''
Polígono que representa el área de influencia de un anclaje individual
   según el artículo 5.2.2.4 b) (figura 5.4a) de EOTA TR029.
   posPerno: Posición del del perno.
   hEf: Profundidad efectiva del anclaje (m).
'''
def getA0cN(posPerno, hEf):
    semiLadoA0cN= getScrN(hEf)/2
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0cN,posPerno.y-semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0cN,posPerno.y-semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x+semiLadoA0cN,posPerno.y+semiLadoA0cN))
    retval.agregaVertice(geom.Pos2d(posPerno.x-semiLadoA0cN,posPerno.y+semiLadoA0cN))
    return retval

'''
Factor que introduce en el cálculo la influencia
   de la distancia al borde de la pieza soporte según el artículo 5.2.2.4.
   A0cN: Área de influencia del anclaje (o grupo de anclajes).
   AcN: Área de la intersección del área de influencia del anclaje con el
        contorno de la pieza soporte.
'''
def getFactor2cN(A0cN, AcN):
    return AcN/A0cN

# Splitting

'''
Semi-lado del área de influencia (distancia crítica al borde)
   de un anclaje individual según el artículo
   la tabla 7 de la ETA-05/0051 (página 19).

   h: Profundidad de la pieza (m).
   hEf: Profundidad efectiva del anclaje (m).
'''
def getCcrSpHiltiHY150(h, hEf):
  if(h>=2*hEf):
    return hef 
  elif(h>1.3*hEf):
    return 4.6*hEf-1.8*h 
  else:
    return 2.26*hEf

'''
Polígono que representa el área de influencia de un anclaje individual
   según el artículo 5.2.2.6 b) de EOTA TR029.
   posPerno: Posición del del perno.
   CcrSp: Distancia al borde crítica por splitting (m).
'''
def getA0spN(posPerno, CcrSp):
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(posPerno.x-CcrSp,posPerno.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x+CcrSp,posPerno.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x+CcrSp,posPerno.y+CcrSp))
    retval.agregaVertice(geom.Pos2d(posPerno.x-CcrSp,posPerno.y+CcrSp))
    return retval

'''
Factor que introduce en el cálculo la influencia
   de la distancia al borde de la pieza soporte según el artículo 5.2.2.6 b).
   A0spN: Área de influencia del anclaje (o grupo de anclajes).
   AspN: Área de la intersección del área de influencia del anclaje con el
        contorno de la pieza soporte.
'''
def getFactor2spN(A0spN, AspN):
    return AspN/A0spN
