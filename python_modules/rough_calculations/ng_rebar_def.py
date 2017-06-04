# -*- coding: utf-8 -*-
from __future__ import division
from materials.sia262 import SIA262_materials
from materials.sia262 import minimal_reinforcement
from materials.sia262 import shearSIA262
from materials.sia262 import anchorage
from rough_calculations import ng_simple_bending_reinforcement
from postprocess.reports import common_formats as fmt
import math


class RebarFamily(object):
  minDiams= 50
  def __init__(self,acier,diam,ecartement,enrobage,exigenceFissuration= 'B'):
    self.acier= acier
    self.diam= diam
    self.ecartement= ecartement
    self.enrobage= enrobage
    self.exigenceFissuration= exigenceFissuration
  def __repr__(self):
    return self.acier.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.ecartement*1e3))
  def getCopy(self,exigenceFissuration):
    return RebarFamily(self.acier,self.diam,self.ecartement,self.enrobage,exigenceFissuration)
  def getDiam(self):
    return self.diam
  def getBarArea(self):
    return math.pi*(self.diam/2.0)**2
  def getNumBarsPerMeter(self):
    return 1.0/self.ecartement
  def getAs(self):
    return self.getNumBarsPerMeter()*self.getBarArea()
  def getBasicAnchorageLength(self,beton):
    return max(anchorage.getBasicAnchorageLength(self.getDiam(),beton.fck,self.acier.fyd()),self.minDiams*self.diam)
  def getExigenceFissuration(self):
    return self.exigenceFissuration
  def getAsMinFlexion(self,beton,epaisseur):
    retval= minimal_reinforcement.AsMinFlexion(beton,self.getEnrobageMec(),self.exigenceFissuration,self.ecartement,epaisseur)
    return retval
  def getAsMinTraction(self,beton,epaisseur):
    retval= minimal_reinforcement.AsMinTraction(beton,self.exigenceFissuration,self.ecartement,epaisseur)
    return retval
  def getMR(self,beton,b,epaisseur):
    return ng_simple_bending_reinforcement.Mu(self.getAs(),beton.fcd(),self.acier.fyd(),b,epaisseur-self.getEnrobageMec())
  def getEnrobageMec(self):
    return self.enrobage+self.diam/2.0
  def d(self,epaisseur):
    return epaisseur-self.getEnrobageMec()
  def getT(self):
    return self.getAs()*self.acier.fyd()
  def getVR(self,beton,Nd,Md,b,epaisseur):
    return shearSIA262.VuNoShearRebars(beton,self.acier,Nd,Md,self.getAs(),b,self.d(epaisseur))
  def getDefStr(self):
    #return definition strings for drawSchema.
    return ("  $\\phi$ "+ fmt.Diam.format(self.getDiam()*1000) + " mm, e= "+ fmt.Diam.format(self.ecartement*1e2)+ " cm")
  def getDefStrings(self):
    #return definition strings for drawSchema.
    retval= []
    retval.append(self.getDefStr())
    retval.append(" - ")
    return retval
  def writeDef(self,outputFile,beton):
    outputFile.write("  diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, ecartement: "+ fmt.Diam.format(self.ecartement*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(beton)
    outputFile.write("  l. ancrage L="+ fmt.Longs.format(ancrage) + " m ("+ fmt.Diam.format(ancrage/self.getDiam())+ " diamètres).\\\\\n")

class FamNBars(RebarFamily):
  n= 2 #Number of bars.
  def __init__(self,acier,n,diam,ecartement,enrobage):
    RebarFamily.__init__(self,acier,diam,ecartement,enrobage)
    self.n= int(n)
  def __repr__(self):
    return str(n) + " x " + self.acier.name + ", diam: " + str(int(self.diam*1e3)) + " mm, e= " + str(int(self.ecartement*1e3))
  def writeDef(self,outputFile,beton):
    outputFile.write("  n= "+str(self.n)+" diam: "+ fmt.Diam.format(self.getDiam()*1000) + " mm, ecartement: "+ fmt.Diam.format(self.ecartement*1e3)+ " mm")
    ancrage= self.getBasicAnchorageLength(beton)
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
  def getEnrobageMec(self):
    T1= self.f1.getT()
    T2= self.f2.getT()
    T= T1+T2
    return (self.f1.getEnrobageMec()*T1+self.f2.getEnrobageMec()*T2)/T
  def getBasicAnchorageLength(self,beton):
    l1= self.f1.getBasicAnchorageLength(beton)
    l2= self.f2.getBasicAnchorageLength(beton)
    return max(l1,l2)
  def getAsMinFlexion(self,beton,epaisseur):
    retval= minimal_reinforcement.AsMinFlexion(beton,self.getEnrobageMec(),self.f1.exigenceFissuration,self.getEcartement(),epaisseur)
    return retval
  def getAsMinTraction(self,beton,epaisseur):
    retval= minimal_reinforcement.AsMinTraction(beton,self.f1.exigenceFissuration,self.getEcartement(),epaisseur)
    return retval
  def getExigenceFissuration(self):
    retval= self.f1.exigenceFissuration
    if(retval!=self.f2.exigenceFissuration):
      cmsg.error("Different specifications for crack control.")
    return retval
  def getMR(self,beton,b,epaisseur):
    MR1= self.f1.getMR(beton,b,epaisseur)
    MR2= self.f2.getMR(beton,b,epaisseur)
    return MR1+MR2
  def d(self,epaisseur):
    return epaisseur-self.getEnrobageMec()
  def getVR(self,beton,Nd,Md,b,epaisseur):
    assert self.f1.acier==self.f2.acier
    return shearSIA262.VuNoShearRebars(beton,self.f1.acier,Nd,Md,self.getAs(),b,self.d(epaisseur))
  def getDefStrings(self):
    #return definition strings for drawSchema.
    retval= []
    retval.append(self.f1.getDefStr())
    retval.append(self.f2.getDefStr())
    return retval

  def writeDef(self,outputFile,beton):
    self.f1.writeDef(outputFile,beton)
    self.f2.writeDef(outputFile,beton)

def writeRebars(outputFile,beton,famArm,AsMin):
  famArm.writeDef(outputFile,beton)
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
