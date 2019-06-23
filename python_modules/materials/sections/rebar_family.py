# -*- coding: utf-8 -*-
'''Definition of typical reinforcement schemes.'''
from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2018, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from rough_calculations import ng_simple_bending_reinforcement
from postprocess.reports import common_formats as fmt
import math

class RebarFamily(object):
  ''' Family or reinforcement bars.

    :ivar steel: reinforcing steel material.
    :ivar diam: diameter of the bars.
    :ivar spacing: spacing of the bars.
    :ivar concreteCover: concrete cover of the bars.
  '''
  minDiams= 50
  def __init__(self,steel,diam,spacing,concreteCover):
    ''' Constructor.

    :param steel: reinforcing steel material.
    :param diam: diameter of the bars.
    :param spacing: spacing of the bars.
    :param concreteCover: concrete cover of the bars.
    '''
    self.steel= steel
    self.diam= diam
    self.spacing= spacing
    self.concreteCover= concreteCover
  def __repr__(self):
    return self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
  def getCopy(self,barController):
    return RebarFamily(self.steel,self.diam,self.spacing,self.concreteCover,barController)
  def getDiam(self):
    ''' Return the diameter of the bars.'''
    return self.diam
  def getBarArea(self):
    ''' Return the area of each bar.'''
    return math.pi*(self.diam/2.0)**2
  def getNumBarsPerMeter(self):
    ''' Return the number of bars per unit length.'''
    return 1.0/self.spacing
  def getAs(self):
    ''' Return the total area of the bars.'''
    return self.getNumBarsPerMeter()*self.getBarArea()
  def getMR(self,concrete,b,thickness):
    '''Return the bending resistance of the (b x thickness) rectangular section.

    :param concrete: concrete material.
    :param b: width of the rectangular section.
    :param thickness: height of the rectangular section.
    '''
    return ng_simple_bending_reinforcement.Mu(self.getAs(),concrete.fcd(),self.steel.fyd(),b,thickness-self.getEffectiveCover())
  def getEffectiveCover(self):
    ''' returns the effective cover of the rebar family.

    Returns the distance between the surface of the concrete and the 
    centroid of the rebars family.
    '''
    return self.concreteCover+self.diam/2.0
  def d(self,thickness):
    return thickness-self.getEffectiveCover()
  def getT(self):
    ''' Return the tension force in the reinforcement.'''
    return self.getAs()*self.steel.fyd()

  def getDefStr(self):
    ''' Return definition strings for drawSchema.'''
    return ("  $\\phi$ "+ fmt.Diam.format(self.getDiam()*1000) + " mm, e= "+ fmt.Diam.format(self.spacing*1e2)+ " cm")
  def getDefStrings(self):
    ''' Return definition strings for drawSchema.'''
    retval= []
    retval.append(self.getDefStr())
    retval.append(" - ")
    return retval
  def writeDef(self,outputFile,concrete):
    outputFile.write("  diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(concrete)
    outputFile.write("  l. ancrage L="+ fmt.Lengths.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")

class FamNBars(RebarFamily):
  n= 2 #Number of bars.
  def __init__(self,steel,n,diam,spacing,concreteCover):
    RebarFamily.__init__(self,steel,diam,spacing,concreteCover)
    self.n= int(n)
  def __repr__(self):
    return str(n) + " x " + self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.spacing*1e3))
  def writeDef(self,outputFile,concrete):
    outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, spacing: "+ fmt.Diam.format(self.spacing*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(concrete)
    outputFile.write("  l. ancrage L="+ fmt.Lengths.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")


class DoubleRebarFamily(object):
  ''' Two reinforcement bars families.

  :ivar f1: first rebar family.
  :ivar f2: second rebar family.
  '''
  def __init__(self,f1,f2):
    ''' Constructor.

    :param f1: first rebar family.
    :param f2: second rebar family.
    '''
    self.f1= f1
    self.f2= f2
  def __repr__(self):
    return self.f1.__repr__() + " + " + self.f2.__repr__()
  def getCopy(self,barController):
    return DoubleRebarFamily(self.f1.getCopy(barController),self.f2.getCopy(barController))
  def getAs(self):
    ''' Return the total area of the bars.'''
    return self.f1.getAs()+self.f2.getAs()
  def getSpacing(self):
    ''' Return the average spacing of the bars.'''
    n1= self.f1.getAs()/self.f1.getBarArea()
    n2= self.f2.getAs()/self.f2.getBarArea()
    return 1/(n1+n2)
  def getEffectiveCover(self):
    ''' returns the effective cover of the rebar family.

    Returns the distance between the surface of the concrete and the 
    centroid of the rebars family.
    '''
    T1= self.f1.getT()
    T2= self.f2.getT()
    T= T1+T2
    return (self.f1.getEffectiveCover()*T1+self.f2.getEffectiveCover()*T2)/T
  def getBasicAnchorageLength(self,concrete):
    ''' Return the basic anchorage length of the bars.'''
    l1= self.f1.getBasicAnchorageLength(concrete)
    l2= self.f2.getBasicAnchorageLength(concrete)
    return max(l1,l2)
  def getMinReinfAreaUnderFlexion(self,concrete,thickness):
    '''Return the minimun amount of bonded reinforcement to control cracking
       for reinforced concrete sections under flexion.

    :param concrete: concrete material.
    :param thickness: thickness of the bended member.
    '''
    retval= self.f1.getMinReinfAreaUnderFlexion(concrete= concrete,thickness= thickness)
    return retval
  def getMinReinfAreaUnderTension(self,concrete,thickness):
    '''Return the minimun amount of bonded reinforcement to control cracking
       for reinforced concrete sections under tension.

    :param concrete: concrete material.
    :param thickness: thickness of the tensioned member.
    '''
    retval= self.f1.getMinReinfAreaUnderTension(concrete= concrete,thickness= thickness)
    return retval
  def getMR(self,concrete,b,thickness):
    '''Return the bending resistance of the (b x thickness) rectangular section.

    :param concrete: concrete material.
    :param b: width of the rectangular section.
    :param thickness: height of the rectangular section.
    '''
    MR1= self.f1.getMR(concrete,b,thickness)
    MR2= self.f2.getMR(concrete,b,thickness)
    return MR1+MR2
  def d(self,thickness):
    return thickness-self.getEffectiveCover()

  def getDefStrings(self):
    ''' Return definition strings for drawSchema.'''
    retval= []
    retval.append(self.f1.getDefStr())
    retval.append(self.f2.getDefStr())
    return retval

  def writeDef(self,outputFile,concrete):
    self.f1.writeDef(outputFile,concrete)
    self.f2.writeDef(outputFile,concrete)

def writeF(outputFile,text,F):
  fmt= "{:4.2f}"
  if(F>1):
    outputFile.write(text+ "= "+ fmt.format(F)+ " Ok!\\\\\n")
  elif(F>=0.95):
    outputFile.write(text+ "= "+ fmt.format(F)+ " $\\sim$ Ok!\\\\\n")
  else:
    outputFile.write(text+ "= "+ fmt.format(F)+ " Erreur!\\\\\n")
