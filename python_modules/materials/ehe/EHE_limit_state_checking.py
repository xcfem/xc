# -*- coding: utf-8 -*-
from __future__ import division
''' Limit state checking according to structural concrete spanish standard EHE-08.'''
__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import math
import xc_base
import geom
from materials.ehe import EHE_materials
from materials.sections.fiber_section import fiber_sets
from materials import limit_state_checking_base as lscb
from postprocess import control_vars as cv
from miscUtils import LogMessages as lmsg
import scipy.interpolate
from solution import predefined_solutions
from materials import concrete_base


# Reinforced concrete section shear checking.

def getFcvEH91(fcd):
  '''
  Return fcv (concrete virtual shear strength)
   according to EH-91.

  :param fcd: design compressive strength of concrete.
  :param b: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  '''
  fcdKpcm2= fcd*9.81/1e6
  return 0.5*sqrt(fcdKpcm2)/9.81*1e6


def getVu1(fcd, Nd, Ac, b0, d, alpha, theta):
  '''
  Return value of Vu1 (shear strength at failure due to diagonal compression
  in the web) according to clause 44.2.3.1 of EHE.

  :param fcd: Design compressive strength of concrete.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth.
  :param alpha: angle between the shear rebars and the member axis (figure 44.2.3.1.a EHE).
  :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  
  '''
  f1cd= 0.6*fcd
  sgpcd= (Nd/Ac)
  K= min(5/3*(1+sgpcd/fcd),1)
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  return K*f1cd*b0*d*(ctgTheta+cotg(alpha))/(1+ctgTheta^2)

def getFcv(fact, fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  '''
  Return the value of fcv (concrete virtual shear strength)
  for parts WITH or WITHOUT shear reinforcement, according to clauses 
  44.2.3.2.1 y  44.2.3.2.2 of EHE.
  
  :param fact: factor equal to 0.12 for parts WITHOUT shear reinforcement 
               (0.18/gammac)
   and 0.10 for parts WITH shear reinforcement  (0.15/gammac).
  :param fck: concrete characteristic compressive strength.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  :param AsPas: Area of tensioned longitudinal steel rebars anchored
   at a distance greater than the effective depth of the section.
  :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
  :param AsAct: Area of tensioned longitudinal prestressed steel anchored
   at a distance greater than the effective depth of the section.
  :param fpd: design value of prestressing steel strength (clause 38.6 EHE).    
  '''
  sgpcd= Nd/Ac
  chi= 1+sqrt(200/(d*1000)) # d MUST BE EXPRESSED IN METERS.
  rol= min((AsPas+AsAct*fpd/fyd)/(b0*d),0.02)
  return fact*chi*(rol*fck/1e4)^(1/3)*1e6-0.15*sgpcd


def getVu2SIN(fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  '''
  Return the value of Vu2 (shear strength at failure due to tensile force in the web)
   for parts WITHOUT shear reinforcement, according to 
   clause 44.2.3.2.1 of EHE (1998).
  
  :param fck: concrete characteristic compressive strength.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  :param AsPas: Area of tensioned longitudinal steel rebars anchored
   at a distance greater than the effective depth of the section.
  :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
  :param AsAct: Area of tensioned longitudinal prestressed steel anchored
   at a distance greater than the effective depth of the section.
  :param fpd: design value of prestressing steel strength (clause 38.6 EHE).
   
  '''
  return getFcv(0.12,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)*b0*d

def getVcu(fck, Nd, Ac, b0, d, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd):
  '''
  Return the value of Vcu (contribution of the concrete to shear strength) 
  for parts WITH shear reinforcement, according to clause 44.2.3.2.2 of 
  EHE (1998).

  :param fck: concrete characteristic compressive strength.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  :param theta: angle between the concrete compressed struts and the member 
           axis (figure 44.2.3.1.a EHE 1998).
  :param AsPas: area of tensioned longitudinal steel rebars anchored
   at a distance greater than the effective depth of the section.
  :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
  :param AsAct: Area of tensioned longitudinal prestressed steel anchored
   at a distance greater than the effective depth of the section.
  :param fpd: design value of prestressing steel strength (clause 38.6 EHE).
  :param sgxd: design value of normal stress at the centre of gravity of the
   section parallel to the main axis of the member. Calculated assuming NON 
   CRACKED concrete (clause 44.2.3.2).
  :param sgyd: design value of normal stress at the centre of gravity of the
   section parallel to shear force Vd. Calculated assuming NON CRACKED 
   concrete (clause 44.2.3.2).
  '''
  fcv= getFcv(0.10,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
  fctm= fctMedEHE08(fck) 
  ctgThetaE= min(max(0.5,sqrt(fctm-sgxd/fctm-sgyd)),2.0)
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  beta= 0.0
  if(ctgTheta<ctgThetaE):
    beta= (2*ctgTheta-1)/(2*ctgThetaE-1)
  else:
    beta= (ctgTheta-2)/(ctgThetaE-2)
  return fcv*b0*d*beta

def getVsu(z, alpha, theta, AsTrsv, fyd):
  '''
  Return the value of Vsu (contribution of the web’s transverse reinforcement 
  to shear strength) for parts WITH shear reinforcement, according to 
  clause 44.2.3.2.2 of EHE.

  :param z: Mechanic lever arm.
  :param alpha: angle of the shear reinforcement with the member axis (see figure 44.2.3.1.a EHE).
  :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  :param AsTrsv: transverse reinforcement area.
  :param fyd: design yield strength of the transverse reinforcement.
  
  '''
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  fyalphad= min(fyd,400e6) # comentario al clause 40.2 EHE
  return z*sin(alpha)*(cotg(alpha)+ctgTheta)*AsTrsv*fyalphad


def getVu2(fck, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
  '''
  Return the value of Vu2 (ultimate shear force failure due to tensile force 
  in the web) for parts WITH or WITHOUT shear reinforcement, according to 
  clause 44.2.3.2.2 of EHE (1998).

  :param fck: concrete characteristic compressive strength.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  :param z: mechanic lever arm.
  :param alpha: angle of the shear reinforcement with the member 
   axis (see figure 44.2.3.1.a EHE).
  :param theta: angle between the concrete compressed struts and the 
   member axis (figure 44.2.3.1.a EHE).
  :param AsPas: Area of tensioned longitudinal steel rebars anchored
   at a distance greater than the effective depth of the section.
  :param fyd: reinforcing steel design yield strength (clause 38.3 of EHE).
  :param AsAct: Area of tensioned longitudinal prestressed steel anchored
   at a distance greater than the effective depth of the section.
  :param fpd: design value of prestressing steel strength (clause 38.6 of EHE).
  :param sgxd: design value of normal stress at the centre of gravity of the
   section parallel to the main axis of the member. Calculated assuming NON 
   CRACKED concrete (clause 44.2.3.2).
  :param sgyd: design value of normal stress at the centre of gravity of the
   section parallel to shear force Vd. Calculated assuming NON CRACKED 
   concrete (clause 44.2.3.2).
  :param AsTrsv: transverse reinforcement area.
  :param fydTrsv: design yield strength of the transverse reinforcement.
  '''
  vcu= 0.0
  vsu= 0.0
  if(AsTrsv>0.0):
    vcu= getVcu(fck,Nd,Ac,b0,d,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd)
    vsu= getVsu(z,alpha,theta,AsTrsv,fydTrsv)
  else:
    vcu= getVu2SIN(fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
    vsu= 0.0
  return vcu+vsu

def getVu(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
  '''
  Return the value of Vu= max(Vu1,Vu2) for parts WITH or WITHOUT shear 
   reinforcement, according to clause 44.2.3.2.2 of EHE (1998).

  :param fck: concrete characteristic compressive strength.
  :param Nd: axial force design value (positive if in tension).
  :param Ac: concrete section total area.
  :param b0: net width of the element according to clause 40.3.5.
  :param d: effective depth (meters).
  :param z: mechanic lever arm.
  :param alpha: angle of the shear reinforcement with the part axis (figure 44.2.3.1.a EHE).
  :param theta: angle between the concrete compressed struts and the 
   member axis (figure 44.2.3.1.a EHE).
  :param AsPas: Area of tensioned longitudinal steel rebars anchored
   at a distance greater than the effective depth of the section.
  :param fyd: reinforcing steel design yield strength (clause 38.3 EHE).
  :param AsAct: Area of tensioned longitudinal prestressed steel anchored
   at a distance greater than the effective depth of the section.
  :param fpd: design value of prestressing steel strength (clause 38.6 EHE).
  :param sgxd: design value of normal stress at the centre of gravity of the
   section parallel to the main axis of the member. Calculated assuming NON 
   CRACKED concrete (clause 44.2.3.2).
  :param sgyd: design value of normal stress at the centre of gravity of the
   section parallel to shear force Vd. Calculated assuming NON CRACKED 
   concrete (clause 44.2.3.2).
  :param AsTrsv: transverse reinforcement area.
  :param fydTrsv: design yield strength of the transverse reinforcement.
  '''
  vu1= getVu1(fcd,Nd,Ac,b0,d,alpha,theta)
  vu2= getVu2(fck,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  return min(vu1,vu2)

def shearOK(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
  '''Check shear.'''
  vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  if(Vrd<=vu):
    return True
  else:
    return False

def shearExploitationRatioe(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
  '''Shear exploitation ratio.'''
  vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  return Vrd/vu

class ShearDesignParameters(object):
  '''Defines shear design parameters.'''
  def __init__(self):
    self.concreteArea= 0.0 # Concrete section total area.
    self.widthMin= 0.0 # net width of the element according to clause 40.3.5.
    self.depthUtil= 0.0 # effective depth (meters).
    self.mechanicLeverArm= 0.0 # mechanic lever arm (meters).
    self.tensionedRebarsArea= 0.0 # Area of tensioned longitudinal steel rebars anchored at a distance greater than the effective depth of the section.
    self.tensionedStrandsArea= 0.0 # Area of tensioned longitudinal prestressed steel anchored at a distance greater than the effective depth of the section.
    self.areaShReinfBranchsTrsv= 0.0 # transverse reinforcement area.
    self.sigmaXD= 0.0 # design value of normal stress at the centre of gravity of the section parallel to the main axis of the member. Calculated assuming NON CRACKED concrete (clause 44.2.3.2).
    self.sigmaYD= 0.0 # design value of normal stress at the centre of gravity of the section parallel to shear force Vd. Calculated assuming NON CRACKED concrete (clause 44.2.3.2).
    self.angAlpha= math.pi/2 # angle of the shear reinforcement with the part axis (figure 44.2.3.1.a EHE).
    self.angTheta= math.pi/6. # Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    self.ultimateShearStrength= 0.0
#    print 'transv. reinf. area=',self.areaShReinfBranchsTrsv

  def computeUltimateShearStrength(self, concreteFibersSet, rebarFibersSet, tensionedRebarsFiberSet, fck, fcd, fyd, fpd, fydTrsv):
    '''Compute section shear strength.'''
    self.concreteArea= concreteFibersSet.getArea()
    self.widthMin= concreteFibersSet.getAnchoMecanico() # Enhance (not valid with non-convex sections).
    self.depthUtil= concreteFibersSet.getEffectiveDepth()
    self.mechanicLeverArm= concreteFibersSet.getMechanicLeverArm()
    self.tensionedRebarsArea= tensionedRebarsFiberSet.getArea
    # self.tensionedStrandsArea= 

    self.sigmaXD= N/area+Mz/Iz*centerOfMassY+My/Iy*centerOfMassZ
    self.ultimateShearStrength= getVu(fck,fcd,N,self.concreteArea,self.widthMin,self.depthUtil,self.mechanicLeverArm,self.angAlpha,self.angTheta,self.tensionedRebarsArea,fyd,self.tensionedStrandsArea,fpd,self.sigmaXD,self.sigmaYD,AsTrsv,self.areaShReinfBranchsTrsv,fydTrsv)

  def printParams(self):
    '''Print shear checking values.'''
    print "area of tensioned rebars; As= ",self.tensionedRebarsArea*1e4," cm2"
    print "transverse reinforcement area; AsTrsv= ",self.areaShReinfBranchsTrsv*1e4," cm2"
    print "design value of normal stress; sigmaXD= ",self.sigmaXD/1e6," MPa"
    print "effective depth; d= ",self.depthUtil," m"
    print "minimal width; b0= ",self.widthMin," m"
    print "mechanic lever arm; z= ",self.mechanicLeverArm," m"
    print "shear strength; Vu= ",self.ultimateShearStrength/1e3," kN"

def getF1cdEHE08(fck,fcd):
    '''getF1cdEHE08(fck,fcd)

    :param fck: concrete characteristic compressive strength (Pa).
    :param fcd: design value of concrete compressive strength (N/m2).

    Returns the value of f1cd (design value of the concrete strut strength)
    according to clause 44.2.3.1 of EHE-08.
    '''
    retval=0.6
    if fck>60e6:
      retval=max(0.9-fck/200.e6,0.5)
    retval= retval*fcd
    return retval



def getKEHE08(sgpcd,fcd):
    '''getKEHE08(sgpcd,fcd)
    :param sgpcd: effective normal stress in concrete Ncd/Ac.
    :param fcd: design value of concrete compressive strength (N/m2).

    Return the value of K (coeficiente que depende del esfuerzo axil)
    according to clause 44.2.3.1 de la EHE-08
    '''
    s=-sgpcd/fcd #Positive when compressed
    if s>1:
        lmsg.warning("getKEHE08; too much compression in concrete: ("+str(sgpcd)+"<",str(-fcd)+")\n")
    if s<=0:
        retval=1.0
    elif s<=0.25:
        retval=1+s
    elif s<=0.5:
        retval=1.25
    else:
        retval=2.5*(1-s)
    return retval



def getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta):
    '''getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta) [uds: N, m, rad]

    :param fck: concrete characteristic compressive strength.
    :param fcd: design value of concrete compressive strength (N/m2).
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth.
    :param alpha: angle of the shear reinforcement with the part axis (figure 44.2.3.1 EHE-08).
    :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).

    Return the value of Vu1 (shear strength at failure due to diagonal 
    compression in the web) according to clause 44.2.3.1 of EHE-08.
    '''
    f1cd=getF1cdEHE08(fck,fcd)
    sgpcd=min(Ncd/Ac,0)
    K=getKEHE08(sgpcd,fcd)
    ctgTheta=min(max(1/math.tan(theta),0.5),2.0)
    return K*f1cd*b0*d*(ctgTheta+1/math.tan(alpha))/(1+ctgTheta**2)

def getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac):
    '''getVu2EHE08NoAtNoFis(fctd,I,S,b0,alphal,Ncd,Ac) [uds: N, m, rad]

    :param fctd: design tensile strength of the concrete.
    :param I: Moment of inertia of the section with respect of its centroid.
    :param S: First moment of the section above the center of gravity.
    :param b0: net width of the element according to clause 40.3.5.
    :param alphal: coeficiente que, en su caso, introduce el efecto de la transferencia.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.

    Return the value of Vu2 (shear strength at failure due to tensile force in the web)
    according to clause 44.2.3.2.1.1 of EHE-08.
    '''
    tmp= fctd
    if alphal!=1.0:
      sgpcd=min(Ncd/Ac,0)
      tmp=math.sqrt(fctd**2-alphal*sgpcd*fctd)
    return I*b0/S*tmp



def getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct):
    '''getFcvEHE08(fact,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct) [uds: N, m, rad]
     Return the value of fcv (concrete virtual shear strength)
     for parts WITH or WITHOUT shear reinforcement in cracked regions, according
     to clauses 44.2.3.2.1.2 and 44.2.3.2.2 of EHE-08.

    :param fact: Factor que toma el valor 0.18 for parts WITHOUT shear 
     reinforcement y 0.15 for parts WITH shear reinforcement.
    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param gammaC: Partial safety factor for concrete.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param chi: coefficient that takes into account the aggregate effect
     inside the effective depth.
    :param sgpcd: average axial stress in the web (positive if in compression).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    '''
    rol=min((AsPas+AsAct)/(b0*d),0.02)
    return fact/gammaC*chi*(rol*fcv/1e4)**(1/3)*1e6-0.15*sgpcd


def getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd):
    '''getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)
     Return the minimum value of fcv (concrete virtual shear strength)
     for members WITHOUT shear reinforcement in cracked regions, according to
     clause 44.2.3.2.1.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param gammaC: Partial safety factor for concrete.
    :param d: effective depth (meters).
    :param chi: coefficient that takes into account the aggregate effect
     inside the effective depth.
    :param sgpcd: average axial stress in the web (positive if in compression).
    '''
    return 0.075/gammaC*math.pow(chi,1.5)*math.sqrt(fcv)*1e3-0.15*sgpcd


def getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAtSiFis(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct) [uds: N, m]
     Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for parts WITHOUT shear reinforcement in cracked regions, according to 
     clause 44.2.3.2.1.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.

    '''
    chi=min(2,1+math.sqrt(200/(d*1000.))) #HA DE EXPRESARSE EN METROS.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    fcvTmp=getFcvEHE08(0.18,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    fcvMinTmp=getFcvMinEHE08(fcv,gammaC,d,chi,sgpcd)
    return max(fcvTmp,fcvMinTmp)*b0*d

def getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct):
    '''getVu2EHE08NoAt(M,Mfis,fcv,fck,gammaC,I,S,alphaL,Ncd,Ac,b0,d,AsPas,AsAct)   [uds: N, m, rad]
     Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for members WITHOUT shear reinforcement, according to clauses 
     44.2.3.2.1.1 y 44.2.3.2.1.2 of EHE-08.

    :param M: Bending moment in the section.
    :param Mfis: Cracking moment of the section calculated using fct,d = fct,k/gammaC in
     the same plane that M.
    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fck: concrete characteristic compressive strength.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    '''
    concrTmp= EHE_materials.EHEConcrete("HA",fck,gammaC)
    fctdTmp=concrTmp.fctkEHE08()/gammaC
    fcdTmp=fck/gammaC
    if M<=Mfis:
        retval=getVu2EHE08NoAtNoFis(fctdTmp,I,S,b0,alphaL,Ncd,Ac)
    else:
        retval=getVu2EHE08NoAtSiFis(fck,fcdTmp,1.5,Ncd,Ac,b0,d,AsPas,AsAct)
    return retval


def getVsuEHE08(z,alpha,theta,AsTrsv,fyd):
    '''getVsuEHE08(z,alpha,theta,AsTrsv,fyd)  [uds: N, m, rad]
    Return the value of Vsu (contribution of the web’s transverse 
    reinforcement to shear strength) for parts WITH shear reinforcement,
    according to clause 44.2.3.2.2 of EHE-08.

    :param z: mechanic lever arm.
    :param alpha: angle of the transverse reinforcement with the member axis.
    :param theta: angle between the concrete compressed struts and the 
     member axis (figure 44.2.3.1.a EHE).
    :param AsTrsv: transverse reinforcement area which contribution will be computed.
    :param fyd: design yield strength of the transverse reinforcement.
    '''
    fyalphad=min(fyd,400e6)
    return z*math.sin(alpha)*(1/math.tan(alpha)+1/math.tan(theta))*AsTrsv*fyalphad



def getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]
    Return the longitudinal strain in the web according to
    expression in commments to the clause 44.2.3.2.2 of EHE-08.

    :param Nd: Design value of axial force (here positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
    denomEpsilonX=2*(Es*AsPas+Ep*AsAct) 
    Md= max(Md,Vd*z)
    assert z>0
    assert Ae>0
    if denomEpsilonX>0:
      retvalEpsilonX=(Md/z+math.sqrt(Vd**2+(ue*Td/2.0/Ae)**2)+Nd/2.+Fp)/denomEpsilonX  
    else:
      retvalEpsilonX=0
    retvalEpsilonX= max(0,retvalEpsilonX)
    return retvalEpsilonX
  


def getCrackAngleEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue):
    '''getCrackAngleEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]
     Return the reference angle of inclination of cracks (in radians) from
     the longitudinal strain in the web. See general method in clause 
     44.2.3.2.2 of EHE-08.

    :param Nd: Design value of axial force (here positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.

    '''
    return math.radians(getEpsilonXEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)*7000+29) 



  
def getBetaVcuEHE08(theta,thetaE):
    '''getBetaVcuEHE08(theta,thetaE) [uds: N, m, rad]
     Return the value of «beta» for the expression
     of Vcu according to clause 44.2.3.2.2 of EHE-08.

    :param thetaE: Reference angle of inclination of cracks.
    :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).    
    '''
    cotgTheta=1/math.tan(theta)
    cotgThetaE=1/math.tan(thetaE)
    retval=0.0
    if cotgTheta<0.5:
        lmsg.warning('getBetaVcuEHE08; theta angle is too small.')
    else:
        if(cotgTheta<cotgThetaE):
            retval= (2*cotgTheta-1)/(2*cotgThetaE-1)
        else:
            if cotgTheta<=2.0:
                retval= (cotgTheta-2)/(cotgThetaE-2)
            else:
                print"getBetaVcuEHE08; theta angle is too big."
    return retval
  

 
def getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) 
     [uds: N, m, rad]
     Return the value of Vcu (contribution of the concrete to shear strength)
     for parts WITH shear reinforcement, according to clause 44.2.3.2.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param theta: angle between the concrete compressed struts and the member 
     axis (figure 44.2.3.1.a EHE)
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
  
    chi=min(2,1+math.sqrt(200/(d*1000.)))   #HA DE EXPRESARSE EN METROS.
    sgpcd=max(max(Ncd/Ac,-0.3*fcd),-12e6)
    FcvVcu=getFcvEHE08(0.15,fcv,gammaC,b0,d,chi,sgpcd,AsPas,AsAct)
    thetaEVcu=getCrackAngleEHE08(Nd,Md,Vd,Td,z,AsPas,AsAct,Es,Ep,Fp,Ae,ue)
    betaVcu=getBetaVcuEHE08(theta,thetaEVcu)
    return FcvVcu*betaVcu*b0*d
  


  
def getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''getVu2EHE08SiAt(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]. 
     Return the value of Vu2 (shear strength at failure due to tensile force in the web)
     for parts WITH shear reinforcement, according to clause 44.2.3.2.2 of EHE-08.

    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength).
    :param fyd: design yield strength of the transverse reinforcement.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param AsTrsv: transverse reinforcement area.
    :param alpha: angle of the transverse reinforcement with the axis of the part.
    :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.

    '''
    return getVcuEHE08(fcv,fcd,gammaC,Ncd,Ac,b0,d,AsPas,AsAct,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue)+getVsuEHE08(z,alpha,theta,AsTrsv,fyd)

  
def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue):
    '''def getVuEHE08SiAt(fck,fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv, alpha, theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue) [uds: N, m, rad]
    Return the value of Vu (section shear strength)
    for parts WITH shear reinforcement, according to clause 44.2.3.2.2 of EHE-08.

    :param fck: characteristic value of concrete compressive strength.
    :param fcv: effective concrete shear strength. For members without shear reinforcement
     fcv= min(fck,60MPa). For members with shear reinforcement fcv= min(fck,100MPa).
     In both cases, if concrete quality control is not direct fcv= 15 MPa.
    :param fcd: design value of concrete compressive strength.
    :param fyd: design yield strength of the transverse reinforcement.
    :param gammaC: Partial safety factor for concrete.
    :param Ncd: design value of axial force in concrete (positive if in tension).
    :param Ac: concrete section total area.
    :param b0: net width of the element according to clause 40.3.5.
    :param d: effective depth (meters).
    :param z: Lever arm.
    :param AsPas: Area of tensioned longitudinal steel rebars anchored
     at a distance greater than the effective depth of the section.
    :param AsAct: Area of tensioned longitudinal prestressed steel anchored
     at a distance greater than the effective depth of the section.
    :param AsTrsv: transverse reinforcement area.
    :param alpha: angle of the transverse reinforcement with the axis of the part.
    :param theta: angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    :param Nd: Design value of axial force (positive if in tension)
    :param Md: Absolute value of design value of bending moment.
    :param Vd: Absolute value of effective design shear (clause 42.2.2).
    :param Td: design value of torsional moment.
    :param Es: reinforcing steel elastic modulus.
    :param Ep: prestressing steel elastic modulus.
    :param Fp: Prestressing force on the section (positive if in tension).
    :param Ae: Area enclosed by the middle line of the effective hollow section.
    :param ue: Perimeter of the middle line of the effective hollow section.
    '''
    return  min(getVu2EHE08(fcv,fcd,fyd,gammaC,Ncd,Ac,b0,d,z,AsPas,AsAct,AsTrsv,alpha,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue),getVu1EHE08(fck,fcd,Ncd,Ac,b0,d,alpha,theta))
  

class ShearController(lscb.LimitStateControllerBase):
  '''Shear control according to EHE-08.'''
  def __init__(self,limitStateLabel):
    super(ShearController,self).__init__(limitStateLabel,fakeSection= False)
    self.concreteFibersSetName= "concrete" #Name of the concrete fibers set.
    self.rebarFibersSetName= "reinforcement" #Name of the rebar fibers set.
    self.isBending= False # True if there is ar bending moment.
    self.fcvH= 0.0 # effective concrete shear strength.
    self.fckH= 0.0 # characteristic value of concrete compressive strength.
    self.fcdH= 0.0 # design value of concrete compressive strength.
    self.fctdH= 0.0 # design tensile strength of the concrete.
    self.gammaC= 0.0 # partial safety factor for concrete.
    self.fydS= 0.0 # design value of reinforcement steel strength.
    self.depthUtil= 0.0 # current effective depth
    self.mechanicLeverArm= 0.0 # current value of mechanic lever arm.
    self.strutWidth= 0.0 # compressed strut width «b0».
    self.I= 0.0 # moment of inertia of the section with respect to the neutral axis in elastic range.
    self.S= 0.0 # first moment of area of the section with respect to the neutral axis in elastic range.
    self.concreteArea= 0.0 #Concrete area of the section.
    self.tensionedRebars= lscb.TensionedRebarsBasicProperties()
    self.eps1= 0.0 #Maximum strain in concrete.
    self.concreteAxialForce= 0.0 # Axial force in concrete.
    self.E0= 0.0 #Concrete tangent stiffness.

    self.alphaL= 1.0 #Factor que depende de la transferencia de pretensado.
    self.AsTrsv= 0.0 #Shear reinforcement area.
    self.alpha= math.radians(90) # angle of the shear reinforcement with the part axis (figure 44.2.3.1 EHE-08).
    self.theta= math.radians(45) #Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    self.thetaMin= math.atan(0.5) #Minimal value of the theta angle.
    self.thetaMax= math.atan(2) #Maximal value of the theta angle.

    self.thetaFisuras= 0.0 # angle of the cracks with the part axis.
    self.Vcu= 0.0 # contribution of the concrete to shear strength.
    self.Vsu= 0.0 # contribution of the web’s transverse reinforcement to shear strength.
    self.Vu1= 0.0 # Shear strength at failure due to diagonal compression in the web.
    self.Vu2= 0.0 # Shear strength at failure due to tensile force in the web
    self.Vu= 0.0 # Shear strength at failure.

  def initControlVars(self,elements):
    ''' Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.RCShearControlVars())


  def extractFiberData(self, scc, concrete, reinfSteel):
    ''' Extract basic parameters from the fiber model of the section

     :param scc: fiber model of the section.
     :param concrete: parameters to modelize concrete.
     :param reinfSteel: parameters to modelize reinforcement steel.
    '''
    self.concreteMatTag= concrete.matTagD
    self.fckH= abs(concrete.fck)
    self.fcdH= abs(concrete.fcd())
    self.fctdH= concrete.fctd()
    self.gammaC= concrete.gmmC
    self.reinfSteelMaterialTag= reinfSteel.matTagD
    self.fydS= reinfSteel.fyd()
    if(not scc.hasProp("rcSets")):
        scc.setProp("rcSets", fiber_sets.fiberSectionSetupRC3Sets(scc,self.concreteMatTag,self.concreteFibersSetName,self.reinfSteelMaterialTag,self.rebarFibersSetName))
    return scc.getProp("rcSets")
  
  def calcVuEHE08NoAt(self, scc, concrete, reinfSteel):
    ''' Compute the shear strength at failure without shear reinforcement
     according to clause 44.2.3.2.1 of EHE-08.
     XXX Presstressing contribution not implemented yet.

     :param scc: fiber model of the section.
     :param reinfSteelMaterialTag: reinforcement steel material identifier.
     :param concrete: parameters to modelize concrete.
     :param reinfSteel: parameters to modelize reinforcement steel.
    '''
    rcSets= self.extractFiberData(scc,concrete,reinfSteel)
    concrFibers= rcSets.concrFibers.fSet
    self.concreteArea= rcSets.getConcreteArea(1)
    if(self.concreteArea<1e-6):
      errMsg= "concrete area too smail; Ac= " + str(self.concreteArea) + " m2\n"
      lmsg.error(errMsg)
    else:
      reinfFibers= rcSets.reinfFibers.fSet
      tensionedReinforcement= rcSets.tensionFibers
      self.isBending= scc.isSubjectedToBending(0.1)
      self.tensionedRebars.number= rcSets.getNumTensionRebars()
      self.E0= rcSets.getConcreteInitialTangent()
      if(self.isBending):
        self.eps1= rcSets.getMaxConcreteStrain()
        self.concreteAxialForce= rcSets.getConcreteCompression()
        self.strutWidth= scc.getCompressedStrutWidth() # b0
        if((self.E0*self.eps1)<self.fctdH): # Non cracked section
          self.I= scc.getHomogenizedI(self.E0)
          self.S= scc.getSPosHomogeneizada(self.E0)
          self.Vu2= getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
        else: # Cracked section
          self.depthUtil= scc.getEffectiveDepth() # d
          if(self.tensionedRebars.number>0):
            self.tensionedRebarsArea= tensionedReinforcement.getArea(1)
          else:
            self.tensionedRebarsArea= 0.0
          self.Vu2= getVu2EHE08NoAtSiFis(self.fckH,self.fcdH,self.gammaC,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.tensionedRebarsArea,0.0)
        self.Vsu= 0.0
        self.Vu1= -1.0
      else: # Uncracked section
        axes= scc.getInternalForcesAxes()
        self.I= scc.getFibers().getHomogenizedSectionIRelToLine(self.E0,axes)
        self.S= scc.getFibers().getSPosHomogenizedSection(self.E0,geom.HalfPlane2d(axes))
        self.strutWidth= scc.getCompressedStrutWidth() # b0
        self.Vu2= getVu2EHE08NoAtNoFis(self.fctdH,self.I,self.S,self.strutWidth,self.alphaL,self.concreteAxialForce,self.concreteArea)
      self.Vcu= self.Vu2
      self.Vu= self.Vu2

  def calcVuEHE08SiAt(self, scc, torsionParameters, concrete, reinfSteel, Nd, Md, Vd, Td):
    ''' Compute the shear strength at failure WITH shear reinforcement.
     XXX Presstressing contribution not implemented yet.

     :param scc: fiber model of the section.
     :param torsionParameters: parameters that define torsional behaviour of
                               the section as in clause 45.1 of EHE-08.
     :param reinfSteelMaterialTag: reinforcement steel material identifier.
     :param concrete: concrete material.
     :param reinfSteel: reinforcement steel.
     :param Nd: Design value of axial force (here positive if in tension)
     :param Md: Absolute value of design value of bending moment.
     :param Vd: Absolute value of effective design shear (clause 42.2.2).
     :param Td: design value of torsional moment. '''
    if(torsionParameters):
      self.VuAe= torsionParameters.Ae()
      self.Vuue= torsionParameters.ue()
    else: # XXX Ignore torsional deformation.
      self.VuAe= 1.0
      self.Vuue= 0.0      

    rcSets= self.extractFiberData(scc,concrete,reinfSteel)

    concrFibers= rcSets.concrFibers.fSet
    reinfFibers= rcSets.reinfFibers.fSet
    tensionedReinforcement= rcSets.tensionFibers

    self.isBending= scc.isSubjectedToBending(0.1)
    self.tensionedRebars.number= rcSets.getNumTensionRebars()
    self.concreteArea= rcSets.getConcreteArea(1)
    if(self.concreteArea<1e-6):
      errMsg= "concrete area too smail; Ac= " + str(self.concreteArea) + " m2\n"
      lmsg.error(errMsg)
    else:
      if(self.isBending):
        self.eps1= concrFibers.getStrainMax()
        self.E0= concrFibers[0].getMaterial().getInitialTangent()
        self.concreteAxialForce= concrFibers.getCompressionResultant()
        self.reinforcementElasticModulus= reinfFibers[0].getMaterial().getInitialTangent()
        self.strutWidth= scc.getCompressedStrutWidth() # b0
        self.depthUtil= scc.getEffectiveDepth() # d
        self.mechanicLeverArm= scc.getMechanicLeverArm() # z
        if(self.tensionedRebars.number>0):
          self.tensionedRebars.area= tensionedReinforcement.getArea(1)
        else:
          self.tensionedRebars.area= 0.0
        self.thetaFisuras= getCrackAngleEHE08(Nd,Md,Vd,Td,self.mechanicLeverArm,self.tensionedRebars.area,0.0,self.reinforcementElasticModulus,0.0,0.0,self.VuAe,self.Vuue)
        self.Vcu= getVcuEHE08(self.fckH,self.fcdH,self.gammaC,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.mechanicLeverArm,self.tensionedRebars.area,0.0,self.theta,Nd,Md,Vd,Td,self.reinforcementElasticModulus,0.0,0.0,self.VuAe,self.Vuue)
        self.Vu1= getVu1EHE08(self.fckH,self.fcdH,self.concreteAxialForce,self.concreteArea,self.strutWidth,self.depthUtil,self.alpha,self.theta)
        self.Vsu= getVsuEHE08(self.mechanicLeverArm,self.alpha,self.theta,self.AsTrsv,self.fydS)
        self.Vu2= self.Vcu+self.Vsu
        self.Vu= min(self.Vu1,self.Vu2)
      else: # Uncracked section
        lmsg.error("Checking of shear strength without bending is not implemented.")

  def calcVuEHE08(self, scc, torsionParameters, concrete, reinfSteel, Nd, Md, Vd, Td):
    '''  Compute the shear strength at failure.
     XXX Presstressing contribution not implemented yet.

     :param scc: fiber model of the section.
     :param torsionParameters: parameters that define torsional behaviour of
                               the section as in clause 45.1 of EHE-08.
     :param concrete: parameters to model concrete.
     :param reinfSteel: parameters to model rebar's steel.
     :param Nd: Design value of axial force (positive if in tension)
     :param Md: Absolute value of design value of bending moment.
     :param Vd: Absolute value of effective design shear (clause 42.2.2).
     :param Td: design value of torsional moment.'''
    if(self.AsTrsv==0):
      self.calcVuEHE08NoAt(scc,concrete,reinfSteel)
    else:
      self.calcVuEHE08SiAt(scc,torsionParameters,concrete,reinfSteel,Nd,Md,Vd,Td)


  def check(self,elements,nmbComb):
    ''' For each element in the set 'elememts' passed as first parameter and 
    the resulting internal forces for the load combination 'nmbComb'  
    passed as second parameter, this method calculates all the variables 
    involved in the shear-ULS checking and obtains the capacity factor.
    In the case that the calculated capacity factor is smaller than the 
    smallest obtained for the element in previous load combinations, this value
    is saved in the element results record.  


    Elements processed are those belonging to the phantom model, that is to 
    say, of type xc.ZeroLengthSection. As we have defined the variable 
    fakeSection as False, a reinfoced concrete fiber section is generated
    for each of these elements. 

    XXX Rebar orientation not taken into account yet.
    '''
    lmsg.log("Postprocessing combination: "+nmbComb)
    secHAParamsTorsion= None # XXX Ignore torsional deformation.
    for e in elements:
      R=e.getResistingForce()
      scc= e.getSection()
      idSection= e.getProp("idSection")
      section= scc.getProp("datosSecc")
      concreteCode= section.concrType
      reinforcementCode= section.reinfSteelType
      self.AsTrsv= section.shReinfY.getAs()
      self.alpha= section.shReinfY.angAlphaShReinf
      self.theta= section.shReinfY.angThetaConcrStruts
      NTmp= scc.getStressResultantComponent("N")
      MyTmp= scc.getStressResultantComponent("My")
      MzTmp= scc.getStressResultantComponent("Mz")
      MTmp= math.sqrt((MyTmp)**2+(MzTmp)**2)
      VyTmp= scc.getStressResultantComponent("Vy")
      VzTmp= scc.getStressResultantComponent("Vz")
      VTmp= math.sqrt((VyTmp)**2+(VzTmp)**2)
      TTmp= scc.getStressResultantComponent("Mx")
      #Searching for the best theta angle (concrete strut inclination).
      #We calculate Vu for several values of theta and chose the highest Vu with its associated theta
#      print '\n VTmp=',VTmp
      thetaVuTmp=list()
      self.calcVuEHE08(scc,secHAParamsTorsion,concreteCode,reinforcementCode,NTmp,MTmp,VTmp,TTmp)
      thetaVuTmp.append([self.theta,self.Vu])
      if(self.Vu<VTmp):
        self.theta= max(self.thetaMin,min(self.thetaMax,self.thetaFisuras))
        self.calcVuEHE08(scc,secHAParamsTorsion,concreteCode,reinforcementCode,NTmp,MTmp,VTmp,TTmp)
        thetaVuTmp.append([self.theta,self.Vu])
      if(self.Vu<VTmp):
        self.theta= (self.thetaMin+self.thetaMax)/2.0
        self.calcVuEHE08(scc,secHAParamsTorsion,concreteCode,reinforcementCode,NTmp,MTmp,VTmp,TTmp)
        thetaVuTmp.append([self.theta,self.Vu])
      if(self.Vu<VTmp):
        self.theta= 0.95*self.thetaMax
        self.calcVuEHE08(scc,secHAParamsTorsion,concreteCode,reinforcementCode,NTmp,MTmp,VTmp,TTmp)
        thetaVuTmp.append([self.theta,self.Vu])
      if(self.Vu<VTmp):
        self.theta= 1.05*self.thetaMin
        self.calcVuEHE08(scc,secHAParamsTorsion,concreteCode,reinforcementCode,NTmp,MTmp,VTmp,TTmp)
        thetaVuTmp.append([self.theta,self.Vu])
      self.theta,self.Vu=max(thetaVuTmp, key=lambda item: item[1])
#      print 'thetaFisuras=',self.thetaFisuras
#      print 'theta=',self.theta
#      print 'Vu=', self.Vu
      VuTmp= self.Vu
      if(VuTmp!=0.0):
        FCtmp= VTmp/VuTmp
      else:
        FCtmp= 1e99
#      print 'FCtmp=', FCtmp
      Mu= 0.0 #Apparently EHE doesn't use Mu
      if(FCtmp>=e.getProp(self.limitStateLabel).CF):
        e.setProp(self.limitStateLabel,cv.RCShearControlVars(idSection,nmbComb,FCtmp,NTmp,MyTmp,MzTmp,Mu,VyTmp,VzTmp,self.theta,self.Vcu,self.Vsu,VuTmp)) # Worst case


class CrackStraightController(lscb.LimitStateControllerBase):
  '''Definition of variables involved in the verification of the cracking
  serviceability limit state according to EHE-08 when considering a concrete
  stress-strain diagram that takes account of the effects of tension stiffening.
  '''
  def __init__(self,limitStateLabel):
    super(CrackStraightController,self).__init__(limitStateLabel,fakeSection= False)
    self.beta=1.7    #if only indirect actions beta must be =1.3

  def initControlVars(self,elements):
    ''' Initialize control variables over elements.

      :param elements: elements to define control variables in
    '''
    for e in elements:
      e.setProp(self.limitStateLabel,cv.RCCrackStraightControlVars())

  def EHE_hceff(self,width,h,x):
    '''Return the maximum height to be considered in the calculation of the 
    concrete effective area in tension.
    
    :param width: section width
    :param h: lever arm
    :param x: depth of the neutral fiber.

    '''
    if width>2*h:   #slab, flat beam
      hceff=min(abs(h+x),h/4.)
    else:
      hceff=min(abs(h+x),h/2.)
    return hceff

  def EHE_k1(self,eps1,eps2):
    '''Return the coefficient k1 involved in the calculation of the mean 
    crack distance according to EHE. This coefficient represents the effect of 
    the tension diagram in the section.

    :param eps1: maximum deformation calculated in the section at the limits 
           of the tension zone
    :param eps2: minimum deformation calculated in the section at the limits 
           of the tension zone
    '''
    k1= (eps1+eps2)/(8.0*eps1)
    return k1

  def check(self,elements,nmbComb):
    ''' For each element in the set 'elememts' passed as first parameter and 
    the resulting internal forces for the load combination 'nmbComb'  
    passed as second parameter, this method calculates all the variables 
    involved in the crack-SLS checking and obtains the crack width.
    In the case that the calculated crack width is greater than the 
    biggest obtained for the element in previous load combinations, this value
    is saved in the element results record. 

    Elements processed are those belonging to the phantom model, that is to 
    say, of type xc.ZeroLengthSection. As we have defined the variable 
    fakeSection as False, a reinfoced concrete fiber section is generated
    for each of these elements. 
    '''
    lmsg.log("Postprocessing combination: "+nmbComb)
    for e in elements:
      Aceff=0  #init. value
      R=e.getResistingForce()
      sct=e.getSection()
      sctCrkProp=lscb.fibSectLSProperties(sct)
      sctCrkProp.setupStrghCrackDist()
      hceff=self.EHE_hceff(sct.getAnchoMecanico(),sctCrkProp.h,sctCrkProp.x)
      Acgross=sct.getGrossEffectiveConcreteArea(hceff)
      Aceff=sct.getNetEffectiveConcreteArea(hceff,"tensSetFb",15.0)
      concrete=EHE_materials.concrOfName[sctCrkProp.concrName]
      rfSteel=EHE_materials.steelOfName[sctCrkProp.rsteelName]
      k1=self.EHE_k1(sctCrkProp.eps1,sctCrkProp.eps2)
      '''
      print 'element= ', e.tag
      print 'Resisting force: [', R[0] , ',', R[1] , ',', R[2] , ',', R[3] , ',', R[4] , ',', R[5], ',',R[6],']'
      print 'N= ', sct.getStressResultantComponent("N")
      print 'My= ',sct.getStressResultantComponent("My")
      print 'Mz= ',sct.getStressResultantComponent("Mz")
      print 'hceff= ',hceff
      print 'Acgross= ',Acgross
      print 'Aceff= ',Aceff
      print 'concrete=',concrete
      print 'eps1=',sctCrkProp.eps1
      print 'eps2=',sctCrkProp.eps2
      print 'As= ', sctCrkProp.As
      print 'cover= ',sctCrkProp.cover
      print 'spacing= ',sctCrkProp.spacing
      print 'fiEqu= ',sctCrkProp.fiEqu
      rfset=sct.getFiberSets()["reinfSetFb"]
      eps_sm=rfset.getStrainMax()
      print 'max. strain= ', eps_sm
      '''
      if Aceff<=0:
        s_rmax=0
      else:
        ro_s_eff=sctCrkProp.As/Aceff      #effective ratio of reinforcement
        sm=2*sctCrkProp.cover+0.2*sctCrkProp.spacing+0.4*k1*sctCrkProp.fiEqu/ro_s_eff
        s_rmax=self.beta*sm
        #Parameters for tension stiffening of concrete
        paramTS= concrete_base.paramTensStiffness(concrMat=concrete,reinfMat=rfSteel,reinfRatio=ro_s_eff,diagType='K')
        concrete.tensionStiffparam=paramTS #parameters for tension stiffening
                                         #are assigned to concrete
        ftdiag=concrete.tensionStiffparam.pointOnsetCracking()['ft']      #stress at the adopted point for concrete onset cracking
        Etsdiag=abs(concrete.tensionStiffparam.regresLine()['slope'])
        '''
        print 'effective ratio of reinforcement=', ro_s_eff
        print 'ft=',ftdiag*1e-6
        print 'Ets0=', Etsdiag*1e-6
        '''
        fiber_sets.redefTensStiffConcr(setOfTenStffConcrFibSect=sctCrkProp.setsRC.concrFibers,ft=ftdiag,Ets=Etsdiag)
      e.setProp('ResF',R)   #vector resisting force
      e.setProp('s_rmax',s_rmax)  #maximum crack distance
    self.preprocessor.getDomain.revertToStart()
    predefined_solutions.resuelveComb(self.preprocessor,nmbComb,self.analysis,1)
    for e in elements:
      sct=e.getSection()
      rfset=sct.getFiberSets()["reinfSetFb"]
      eps_sm=rfset.getStrainMax()
      srmax=e.getProp("s_rmax")
      wk=srmax*eps_sm
#      print ' eps_sm= ',eps_sm, ' srmax= ', srmax, ' wk= ',wk
      if (wk>e.getProp(self.limitStateLabel).wk):
#        e.setProp(self.limitStateLabel,cv.RCCrackStraightControlVars(idSection,nmbComb,NTmp,MyTmp,MzTmp,srmax,eps_sm,wk))
        R=e.getProp('ResF')
        e.setProp(self.limitStateLabel,cv.RCCrackStraightControlVars(idSection=e.getProp("idSection"),combName=nmbComb,N=-R[0],My=-R[4],Mz=-R[5],s_rmax=srmax,eps_sm=eps_sm,wk=wk))
      '''
      print 'element= ', e.tag
      print 'max. strain= ', eps_sm
      print 'crack widths: ',wk*1e3, ' mm'
      R=e.getResistingForce()
      print 'Resisting force: [', R[0] , ',', R[1] , ',', R[2] , ',', R[3] , ',', R[4] , ',', R[5], ',',R[6],']'
      '''
      
class CrackControl(lscb.CrackControlBaseParameters):
  '''
  Define the properties that will be needed for crack control checking
  as in clause 49.2.4 of EHE-08.'''

  def __init__(self,limitStateLabel):
    super(CrackControl,self).__init__(limitStateLabel)
    self.concreteArea= 0.0 # Concrete section area.
    self.fctmFis= 0.0 # Average tensile strength of the concrete.
    self.tensionedRebars= lscb.TensionedRebarsProperties()
    self.eps1= 0.0 # Maximum strain in concrete.
    self.eps2= 0.0 # Minimum strain in concrete.
    self.k1= 0.0 # Coefficient representing the effect of the tension diagram in the section.
    self.k2= 0.5 # Coefficient of value 1.0 in the case of non-repeating
                 # temporary load and 0.5 otherwise.
    self.depthMecanico= 0.0 # mechanic section depth.
    self.widthMecanico= 0.0 # mechanic section width.
    self.aspectRatio= 0.0 #aspect ratio width/depth.
    self.hEfMax= 0.0 # maximum depth of effective area.
    self.grossEffectiveArea= 0.0 # gross effective area.
    self.netEffectiveArea= 0.0 # net effective area.
    self.E0= 0.0 # Concrete tangent stiffness.
    self.beta= 1.7 # Coefficient which relates the mean crack opening to the
                   # characteristic value and is equal to 1.3 in the case of
                   # cracking caused by indirect actions only, and 1.7 otherwise.
    self.Wk= 0.0 # Characteristic crack opening.
    
  def printParams(self):
    ''' Prints crack control parameters.'''
    self.tensionedRebars.printParams()    
    print "Maximum concrete strain in the cracked section inside the tensioned zone; eps1= ",self.eps1*1e3," per mil.\n"
    print "Minimum concrete strain in the cracked section inside the tensioned zone; eps2= ",self.eps2*1e3," per mil.\n"
    print "Effect of the tension diagram; k1= ",self.k1,"\n"
    print "Mechanic depth; h= ",self.depthMecanico," m\n"
    print "Lever arm; z= ",self.mechanicLeverArm," m\n"
    print "Mechanical width; b= ",self.widthMecanico," m\n"
    print "Aspect ratio; r= ",self.aspectRatio,"\n"
    print "Maximum depth for effective area; hEfMax= ",self.hEfMax," m\n"
    print "Average tensile strength of the concrete; fctm= ",self.fctmFis/1e6," MPa\n"
    print "Concrete tangent stiffness; E0= ",self.E0/1e9," GPa\n"
    print "Characteristic crack opening; Wk= ",self.Wk*1e3," mm\n"

  def computeWkOnBars(self,tensionedReinforcement):
    '''Compute the characteristic crack opening on each bar and return
       the maximum.

     :param tensionedReinforcement: 
    '''
    retval= 0.0
    sz= len(tensionedReinforcement)
    for i in range(0,sz):
      barra= tensionedReinforcement[i]
      AsBarra= barra.getArea()
      posBarra= barra.getPos()
      yBarra= posBarra.x
      zBarra= posBarra.y
      sigmaBarra= barra.getMaterial().getStress()
      coverBarra= tensionedReinforcement.getFiberCover(i)
      diamBarra= tensionedReinforcement.getEquivalentDiameterOfFiber(i)
      sigmaSRBarra= tensionedReinforcement.getSigmaSRAtFiber(i,self.E0,self.tensionedRebars.E,self.fctmFis)
      self.tensionedRebars.averageStress+= AsBarra*sigmaSRBarra

      AcEfBarra= tensionedReinforcement.getFiberEffectiveConcreteArea(i)
      sepBarra= min(tensionedReinforcement.getFiberSpacing(i),15*diamBarra)
      smFisurasBarra= 2*coverBarra+0.2*sepBarra+0.4*self.k1*diamBarra*AcEfBarra/AsBarra
      maxBarElongation= sigmaBarra/self.tensionedRebars.E
      averageBarElongation= max(1.0-self.k2*(sigmaSRBarra/sigmaBarra)**2,0.4)*maxBarElongation
      WkBarra=  self.beta*smFisurasBarra*averageBarElongation
      retval= max(retval,WkBarra)
    self.tensionedRebars.averageStress/= self.tensionedRebars.area
    return retval

  def computeWk(self, scc, concreteMatTag, reinfSteelMaterialTag, fctm):
    '''Computes the characteristic value of the crack width.

     :param scc: section.
     :param reinfSteelMaterialTag: identifier of the concrete material.
     :param reinfSteelMaterialTag: identifier of the reinforcing steel material.
     :param fctm: average tensile strength of the concrete.
    '''
    if(self.rcSets == None):
      self.rcSets= fiber_sets.fiberSectionSetupRC3Sets(scc,concreteMatTag,self.concreteFibersSetName,reinfSteelMaterialTag,self.rebarFibersSetName)
    concrFibers= self.rcSets.concrFibers.fSet
    reinfFibers= self.rcSets.reinfFibers.fSet
    tensionedReinforcement= self.rcSets.tensionFibers

    self.fctmFis= fctm
    self.claseEsfuerzo= scc.getStrClaseEsfuerzo(0.0)
    self.tensionedRebars.number= self.rcSets.getNumTensionRebars()
    self.Wk= 0.0
    if(self.tensionedRebars.number>0):
      scc.computeCovers(self.tensionedRebarsFiberSetName)
      scc.computeSpacement(self.tensionedRebarsFiberSetName)
      self.eps1= concrFibers.getStrainMax()
      self.eps2= max(concrFibers.getStrainMin(),0.0)
      self.k1= (self.eps1+self.eps2)/8/self.eps1
      self.E0= concrFibers[0].getMaterial().getInitialTangent()
      self.concreteArea= concrFibers.getArea(1.0)
      self.depthMecanico= scc.getLeverArm()
      self.mechanicLeverArm= scc.getMechanicLeverArm() # z
      self.widthMecanico= scc.getAnchoMecanico()
      self.aspectRatio= self.widthMecanico/self.depthMecanico
      if(self.aspectRatio>1):
        self.hEfMax= self.depthMecanico/4.0 # Pieza tipo losa
      else:
        self.hEfMax= self.depthMecanico/2.0
      self.netEffectiveArea= scc.computeFibersEffectiveConcreteArea(self.hEfMax,self.tensionedRebarsFiberSetName,15)

      self.tensionedRebars.setup(tensionedReinforcement)
      self.Wk= self.computeWkOnBars(tensionedReinforcement)

  def check(self,elements,nmbComb):
    ''' Crack control of concrete sections.'''
    lmsg.log("Postprocessing combination: "+nmbComb+"\n")

    defParamsFisuracion("secHAParamsFisuracion")
    materiales= preprocessor.getMaterialHandler
    concrete= materiales.getMaterial(concreteCode)
    concrTag= concrete.getProp("matTagK")
    concrFctm= concrete.getProp("fctm")
    reinforcement= materiales.getMaterial(reinforcementCode)
    for e in elements:
      scc= elements.getSeccion()
      Ntmp= scc.N
      MyTmp= scc.My
      MzTmp= scc.Mz
      secHAParamsFisuracion= computeWk(concrTag,reinforcementTag,concrFctm)
      Wk= secHAParamsFisuracion.Wk
      if(Wk>WkCP):
        WkCP= Wk # Worst case
        HIPCP= nmbComb
        NCP= Ntmp
        MyCP= MyTmp
        MzCP= MzTmp

#class CrackControlTensStiff(lscb.CrackControlBase):
  

def printParamFisBarra():
  '''Prints crack control parameters of a bar.'''
  print "\niBarra= ",iBarra,"\n"
  print "Effective area Acef= ",AcEfBarra*1e4," cm2\n"
  print "Bar area As= ",AsBarra*1e4," cm2\n"
  print "Bar position: (",yBarra,",",zBarra,")\n"
  print "Bar cover c= ",coverBarra," m\n"
  print "Bar diameter fi= ",diamBarra,"\n"
  print "Bar stress= ",sigmaBarra/1e6," MPa\n"
  print "Bar stress_SR= ",sigmaSRBarra/1e6," MPa\n"
  print "Bar spacement s= ",sepBarra," m\n"
  print "k1= ",k1,"\n"
  print "smFisurasBarra= ",smFisurasBarra," m\n"
  print "Maximum bar elongation: ",maxBarElongation*1e3," por mil.\n"
  print "Average bar elongation: ",averageBarElongation*1e3," por mil.\n"
  print "Characteristic crack width= ",WkBarra*1e3," mm\n\n"


class TorsionParameters(object):
  '''Methods for checking reinforced concrete section under torsion according to
     clause 45.1 of EHE-08.'''
  def __init__(self):
    self.h0= 0.0  # Real wall thickess.
    self.c= 0.0  # Longitudinal reinforcement concrete cover.

    self.crossSectionContour= geom.Poligono2d()  # Cross section contour.
    self.lineaMedia=  geom.Poligono2d() # Polygon defined by the midline of the effective hollow section.
    self.lineaInt=  geom.Poligono2d() # Polygon defined by the interior contour of the effective hollow section.
    self.effectiveHollowSection= geom.PoligonoConAgujeros2d() # Effective hollow section contour
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

def computeEffectiveHollowSectionParameters(sectionGeometry, h0, c):
  '''Computes the parameter for torsion analysis of an
   effective hollow section. Not valid if for non-convex sections.

  :param sectionGeometry: section geometry.
  :param h0: real thickness of the section wall.
  :param c: cover of the longitudinal reinforcement.
  '''
  retval= TorsionParameters()
  retval.h0= h0
  retval.c= c
  retval.crossSectionContour= sectionGeometry.getRegionsContour()
  he= retval.he()
  retval.lineaMedia= retval.crossSectionContour.offset(-he/2)
  retval.lineaInt= retval.crossSectionContour.offset(-he)
  retval.effectiveHollowSection.contour(retval.crossSectionContour)
  retval.effectiveHollowSection.addHole(retval.lineaInt)
  return retval

class ColumnReinforcementRatios(object):
  def __init__(self,Ac,fcd,fyd):
    '''Constructor.

    :param Ac: concrete area.
    :param fcd: design value of concrete compressive strength (N/m2).
    :param fyd: design yield strength (Pa)
    '''
    self.Ac= Ac
    self.fcd= fcd
    self.fyd= fyd
  def getMinimumGeometricAmount(self):
    ''' Minimum geometric reinforcement amount for columns
        according to table 42.3.5 of EHE-08.'''
    return 0.004*self.Ac 

  def getMinimumMechanicalAmount(self,Nd):
    ''' Minimum mechanical reinforcement amount.'''
    return 0.1*Nd/self.fyd

  def getMaximumReinforcementAmount(self):
    '''Return the maximal reinforcement amount.'''
    return min((self.fcd*self.Ac)/self.fyd,0.08*self.Ac)

  def check(self, As, Nd):
    '''Checking of main reinforcement ratio in compression.'''
    ctMinGeom= self.getMinumumGeometricAmount()
    if(As<ctMinGeom):
      msg= "Reinforcement area: "+str(As*1e4)+" cm2, gives a ratio below minimal geometric reinforcement amount: "+str(ctMinGeom*1e4)+" cm2\n"
      lmsg.warning(msg)
    ctMinMec= self.getMinimumMechanicalAmount(Nd)
    if(As<ctMinMec):
      msg= "Reinforcement area: "+str(As*1e4)+" cm2, gives a ratio below minimal mechanical reinforcement amount: "+str(ctMinMec*1e4)+" cm2\n"
      lmsg.warning(msg)
    ctMax= self.getMaximumReinforcementAmount()
    if(As>ctMax):
      msg= "Reinforcement area: "+str(As*1e4)+" cm2, gives a ratio above the maximum mechanical reinforcement ratio: "+str(ctMax*1e4)+" cm2\n"
      lmsg.warning(msg)


class ConcreteCorbel(object):
    '''Concrete corbel as in EHE-08 design code.'''
    def __init__(self,concrete, steel, jointType):
        '''
        Constructor.

        :param concrete: concrete material for corbel.
        :param concrete: steel material for corbel reinforcement.
        :param jointType: corbel to column joint quality
          ("monolitica", "junta" o "junta_debil").
        '''
        self.concrete= concrete
        self.steel= steel
        self.jointType= jointType
        
    def getCotgStrutAngle(self):
        '''getCotgStrutAngle()
 
          Return the cotangent of the angle between the concrete
          compressed strut an the vertical according to 
          clause 64.1.2.1 of EHE-08.
        '''
        if self.jointType=="monolitica":
            retval=1.4
        elif self.jointType=="junta":
            retval=1.0
        elif self.jointType=="junta_debil":
            retval=0.6
        return retval

    def getMinimumEffectiveDepth(self, a):
        '''getMinimumEffectiveDepth(self, a) return the minimal effective 
        depth of the corbel according to clause 64.1.2.1 of EHE-08.

        :param a: Distance (m) between the axis of the applied load and
                  the corbel fixed section (see figure 64.1.2 of EHE-08).

        '''
        return a*self.getCotgStrutAngle()/0.85

    def getMainReinforcementTension(self, Fv,Fh):
        '''getMainReinforcementTension(self, Fv,Fh). Return the tension
         in the main reinforcement according to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).        
        '''
        return Fv/self.getCotgStrutAngle()+Fh

    def getAreaNecMainReinforcement(self, Fv,Fh):
        '''getAreaNecMainReinforcement(self, Fv,Fh): return the area needed
         for the main reinforcement according to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        :param Fh: Horizontal load on the corbel, positive outwards (N).
        '''
        return self.getMainReinforcementTension(Fv,Fh)/min(self.steel.fyd(),400e6)
    @staticmethod
    def getStirrupsTension(Fv):
        ''' getStirrupsTension(Fv): return the tension in the stirrups
         to clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return 0.2*Fv

    def getAreaNecCercos(self,Fv):
        '''getAreaNecCercos(self,Fv): return the area needed for the 
         stirrup reinforcements bars in the corbel according to 
         clause 64.1.2.1.1 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return self.getStirrupsTension(Fv)/min(self.steel.fyd(),400e6)

    def getAreaNecApoyo(self,Fv):
        '''getAreaNecApoyo(self,Fv): return the area needed for the
         support according to clause 64.1.2.1.2 of EHE-08.

        :param Fv: Vertical load on the corbel, positive downwards (N).
        '''
        return Fv/0.7/-self.concrete.fcd()

EC2table48_x= [12.0e6,16.0e6,20.0e6,25.0e6,30.0e6,35.0e6,40.0e6,45.0e6,50.0e6]
EC2table48_y= [0.18,0.22,0.26,0.30,0.34,0.37,0.41,0.44,0.48]
EC2table48= scipy.interpolate.interp1d(EC2table48_x,EC2table48_y)


def shearBetweenWebAndFlangesStrength(fck,gammac,hf,Asf,Sf,fyd):
    ''' Return the shear strength (kN/m) in the flange web contact by unit 
    length according to clause 4.3.2.5 of Eurocode 2

    Parameters:
    :param fck: concrete characteristic compressive strength (Pa).
    :param gammac: Partial safety factor for concrete.
    :param hf: flange thickness (m)
    :param Asf: reinforcement that cross the section by unit length (m2)
    :param Sf: spacement of the rebars that cross the section (m)
    :param fyd: design yield strength (Pa)
    '''
    hf=hf*1000     #Flange thickness in mm
    #shear strength at failure due to diagonal compression in the web.
    fcd=fck/gammac
    VRd2=0.2*fcd*hf
    #Section tensile strength
    taoRd= EC2table48(fck)
    VRd3=2.5*taoRd*hf+Asf/Sf*fyd
    return min(VRd2,VRd3)

#Example:
#  esfRasMax= shearBetweenWebAndFlangesStrength(25e6,1.5,0.3,565e-6,0.2,500e6)


# Checking of solid block members under concentrated loads, according to clause
# 61 of EHE-08


class BlockMember(object):
  def __init__(self,a,b,a1,b1):
    '''
    Constructor.

    Parameters:
      :param a: side length.
      :param a1: lenght of the side of the loaded area parallel to a.
      :param b: side length.
      :param b1: lenght of the side of the loaded area parallel to b.
    '''
    self.a= a
    self.a1= a1
    self.b= b
    self.b1= b1
  def getAc(self):
    '''Return area of the block member.'''
    return self.a*self.b
  def getAc1(self):
    '''Return block member loaded area.'''
    return self.a1*self.b1
  def getF3cd(self, fcd):
    '''Return the value of f3cd.'''
    return min(sqrt(self.getAc()/self.Ac1()),3.3)*fcd
  def getNuConcentratedLoad(self, fcd):
    '''
    Return the the maximum compressive force that can be obtained in the
    Ultimate Limit State of on a restricted surface (see figure 61.1.a on
    page 302 of EHE-08), of area Ac1 , concentrically and homothetically 
    situated on another area, Ac.

    Parameters:
    :param fcd: design compressive strength of concrete.
    '''
    return self.getAc1()*self.getF3cd(fcd)
  def getUad(self, Nd):
      '''
      Return the design tension for the transverse reinforcement in
      a direction parallel to side a (see figure 61.1.a page 302 EHE-08).

      Parameters:
        :param Nd: concentrated load.
      '''
      return 0.25*((self.a-self.a1)/self.a)*Nd
  def getReinforcementAreaAd(self, Nd, fyd):
      '''
      Return the area of the reinforcement parallel to side a
      (see figure 61.1.a page 302 EHE-08)

        :param Nd: concentrated load.
        :param fyd: steel yield strength.
      '''
      return self.getUad(Nd)/min(fyd,400e6)
  def getUbd(self, Nd):
      '''
      Return the design tension for the transverse reinforcement in
      a direction parallel to side b (see figure 61.1.a page 302 EHE-08).

        :param Nd: concentrated load.
      '''
      return 0.25*((self.b-self.b1)/self.b)*Nd
  def getReinforcementAreaBd(Nd, fyd):
      '''
      Return the area of the reinforcement parallel to side b
      (see figure 61.1.a page 302 EHE-08)

        :param Nd: concentrated load.
        :param fyd: steel yield strength.
      '''
      return self.getUbd(Nd)/min(fyd,400e6)
