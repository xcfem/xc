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
from miscUtils import LogMessages as lmsg

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
     (equation 5.2c) of EOTA TR029.

   :param d: anchor diameter (m).
   :param hEf: effective anchorage depth (m).
   :param tauRkUcr: Characteristic bond resistance for non-craked concrete (must be taken from relevant ETA) (Pa).
  '''   
  return min(20e3*d*math.sqrt(tauRkUcr/7.5e6),3*hEf)

def getCcrNp(d, hEf, tauRkUcr):
  ''' Half-side of the influence area (critical distance to the edge) of an
      individual anchor according to clause 5.2.2.3 b) (equation 5.2d) 
      of EOTA TR029.

   :param d: anchor diameter (m).
   :param hEf: effective anchorage depth (m).
   :param tauRkUcr: Characteristic bond resistance for non-craked concrete 
          (must be taken from relevant ETA) (Pa).
  '''   
  return getScrNp(d,hEf,tauRkUcr)/2

def getA0pN(d,anchorPosition, hEf, tauRkUcr):
    '''
     Polígono que representa el influence area of an individual anchor according to clause 5.2.2.3 b) (figura 5.1) of EOTA TR029.

     :param d: anchor diameter (m).
     :param anchorPosition: Posición del del perno.
     :param hEf: effective anchorage depth (m).
     :param tauRkUcr: Characteristic bond resistance for non-cracked concrete (must be taken from relevant ETA) (Pa).
    '''   
    halfSideA0pN= getCcrNp(d,hEf,tauRkUcr)
    retval= geom.Polygon2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0pN,anchorPosition.y-halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0pN,anchorPosition.y-halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0pN,anchorPosition.y+halfSideA0pN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0pN,anchorPosition.y+halfSideA0pN))
    return retval

def getFactor2pN(A0pN, ApN):
    '''
    Factor that takes into account the influence of the distance to the edge
    of the support member according to clause 5.2.2.3 b).

    :param A0pN: influence area of the anchor (or anchor group).
    :param ApN: Area of the intersection of the anchor influence area and the concrete support contour.
    '''
    return ApN/A0pN

def getFactor1N(C, CcrN):
    '''
    Factor that takes into account the influence of the distance to the edge
    of the support member in the stress distribution, according to 
    expression 5.2e of clause 5.2.2.3 c).

     :param C: edge distance.
     :param CcrN: half-side del influence area (critical distance to the edge).
    '''
    return min(0.7+0.3*C/CcrN,1)

def getScrN(hEf):
  '''
  SideLength del influence area of an individual anchor
   according to clause 5.2.2.4 b) (equation 5.3b) del
   clause 5.2.2.4 b) of EOTA TR029.

   :param hEf: effective anchorage depth (m).
  '''
  return 3*hEf

def getA0cN(anchorPosition, hEf):
    '''
    Polígono que representa el influence area of an individual anchor according to clause 5.2.2.4 b) (figura 5.4a) of EOTA TR029.

    :param anchorPosition: anchor position.
    :param hEf: effective anchorage depth (m).
    '''
    halfSideA0cN= getScrN(hEf)/2
    retval= geom.Polygon2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0cN,anchorPosition.y-halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0cN,anchorPosition.y-halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+halfSideA0cN,anchorPosition.y+halfSideA0cN))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-halfSideA0cN,anchorPosition.y+halfSideA0cN))
    return retval

def getFactor2cN(A0cN, AcN):
  '''
  Factor that takes into account the influence of the distance to the edge
  of the support member according to clause 5.2.2.4.

  :param A0cN: influence area of the anchor (or anchor group).
  :param AcN: area of the intersection of the anchor influence area and the 
              concrete support contour.
  '''
  return AcN/A0cN

# Splitting

def getCcrSpHiltiHY150(h, hEf):
  '''
  half-side del influence area (critical distance to the edge)
     of an individual anchor according to 
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
    Polygon that represents the influence area of an individual anchor
    according to clause 5.2.2.6 b) of EOTA TR029.

    :param anchorPosition: anchor position.
    :param CcrSp: edge distance for ensuring the transmission of the
                  characteristic tensile resistance of a single
                  anchor without spacing and edge effects in case of 
                  splitting failure (m).
    '''
    retval= geom.Polygon2d()
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-CcrSp,anchorPosition.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+CcrSp,anchorPosition.y-CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x+CcrSp,anchorPosition.y+CcrSp))
    retval.agregaVertice(geom.Pos2d(anchorPosition.x-CcrSp,anchorPosition.y+CcrSp))
    return retval

def getFactor2spN(A0spN, AspN):
  ''' Factor that takes into account the influence of the distance to the edge
       of the support member according to clause 5.2.2.6 b).

   :param A0spN: influence area of the anchor (or anchor group).
   :param AspN: area of the intersection of the anchor influence area and the 
                concrete support contour.
  '''
  return AspN/A0spN

'''
************************************************************************
************************ Shear resistance ******************************
************************************************************************
'''

def shearResistanceWithoutLeverArm(As, fuk):
    '''
    Shear strength of a bolt according to clause 5.2.3.2 a) of EOTA TR029.

    :param As: stressed cross section of steel.
    :param fuk: characteristic steel ultimate tensile strength (nominal value).
    '''
    return 0.5*As*fuk

def shearResistanceGroupWithoutLeverArm(As, fuk, nr):
    '''
    Shear strenght of a bolt group according to clause 5.2.3.2 a) of EOTA TR029.
     As: stressed cross section of steel.
     fuk: characteristic steel ultimate tensile strength (nominal value).
     n: number of anchors in a group.
    '''
    return 0.5*As*fuk*nr*0.8

def shearResistanceConcretePryOut(NRkp, NRkc, hEf):
    '''
    Shear strength of a bolt por desprendimiento de cuña de hormigón
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
  Coeficiente que introduce la influencia del tipo de refuerzo empleado en cracked concrete  en la expresión 5.8 of EOTA TR029, computed according to section g) of clause 5.2.3.4 of EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: anclaje en cracked concrete o no fisurado sin reinforcement de refuerzo en el borde.
    2: anclaje en cracked concrete con reinforcement de refuerzo en el borde (diámetro > 12mm).
    3: anclaje en cracked concrete con reinforcement de refuerzo en el borde y estribos próximos entre si (separados menos de 10 cm).
  '''
  return ifte(descr<=1,1.0,ifte(descr<=2,1.2,1.4))

def psiEcVFactor(ev, c1):
  '''
  Factor that takes account of a group effect when different shear loads are 
  acting on the individual anchors of a group, computed according to 
  expression 5.8h del section f) of clause 5.2.3.4 of EOTA TR029.

   :param ev: Eccentricity of the shear forces resultant with respect to the 
              group centroid.
   :param c1: edge distance in direction 1; in case of anchorages close to an 
              edge loaded in shear c 1 is the edge distance in direction of 
              the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  '''
  return min(1/(1+2*ev/3/c1),1.0)

def psiAlphaVFactor(alphaV):
  '''
  Factor that takes account of the angle between the applied load and the direction perpendicular to the free edge of the concrete member (must be less than 90 degrees) see figure 4.7c and expression 5.8g on section f) of clause 5.2.3.4 of EOTA TR029.

  :param alphaV: angle between the applied load and the direction perpendicular to the free edge of the concrete member.
  '''
  if(alphaV>(math.pi/2.0)):
    lmsg.error('angle must be smaller than 90 degrees (see section f of clause 5.2.3.4 of EOTA TR029).')
  return max(sqrt(1.0/(cos(alphaV)^2+(sin(alphaV)/2.5)^2)),1.0)

def psiHVFactor(h, c1):
  '''
  Coefficient that is used to introduce in the calculation the lack of 
  proportionality between the reduction of shear strength and the decrease 
  of the part thickness as is assumed in the quotient Ac,v/Ac,v0. See 
  expression 5.8f of the clause 5.2.3.4 of EOTA TR029.

  :param h: Concrete part thickness.
  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  '''
  return max(sqrt(1.5*c1/h),1.0)

def psiSVFactor(c1, c2):
  '''
  Coeficiente que sirve para introducir en el cálculo la perturbación en la distribución de tensiones en el hormigón que produce la presencia de un segundo borde libre próximo al anclaje. Ver expresión 5.8e of clause 5.2.3.4 of EOTA TR029.

  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  :param c2: edge distance in direction 2; direction 2 is perpendicular to 
             direction 1.
  '''
  return min((0.7+0.2*c2/c1),1.0)

def areaAcV0(h, c1):
  '''
  area of concrete cone of an individual anchor at the lateral concrete surface
  not affected by edges parallel to the assumed loading direction, member 
  thickness or adjacent anchors, assuming the shape of the fracture area as a 
  half pyramid with a height equal to c1 and a base-length of 1.5 c1 and 3 c1.
  See figure 5.6 and expression 5.8d of EOTA TR029.

  :param h: Concrete part thickness.
  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  '''
  return 3*c1*min(1.5*c1,h)

def AcV2Pernos(h, c1, s2):
  '''
  Area del cono de anclaje para dos pernos. see figure 5.7 b of EOTA TR029.

  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  :param h: Concrete part thickness.
  :param s2: Distance between the bolts.
  '''
  return (3*c1+min(s2,3*c1))*min(1.5*c1,h)

def AcV2PernosC2(h, c1, c2, s2):
  '''
  Area del cono de anclaje para dos pernos junto a un borde libre. see figure 5.7 b y c of EOTA TR029.

  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  :param c2: edge distance in direction 2; direction 2 is perpendicular to 
             direction 1.
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
  Initial value of the characteristic resistance of an anchor placed in cracked
  or non-cracked concrete and loaded perpendicular to the edge according to 
  expression 5.8a of clause 5.2.3.4 (a) of EOTA TR029.

  :param d: anchor diameter.
  :param hEf: effective anchorage depth.
  :param fckCube: characteristic concrete compression strength measured 
                  on cubes with a side length of 150 mm (value of concrete 
                  strength class according to ENV 206) (Pa).
  :param c1: edge distance in direction 1; in case of anchorages close to an 
             edge loaded in shear c 1 is the edge distance in direction of 
             the shear load (see Figure 2.1b and Figure 5.7of EOTA TR029).
  '''
  alpha= tonum(0.1*(hEf/c1)^1.5)
  beta= tonum(0.1*(d/c1)^0.2)
  return k1*pow(d*1000,alpha)*pow(hEf*1000,beta)*sqrt(fckCube/1e6)*pow(c1*1000,1.5)

