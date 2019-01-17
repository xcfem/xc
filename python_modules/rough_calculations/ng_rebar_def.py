# -*- coding: utf-8 -*-
from __future__ import division
from materials.sia262 import SIA262_materials
from materials.sia262 import SIA262_limit_state_checking
from rough_calculations import ng_simple_bending_reinforcement
from postprocess.reports import common_formats as fmt
import math


class RebarFamily(object):
  minDiams= 50
  def __init__(self,steel,diam,ecartement,concreteCover,exigenceFissuration= 'B'):
    self.steel= steel
    self.diam= diam
    self.ecartement= ecartement
    self.concreteCover= concreteCover
    self.exigenceFissuration= exigenceFissuration
  def __repr__(self):
    return self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.ecartement*1e3))
  def getCopy(self,exigenceFissuration):
    return RebarFamily(self.steel,self.diam,self.ecartement,self.concreteCover,exigenceFissuration)
  def getDiam(self):
    return self.diam
  def getBarArea(self):
    return math.pi*(self.diam/2.0)**2
  def getNumBarsPerMeter(self):
    return 1.0/self.ecartement
  def getAs(self):
    return self.getNumBarsPerMeter()*self.getBarArea()
  def getBasicAnchorageLength(self,concrete):
    return max(SIA262_limit_state_checking.getBasicAnchorageLength(self.getDiam(),concrete.fck,self.steel.fyd()),self.minDiams*self.diam)
  def getExigenceFissuration(self):
    return self.exigenceFissuration
  def getMinReinfAreaUnderFlexion(self,concrete,epaisseur):
    retval= SIA262_limit_state_checking.MinReinfAreaUnderFlexion(concrete,self.getEffectiveCover(),self.exigenceFissuration,self.ecartement,epaisseur)
    return retval
  def getMinReinfAreaUnderTension(self,concrete,epaisseur):
    retval= SIA262_limit_state_checking.MinReinfAreaUnderTension(concrete,self.exigenceFissuration,self.ecartement,epaisseur)
    return retval
  def getMR(self,concrete,b,epaisseur):
    return ng_simple_bending_reinforcement.Mu(self.getAs(),concrete.fcd(),self.steel.fyd(),b,epaisseur-self.getEffectiveCover())
  def getEffectiveCover(self):
    ''' returns the effective cover of the rebar family.

    Returns the distance between the surface of the concrete and the 
    centroid of the rebars family.
    '''
    return self.concreteCover+self.diam/2.0
  def d(self,epaisseur):
    return epaisseur-self.getEffectiveCover()
  def getT(self):
    return self.getAs()*self.steel.fyd()
  def getVR(self,concrete,Nd,Md,b,epaisseur):
    return SIA262_limit_state_checking.VuNoShearRebars(concrete,self.steel,Nd,Md,self.getAs(),b,self.d(epaisseur))
  def getDefStr(self):
    #return definition strings for drawSchema.
    return ("  $\\phi$ "+ fmt.Diam.format(self.getDiam()*1000) + " mm, e= "+ fmt.Diam.format(self.ecartement*1e2)+ " cm")
  def getDefStrings(self):
    #return definition strings for drawSchema.
    retval= []
    retval.append(self.getDefStr())
    retval.append(" - ")
    return retval
  def writeDef(self,outputFile,concrete):
    outputFile.write("  diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, ecartement: "+ fmt.Diam.format(self.ecartement*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(concrete)
    outputFile.write("  l. ancrage L="+ fmt.Longs.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")

class FamNBars(RebarFamily):
  n= 2 #Number of bars.
  def __init__(self,steel,n,diam,ecartement,concreteCover):
    RebarFamily.__init__(self,steel,diam,ecartement,concreteCover)
    self.n= int(n)
  def __repr__(self):
    return str(n) + " x " + self.steel.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.ecartement*1e3))
  def writeDef(self,outputFile,concrete):
    outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, ecartement: "+ fmt.Diam.format(self.ecartement*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(concrete)
    outputFile.write("  l. ancrage L="+ fmt.Longs.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")


class DoubleRebarFamily(object):
  def __init__(self,f1,f2):
    self.f1= f1
    self.f2= f2
  def __repr__(self):
    return self.f1.__repr__() + " + " + self.f2.__repr__()
  def getCopy(self,exigenceFissuration):
    return DoubleRebarFamily(self.f1.getCopy(exigenceFissuration),self.f2.getCopy(exigenceFissuration))
  def getAs(self):
    return self.f1.getAs()+self.f2.getAs()
  def getEcartement(self):
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
    l1= self.f1.getBasicAnchorageLength(concrete)
    l2= self.f2.getBasicAnchorageLength(concrete)
    return max(l1,l2)
  def getMinReinfAreaUnderFlexion(self,concrete,epaisseur):
    retval= SIA262_limit_state_checking.MinReinfAreaUnderFlexion(concrete,self.getEffectiveCover(),self.f1.exigenceFissuration,self.getEcartement(),epaisseur)
    return retval
  def getMinReinfAreaUnderTension(self,concrete,epaisseur):
    retval= SIA262_limit_state_checking.MinReinfAreaUnderTension(concrete,self.f1.exigenceFissuration,self.getEcartement(),epaisseur)
    return retval
  def getExigenceFissuration(self):
    retval= self.f1.exigenceFissuration
    if(retval!=self.f2.exigenceFissuration):
      cmsg.error("Different specifications for crack control.")
    return retval
  def getMR(self,concrete,b,epaisseur):
    MR1= self.f1.getMR(concrete,b,epaisseur)
    MR2= self.f2.getMR(concrete,b,epaisseur)
    return MR1+MR2
  def d(self,epaisseur):
    return epaisseur-self.getEffectiveCover()
  def getVR(self,concrete,Nd,Md,b,epaisseur):
    assert self.f1.steel==self.f2.steel
    return SIA262_limit_state_checking.VuNoShearRebars(concrete,self.f1.steel,Nd,Md,self.getAs(),b,self.d(epaisseur))
  def getDefStrings(self):
    #return definition strings for drawSchema.
    retval= []
    retval.append(self.f1.getDefStr())
    retval.append(self.f2.getDefStr())
    return retval

  def writeDef(self,outputFile,concrete):
    self.f1.writeDef(outputFile,concrete)
    self.f2.writeDef(outputFile,concrete)

def writeRebars(outputFile,concrete,famArm,AsMin):
  famArm.writeDef(outputFile,concrete)
  outputFile.write("  area: As= "+ fmt.Areas.format(famArm.getAs()*1e4) + " cm2/m areaMin("+famArm.getExigenceFissuration()+"): " + fmt.Areas.format(AsMin*1e4) + " cm2/m")
  writeF(outputFile,"  F(As)", famArm.getAs()/AsMin)

def writeF(outputFile,text,F):
  fmt= "{:4.2f}"
  if(F>1):
    outputFile.write(text+ "= "+ fmt.format(F)+ " Ok!\\\\\n")
  elif(F>=0.95):
    outputFile.write(text+ "= "+ fmt.format(F)+ " $\\sim$ Ok!\\\\\n")
  else:
    outputFile.write(text+ "= "+ fmt.format(F)+ " Erreur!\\\\\n")
