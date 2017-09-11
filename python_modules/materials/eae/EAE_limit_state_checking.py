# -*- coding: utf-8 -*-
from __future__ import division
import math
import scipy.interpolate

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def VdSoldAlma(l,a,bw,gmw,fu):
    '''
    Shear strength of the weld beads according to clause 61.4.1
    of EAE.
    '''
    return l*2*a*bw*gmw*math.sqrt(3.0)*fu  #Check 30/11/2011

def LongMaxSoldAlma(tw):
    '''
    Maximum length of the weld beads according to clause 61.4.1 
    of EAE.
    '''
    return 14*tw

def getFilletWeldThroatPlaneYieldCriteriaLHS(sigmaN, tauN, tauPll):
  '''
  Returns the stress combination to use in the left hand side of the
  yield criterion checking according to clause 59.8.2 of EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param tauN: Tangential stress normal to weld bead axis
          in weld bead throat plane (see figure 59.8).
    :param tauPll: Tangential stress parallel to weld bead axis 
          in weld bead throat plane (see figure 59.8). 
  '''
  return math.sqrt((sigmaN)**2+3*((tauN)**2+(tauPll)**2))

# Table 59.8.2 in clause 59.8.2 of EAE (page 256)
x= [235e6,275e6,355e6,420e6,460e6]
y= [0.8,0.85,0.90,1.0,1.0]
BetaWtable= scipy.interpolate.interp1d(x,y)

def getFilletWeldYieldCriteriaRHSValue(fu, fy, gammaM2):
  '''
  Return weld bead ultimate stress to be used for strength checking 
   according to first condition in clause 59.8.2 of EAE (page 256).

    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return fu/BetaWtable(fy)/gammaM2


def getFilletWeldThroatPlaneCondition1CapacityFactor(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  '''
  Return the capacity factor for the first condition of clause
   59.8.2 de EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param tauN: Tangential stress normal to weld bead axis
          in weld bead throat plane (see figure 59.8).
    :param tauPll: Tangential stress parallel to weld bead axis 
          in weld bead throat plane (see figure 59.8). 

    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return getFilletWeldThroatPlaneYieldCriteriaLHS(sigmaN,tauN,tauPll)/getFilletWeldYieldCriteriaRHSValue(fu,fy,gammaM2)

def getFilletWeldUltimateNormalStress(fu, gammaM2):
  '''
  Return the ultimate stress of the weld bead to be used for checking the
  second condition in clause 59.8.2 of EAE (page 256).

    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return 0.9*fu/gammaM2

def getFilletWeldThroatPlaneCondition2CapacityFactor(sigmaN, fu, gammaM2):
  '''
  Return the capacity factor that correspond to the second condition of
  clause 59.8.2 of EAE (page 256).

    :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor of value 1.25. 
  '''
  return abs(sigmaN)/getFilletWeldUltimateNormalStress(fu,gammaM2)

def getFilletWeldThroatPlaneCapacityFactor(sigmaN, tauN, tauPll, fu, fy, gammaM2):
  '''
  Return the minimum of the capacity factors that correspond to the
  conditions of clause 59.8.2 of EAE (page 256).

  :param sigmaN: Tensión normal sobre el plano de garganta del cordón (ver figure 59.8).
  :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
  :param gammaM2: Partial safety factor of value 1.25.
  '''
  return min(getFilletWeldThroatPlaneCondition1CapacityFactor(sigmaN,tauN,tauPll,fu,fy,gammaM2),getFilletWeldThroatPlaneCondition2CapacityFactor(sigmaN,fu,gammaM2))


def getSigmaNThroatPlane(n, t):
  '''
  Return the normal stress in the throat plane from the stress values
  in the section rotated onto one of the weld legs.
  See "Estructuras de Acero" of Ramón Argüelles Álvarez 
  (url={https://books.google.es/books?id=ubIXPwAACAAJ}, isbn={9788495279972}) 
  page 4.17 and the book "Estructuras. Estructuras metálicas. U.D.2, Uniones"
  (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
  of Vicente Cudós Samblancat from Escuela de la Edificación.

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  t: Tangential stress normal to the weld axis.
  '''
  return (n-t)/math.sqrt(2)

def getTauNThroatPlane(n, t):
  '''
  Return the tangential stress normal to the weld axis on the throat plane
  from the values of the stress in the section rotated onto one of the weld
  legs.
  See "Estructuras de Acero" of Ramón Argüelles Álvarez 
  (url={https://books.google.es/books?id=ubIXPwAACAAJ}, isbn={9788495279972}) 
  page 4.17 and the book "Estructuras. Estructuras metálicas. U.D.2, Uniones"
  (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
  of Vicente Cudós Samblancat from Escuela de la Edificación.

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  t: Tangential stress normal to the weld axis.
'''
  return (n+t)/math.sqrt(2)

def getFilletWeldYieldCriteriaLHS(n, tn, ta):
  '''
  Return the stress value to use in the right hand side of the yield criterion
  according to clause 59.8.2 of EAE (page 256)

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  tn: Tangential stress over the rotated plane normal to the weld axis.
  :param  ta: Tangential stress over the rotated plane parallel to 
              the weld axis.
  '''
  sgN= getSigmaNThroatPlane(n,tn) 
  tN= getTauNThroatPlane(n,tn)
  return getFilletWeldThroatPlaneYieldCriteriaLHS(sgN,tN,ta)

def getFilletWeldCondition1CapacityFactor(n, tn, ta, fu, fy, gammaM2):
  '''
  Return the capacity factor that correspond to the first condition
   of clause 59.8.2 of EAE (page 256)

  :param  n: Normal stress over the rotated plane (see figure 24.1 
             on the book from UNED).
  :param  tn: Tangential stress over the rotated plane normal to the weld axis.
  :param  ta: Tangential stress over the rotated plane parallel to 
              the weld axis.
  :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
  :param fy: limit of elasticity of the welding steel (see table 59.8.2) 
        expressed in pascals.
  :param gammaM2: Partial safety factor of value 1.25.
  '''
  return getFilletWeldYieldCriteriaLHS(n,tn,ta)/getFilletWeldYieldCriteriaRHSValue(fu,fy,gammaM2)

def getFilletWeldCondition2CapacityFactor(n, tn, fu, gammaM2):
  '''
  Return the capacity factor that correspond to the second condition of those
  in clause 59.8.2 of EAE (page 256)

  :param sigmaN: Normal stress in weld bead throat plane (see figure 59.8).
  :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
  :param gammaM2: Partial safety factor of value 1.25.
  '''
  sgN= getSigmaNThroatPlane(n,tn) 
  return abs(sgN)/getFilletWeldUltimateNormalStress(fu,gammaM2)

def getFilletWeldMinimumThroatDepth(t):
  '''
  Return the minimum throat depth for a fillet bead that welds a sheet 
  of thickness t according to clause 59.3.2 of EAE (page 296).

    :param t: Sheet thickness.
  '''
  if(t<=10e-3):
    return 3e-3
  else:
    if(t<=20e-3):
      return 4.5e-3
    else:
      return 5.6e-3

def getFilletWeldMaximumThroatDepth(t):
  '''
  Return the maximum throat thickness for a fillet bead that welds a sheet 
  of thickness t according to clause 59.3.2 of EAE (page 296).

    :param t: Sheet thickness.
  '''
  return 0.7*t

def getFilletWeldMinimumThroatDepthSheets(t1, t2):
  '''
  Return the minimum throat thickness which can be used to weld two sheets
  according to articla 59.3.2 of EAE (page 296).

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
  '''
  amin1= getFilletWeldMinimumThroatDepth(t1)
  amin2= getFilletWeldMinimumThroatDepth(t2)
  return max(amin1,amin2)

def getFilletWeldMaximumThroatDepthSheets(t1, t2):
  '''
  Return the maximum throat thickness which can be used to weld two sheets
  according to articla 59.3.2 of EAE (page 296).

    :param t1: Thickness of sheet 1.
    :param t2: Thickness of sheet 2.
  '''
  amax1= getFilletWeldMaximumThroatDepth(t1)
  amax2= getFilletWeldMaximumThroatDepth(t2)
  return min(amax1,amax2)

# Maximum value of the force to bear by means of an
# simple angle support (without stiffeners). According to EAE, clause 
# 61.5, page 322. See also 25.2.2 on the book
# "Estructuras. Estructuras metálicas. U.D.2, Uniones"
# (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
# of Vicente Cudós Samblancat from Escuela de la Edificación.

def ultimateLoad1(tf,r,tw,fy):
    '''
    Return the local crushing strength of the beam web.

    :param tf: Thickness of beam flange.
    :param r: Flange-web fillet radius.
    :param tw: Thickness of beam web.
    :param fy: Steel yield strength.
    '''
    return 2.5*(tf+r)*tw*fy
  

def ultimateLoad2(b,a,fu,betaW,gammaM2):
    '''
    Return the weld bead strength.

    :param b: Support length measured perpendicular to the beam direction.
    :param a: Weld bead throat thickness.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param betaW: Correlation coefficient (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor for steel (clause 15.3 page 34 EAE).
    '''
    return b*a*fu/betaW/math.sqrt(2.0)/gammaM2
  

def ultimateLoad3(b,t,fy):
    '''
    Return angle leg shear strength.

    :param b: Support length measured perpendicular to the beam direction.
    :param t: Angle leg thickness.
    :param fy: Steel yield strength.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    '''
    return b*t*fy/math.sqrt(3.0)
  

def ultimateLoad(tf,tw,r,b,a,fy,fu,betaW,gammaM2):
    '''
    Return the bearing capacity of the angle support.

    :param tf: Thickness of beam flange.
    :param tw: Thickness of beam web.
    :param r: Flange-web fillet radius.
    :param b: Support length measured perpendicular to the beam direction.
    :param a: Weld bead throat thickness.
    :param fy: Steel yield strength.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param betaW: Correlation coefficient (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor for steel (clause 15.3 page 34 EAE).
    '''
    return min(ultimateLoad1(tf,r,tw,fy),min(ultimateLoad2(b,a,fu,betaW,gammaM2),ultimateLoad3(b,t,fy)))
  
# Maximum value of the force to bear by means of an
# angle support with stiffeners. According to EAE, clause 
# 61.5.1, page 324. See also 25.3.3 on the book
# "Estructuras. Estructuras metálicas. U.D.2, Uniones"
# (url={https://books.google.es/books?id=X9JIRAAACAAJ}, isbn={9788486957087}) 
# of Vicente Cudós Samblancat from Escuela de la Edificación.

def widthMax(tChapa,l,H):
    '''
    Return the maximum depth of the stiffener (see figure 61.1.5.b
    page 325 EAE).

    :param tChapa: Thicness of the sheets that supports the load.
    :param l: Length of the horizontal cathetus of the stiffener.
    :param H: Length of the vertical cathetus of the stiffener. 
    '''
    theta=math.atan2(l,H)
    return l*math.cos(theta)+tChapa*math.sin(theta)
  

  
def esbeltezAdim(c,tRig,fy,Es):
    '''
    Returns stiffener dimensionless slenderness.

    :param c: Maximum stiffener depth.
    :param tRig: Stiffener thickness.
    :param fy: Steel yield strength.
    :param Es: Steel elastic modulus.
    '''
    return 0.805*c/tRig*math.sqrt(fy/Es) 


  
def coefEscuadra(coefLambda):
    '''
    Return stiffener square coefficient.

    :param coefLambda: stiffener dimensionless slenderness.
    '''
    return 0.14*(coefLambda)**2-1.07*coefLambda+2.3 

  
def momPlastRig(tRig,c,fy):
    '''
    Return stiffener plastic moment.

    :param tRig: Stiffener thickness.
    :param c: Maximum stiffener depth.
    :param fy: Steel yield strength.
    '''
    return tRig*c**2*fy/4 

def ultimateLoadRig(CE,d,MplRd):
    '''
    Return the angle support strength due to the collapse of the stiffener.

    :param CE: Coeficiente de escuadra.
    :param d: Lever arm of the load.
    :param MplRd: Momento plástico de la sección del rigidizador de depth c. 
    '''
    return CE*MplRd/d 

#Soldaduras

 
def ultimateLoadBead1(a1,l,H,fu,betaW,gammaM2):
    '''
    Return the angle support strength due to the collapse of weld beads (1)
    (see figure 61.1.5.b page 325 EAE) that connect the stiffener to the
    angle leg.

    :param a1: Weld throat thickness (1).
    :param l: Length of the horizontal cathetus of the stiffener.
    :param H: Length of the vertical cathetus of the stiffener.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param betaW: Correlation coefficient (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor for steel (clause 15.3 page 34 EAE).
     '''
    theta=math.atan2(l,H)
    return (2.0*a1*l*fu)/(betaW*math.sqrt(2+3*math.tan(theta)**2)*gammaM2)
  

  
def ultimateLoadBead2(a2,l,H,fu,betaW,gammaM2):
    '''
    Return the angle support strength due to the collapse of weld beads (2)
    (see figure 61.1.5.b page 325 EAE) that connect the stiffener to the
    angle leg.

    :param a2: Weld throat thickness (2).
    :param l: Length of the horizontal cathetus of the stiffener.
    :param H: Length of the vertical cathetus of the stiffener.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param betaW: Correlation coefficient (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor for steel (clause 15.3 page 34 EAE).
    '''
    theta=math.atan2(l,H)
    return (2.0*a2*H*fu)/(betaW*math.sqrt(3+2*math.tan(theta)**2)*gammaM2)
  

  
def ultimateLoadBead3(a3,b,l,H,fu,betaW,gammaM2):
    '''
    Return the angle support strength due to the collapse of weld beads (3)
    (see figure 61.1.5.b page 325 EAE) that connect the stiffener to the
    angle leg.

    :param a3: Weld throat thickness (3).
    :param b: Angle leg length (beads length 3).
    :param l: Length of the horizontal cathetus of the stiffener.
    :param H: Length of the vertical cathetus of the stiffener.
    :param fu: Steel ultimate strength (table 59.8.2 page 304 EAE).
    :param betaW: Correlation coefficient (table 59.8.2 page 304 EAE).
    :param gammaM2: Partial safety factor for steel (clause 15.3 page 34 EAE).
    '''
    theta=math.atan2(l,H)
    return (math.sqrt(2)*b*a3*fu)/(betaW*math.tan(theta)*gammaM2)
  
