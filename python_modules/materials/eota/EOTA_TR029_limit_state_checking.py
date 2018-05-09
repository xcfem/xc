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

   As: stressed cross section of steel (m2).
   fuk: characteristic steel ultimate tensile strength (nominal value) (Pa).
  '''
  return As*fuk

def axialInitialResistancePullOut(d, hEf, tauRk):
    '''Characteristic resistance in case of combined pull -out and concrete cone failure according to expression 5.2a in clause 5.2.2.3 a) of EOTA TR029.

    :param d: Anchor diameter (m).
    :param hEf: effective anchorage depth (m).
    :param tauRk: Characteristic bond resistance, depending on the concrete strength class, (must be taken from relevant ETA) (Pa).
    '''   
    return math.pi*d*hEf*tauRk

def axialInitialResistanceConeFailure(k1, fck_cube, hEf):
  ''' Characteristic resistance of an anchor in case of concrete cone failure
   according to expression 5.3a in clause 5.2.2.4 a) of EOTA TR029.

   :param k1: 7.2 for applications in cracked concrete and 10.1 for applications in non-cracked concrete.
   :param fck_cube: characteristic concrete compression strength measured 
                    on cubes with a side length of 150 mm (value of concrete 
                    strength class according to ENV 206) (Pa).
   :param hEf: effective anchorage depth (m).
   '''
  return k1*math.sqrt(fck_cube/1e6)*pow(hEf*1e3,1.5)

def getScrNp(d, hEf, tauRkUcr):
  '''Spacing for ensuring the transmission of the characteristic resistance 
     of a single anchor without spacing and edge effects in case of pullout
     failure. Is equal to the side of the influence area of an individual 
     anchor (critical distance between anchors) according to clause 5.2.2.3 b)
     (ecuación 5.2c) of EOTA TR029.

   :param d: anchor diameter (m).
   :param hEf: effective anchorage depth (m).
   :param tauRkUcr: Characteristic bond resistance for non-craked concrete (must be taken from relevant ETA) (Pa).
  '''   
  return min(20e3*d*math.sqrt(tauRkUcr/7.5e6),3*hEf)

def getCcrNp(d, hEf, tauRkUcr):
  '''  half-side del área de influencia (distancia crítica al borde) de un anclaje individual according to clause 5.2.2.3 b) (ecuación 5.2d) of EOTA TR029.

   :param d: anchor diameter (m).
   :param hEf: effective anchorage depth (m).
   :param tauRkUcr: Characteristic bond resistance for non-craked concrete (must be taken from relevant ETA) (Pa).
  '''   
  return getScrNp(d,hEf,tauRkUcr)/2

def getA0pN(d,anchorPosition, hEf, tauRkUcr):
    '''
     Polígono que representa el área de influencia de un anclaje individual according to clause 5.2.2.3 b) (figura 5.1) of EOTA TR029.

     :param d: anchor diameter (m).
     :param anchorPosition: Posición del del perno.
     :param hEf: effective anchorage depth (m).
     :param tauRkUcr: Characteristic bond resistance for non-cracked concrete (must be taken from relevant ETA) (Pa).
    '''   
    halfSideA0pN= getCcrNp(d,hEf,tauRkUcr)
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0pN,anchorPosition.y-halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0pN,anchorPosition.y-halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0pN,anchorPosition.y+halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0pN,anchorPosition.y+halfSideA0pN))
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

     :param C: edge distance.
     :param CcrN: half-side del área de influencia (distancia crítica al borde).
    '''
    return min(0.7+0.3*C/CcrN,1)

def getScrN(hEf):
  '''
  SideLength del área de influencia de un anclaje individual
   according to clause 5.2.2.4 b) (ecuación 5.3b) del
   clause 5.2.2.4 b) of EOTA TR029.

   :param hEf: effective anchorage depth (m).
  '''
  return 3*hEf

def getA0cN(anchorPosition, hEf):
    '''
    Polígono que representa el área de influencia de un anclaje individual according to clause 5.2.2.4 b) (figura 5.4a) of EOTA TR029.

    :param anchorPosition: anchor position.
    :param hEf: effective anchorage depth (m).
    '''
    halfSideA0cN= getScrN(hEf)/2
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0cN,anchorPosition.y-halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0cN,anchorPosition.y-halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0cN,anchorPosition.y+halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0cN,anchorPosition.y+halfSideA0cN))
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
  half-side del área de influencia (distancia crítica al borde)
     de un anclaje individual according to 
     table 7 of ETA-05/0051 (page 19).

   :param h: thickness of concrete member (m).
   :param hEf: effective anchorage depth (m).
  '''
  if(h>=2*hEf):
    return hef 
  elif(h>1.3*hEf):
    return 4.6*hEf-1.8*h 
  else:
    return 2.26*hEf

def getA0spN(anchorPosition, CcrSp):
    '''
    Polígono que representa el área de influencia de un anclaje individual
    according to clause 5.2.2.6 b) of EOTA TR029.

    :param anchorPosition: anchor position.
    :param CcrSp: edge distance for ensuring the transmission of the
                  characteristic tensile resistance of a single
                  anchor without spacing and edge effects in case of 
                  splitting failure (m).
    '''
    retval= geom.Poligono2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-CcrSp,anchorPosition.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+CcrSp,anchorPosition.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+CcrSp,anchorPosition.y+CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-CcrSp,anchorPosition.y+CcrSp))
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

    :param As: stressed cross section of steel.
    :param fuk: characteristic steel ultimate tensile strength (nominal value).
    '''
    return 0.5*As*fuk

def shearResistanceGroupWithoutLeverArm(As, fuk, nr):
    '''
    Resistencia de un grupo de pernos a cortante according to clause 5.2.3.2 a) of EOTA TR029.
     As: stressed cross section of steel.
     fuk: characteristic steel ultimate tensile strength (nominal value).
     n: number of anchors in a group.
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
     :param hEf: effective anchorage depth (m).
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
  Coeficiente que introduce la influencia del tipo de refuerzo empleado en cracked concrete  en la expresión 5.8 of EOTA TR029, computed according to apartado g) of clause 5.2.3.4 of EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: anclaje en cracked concrete o no fisurado sin reinforcement de refuerzo en el borde.
    2: anclaje en cracked concrete con reinforcement de refuerzo en el borde (diámetro > 12mm).
    3: anclaje en cracked concrete con reinforcement de refuerzo en el borde y estribos próximos entre si (separados menos de 10 cm).
  '''
  return ifte(descr<=1,1.0,ifte(descr<=2,1.2,1.4))

def psiEcVFactor(ev, c1):
  '''
  Coeficiente que introduce la influencia de la variación de los cortantes en los distintos pernos de un grupo, computed according to expression 5.8h del apartado f) of clause 5.2.3.4 of EOTA TR029.

   :param ev: Eccentricity of the shear forces resultant with respect to the group centroid.
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
    1\: anclaje en cracked concrete.
    2\: anclaje en non-cracked concrete.
  '''
  return ifte(descr<=1,1.7,2.4)

def VRkC0Expr58(k1, d, hEf, fckCube, c1):
  '''
  Valor inicial de la resistencia característica of the anchor a cortante por fallo del borde according to expression 5.8a of clause 5.2.3.4 (a) of EOTA TR029.

  :param d: anchor diameter.
  :param hEf: effective anchorage depth.
  :param fckCube: characteristic concrete compression strength measured 
                  on cubes with a side length of 150 mm (value of concrete 
                  strength class according to ENV 206) (Pa).
  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.

  '''
  alpha= tonum(0.1*(hEf/c1)^1.5)
  beta= tonum(0.1*(d/c1)^0.2)
  return k1*pow(d*1000,alpha)*pow(hEf*1000,beta)*sqrt(fckCube/1e6)*pow(c1*1000,1.5)

