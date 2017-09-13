# -*- coding: utf-8 -*-
''' Anchor design according to EOTA TR029'''

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

import xc_base
import geom
import math

def axialResistanceSteelFailure(As, fuk):
  '''Characteristic value of anchor bolt strength in tension
   due to steel collapse according to clause 5.2.2.2 of EOTA TR029.

   As: Área de la sección transversal del perno (m2).
   fuk: Valor característico de la carga de rotura a tracción (Pa).
  '''
  return As*fuk

def axialInitialResistancePullOut(d, hEf, tauRk):
    ''' Valor inicial de la resistencia de un perno a tracción por fallo combinado de arrancamiento y extracción de cono de hormigón according to expression 5.2a in clause 5.2.2.3 a) of EOTA TR029.

    :param d: Anchor diameter (m).
    :param hEf: Profundidad efectiva del anclaje (m).
    :param tauRk: Valor característico de la tensión de adherencia (debe tomarse del documento ETA que resulte aplicable) (Pa).
    '''   
    return math.pi*d*hEf*tauRk

def axialInitialResistanceConeFailure(k1, fck_cube, hEf):
  ''' Valor inicial de la resistencia de un perno a tracción por
   extracción de cono de hormigón according to expression 5.3a in clause 5.2.2.4 a) of EOTA TR029.
   :param k1: 7.2 para hormigón fisurado y 10.1 para hormigón no fisurado.
   :param fck_cube: Concrete characteristic strength medida en probeta cúbica (Pa).
   :param hEf: Profundidad efectiva del anclaje (m).
   '''
  return k1*math.sqrt(fck_cube/1e6)*pow(hEf*1e3,1.5)

def getScrNp(d, hEf, tauRkUcr):
  ''' Lado del área de influencia (distancia crítica entre pernos) de un anclaje individual according to clause 5.2.2.3 b) (ecuación 5.2c) of EOTA TR029.

   :param d: Diámetro del perno (m).
   :param hEf: Profundidad efectiva del anclaje (m).
   :param tauRkUcr: Valor característico de la tensión de adherencia con hormigón no fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
  '''   
  return min(20e3*d*math.sqrt(tauRkUcr/7.5e6),3*hEf)

def getCcrNp(d, hEf, tauRkUcr):
  '''  Semi-lado del área de influencia (distancia crítica al borde) de un anclaje individual according to clause 5.2.2.3 b) (ecuación 5.2d) of EOTA TR029.

   :param d: Diámetro del perno (m).
   :param hEf: Profundidad efectiva del anclaje (m).
   :param tauRkUcr: Valor característico de la tensión de adherencia con hormigón no fisurado (debe tomarse del documento ETA que resulte aplicable) (Pa).
  '''   
  return getScrNp(d,hEf,tauRkUcr)/2

def getA0pN(d,posPerno, hEf, tauRkUcr):
    '''
     Polígono que representa el área de influencia de un anclaje individual according to clause 5.2.2.3 b) (figura 5.1) of EOTA TR029.

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
    Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte according to clause 5.2.2.3 b).

    :param A0pN: Área de influencia del anclaje (o grupo de anclajes).
    :param ApN: Area of the intersection of the anchor influence area and the concrete support contour.
    '''
    return ApN/A0pN

def getFactor1N(C, CcrN):
    '''
    Factor que introduce en el cálculo la influencia en la distribución de tensiones de la distancia al borde de la pieza soporte, according to expression 5.2e of clause 5.2.2.3 c).

     :param C: Distancia al borde del perno.
     :param CcrN: Semi-lado del área de influencia (distancia crítica al borde).
    '''
    return min(0.7+0.3*C/CcrN,1)

def getScrN(hEf):
  '''
  Lado del área de influencia de un anclaje individual
   according to clause 5.2.2.4 b) (ecuación 5.3b) del
   clause 5.2.2.4 b) of EOTA TR029.

   :param hEf: Profundidad efectiva del anclaje (m).
  '''
  return 3*hEf

def getA0cN(posPerno, hEf):
    '''
    Polígono que representa el área de influencia de un anclaje individual according to clause 5.2.2.4 b) (figura 5.4a) of EOTA TR029.

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
  Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte according to clause 5.2.2.4.

  :param A0cN: Área de influencia del anclaje (o grupo de anclajes).
  :param AcN: Area of the intersection of the anchor influence area and the concrete support contour.
  '''
  return AcN/A0cN

# Splitting

def getCcrSpHiltiHY150(h, hEf):
  '''
  Semi-lado del área de influencia (distancia crítica al borde)
     de un anclaje individual according to 
     table 7 of ETA-05/0051 (page 19).

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
    according to clause 5.2.2.6 b) of EOTA TR029.

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
  ''' Factor que introduce en el cálculo la influencia de la distancia al borde de la pieza soporte according to clause 5.2.2.6 b).

   :param A0spN: Área de influencia del anclaje (o grupo de anclajes).
   :param AspN: Area of the intersection of the anchor influence area and the concrete support contour.
  '''
  return AspN/A0spN

'''
************************************************************************
************************ Shear resistance ******************************
************************************************************************
'''

def shearResistanceWithoutLeverArm(As, fuk):
    '''
    Resistencia de un perno a cortante according to clause 5.2.3.2 a) of EOTA TR029.

    :param As: Área de la sección transversal del perno.
    :param fuk: Valor característico de la carga de rotura a tracción.
    '''
    return 0.5*As*fuk

def shearResistanceGroupWithoutLeverArm(As, fuk, nr):
    '''
    Resistencia de un grupo de pernos a cortante according to clause 5.2.3.2 a) of EOTA TR029.
     As: Área de la sección transversal del perno.
     fuk: Valor característico de la carga de rotura a tracción.
     n: Número de pernos del grupo.
    '''
    return 0.5*As*fuk*nr*0.8

def shearResistanceConcretePryOut(NRkp, NRkc, hEf):
    '''
    Resistencia de un perno a cortante por desprendimiento de cuña de hormigón
     according to expressions 5.7 y 5.7a of clause 5.2.3.3 of EOTA TR029.

     :param NRkp: Characteristic resistance of combined pull-out and
                  concrete cone failure (5.2.2.3 of EOTA TR029).
     :param NRkc: Characteristic resistance in case of concrete cone
                  failure (5.2.2.4 of EOTA TR029).
     :param hEf: Profundidad efectiva of the anchor (m).
    '''
    k= 1.0
    if(hEf>=60e-3): # 60mm
      k=2
    return k*min(NRkp,NRkc)

'''
 ************************************************************************
 *********** Shear resistance: concrete edge failure ********************
 ************************************************************************
'''

def psiReVFactor(descr):
  '''
  Coeficiente que introduce la influencia del tipo de refuerzo empleado en cracked concrete  en la expresión 5.8 of EOTA TR029, calculado according to apartado g) of clause 5.2.3.4 of EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: Anclaje en cracked concrete o no fisurado sin reinforcement de refuerzo en el borde.
    2: Anclaje en cracked concrete con reinforcement de refuerzo en el borde (diámetro > 12mm).
    3: Anclaje en cracked concrete con reinforcement de refuerzo en el borde y estribos próximos entre si (separados menos de 10 cm).
  '''
  return ifte(descr<=1,1.0,ifte(descr<=2,1.2,1.4))

def psiEcVFactor(ev, c1):
  '''
  Coeficiente que introduce la influencia de la variación de los cortantes en los distintos pernos de un grupo, calculado according to expression 5.8h del apartado f) of clause 5.2.3.4 of EOTA TR029.

   :param ev: Excentricidad de la resultante de esfuerzos cortantes respecto al centro de gravedad del grupo.
   :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return min(1/(1+2*ev/3/c1),1.0)

def psiAlphaVFactor(alphaV):
  '''
  Coeficiente que introduce la influencia del ángulo entre la carga aplicada y la perpendicular al borde libre de la pieza de hormigón (menor de 90 grados) see figure 4.7c y expresión 5.8g del apartado f) of clause 5.2.3.4 of EOTA TR029.

  :param alphaV: Ángulo entre la carga aplicada y la perpendicular al borde libre de la pieza de hormigón.
  '''
  return max(sqrt(1.0/(cos(alphaV)^2+(sin(alphaV)/2.5)^2)),1.0)

def psiHVFactor(h, c1):
  '''
  Coefficient that is used to introduce in the calculation the lack of 
  proportionality between the reduction of shear strength and the decrease 
  of the part thickness as is assumed in the quotient Ac,v/Ac,v0. See 
  expression 5.8f of the clause 5.2.3.4 of EOTA TR029.

  :param h: Concrete part thickness.
  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return max(sqrt(1.5*c1/h),1.0)

def psiSVFactor(c1, c2):
  '''
  Coeficiente que sirve para introducir en el cálculo la perturbación en la distribución de tensiones en el hormigón que produce la presencia de un segundo borde libre próximo al anclaje. Ver expresión 5.8e of clause 5.2.3.4 of EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param c2: Distancia desde el centro de gravedad del grupo al otro borde libre.
  '''
  return min((0.7+0.2*c2/c1),1.0)

def areaAcV0(h, c1):
  '''
  Area del cono de anclaje a cortante para un perno. see figure 5.6 y expresión 5.8d of EOTA TR029.

  :param h: Concrete part thickness.
  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return 3*c1*min(1.5*c1,h)

def AcV2Pernos(h, c1, s2):
  '''
  Area del cono de anclaje para dos pernos. see figure 5.7 b of EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param h: Concrete part thickness.
  :param s2: Distance between the bolts.
  '''
  return (3*c1+min(s2,3*c1))*min(1.5*c1,h)

def AcV2PernosC2(h, c1, c2, s2):
  '''
  Area del cono de anclaje para dos pernos junto a un borde libre. see figure 5.7 b y c of EOTA TR029.

  :param  c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param c2: Distancia desde el centro de gravedad del grupo al otro borde.
  :param h: Concrete part thickness.
  :param s2: Distance between the bolts.
  '''
  return (1.5*c1+c2+min(s2,3*c1))*min(1.5*c1,h)

def k1Expr58A(descr):
  '''
  Coeficiente que introduce la influencia de la fisuración del hormigón en la expresión 5.8a of EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: Anclaje en cracked concrete.
    2: Anclaje en non-cracked concrete.
  '''
  return ifte(descr<=1,1.7,2.4)

def VRkC0Expr58(k1, d, hEf, fckCube, c1):
  '''
  Valor inicial de la resistencia característica of the anchor a cortante por fallo del borde according to expression 5.8a of clause 5.2.3.4 (a) of EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param d: Diámetro del perno.
  :param hEf: Profundidad efectiva of the anchor.
  '''
  alpha= tonum(0.1*(hEf/c1)^1.5)
  beta= tonum(0.1*(d/c1)^0.2)
  return k1*pow(d*1000,alpha)*pow(hEf*1000,beta)*sqrt(fckCube/1e6)*pow(c1*1000,1.5)

