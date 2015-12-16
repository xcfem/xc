# -*- coding: utf-8 -*-

from rough_calculations import ng_rebar_def
from materials.sia262 import concreteSIA262
from materials.sia262 import steelSIA262
from materials.sia262 import minimal_reinforcement
from postprocess.reports import common_formats as fmt


class RCSection(object):
  tensionRebars= None
  beton= concreteSIA262.c25_30
  exigeanceFisuration= "B"
  b= 0.25
  h= 0.25
  def __init__(self,tensionRebars,beton,exigFis,b,h):
    self.tensionRebars= tensionRebars
    self.beton= beton
    self.exigeanceFisuration= exigFis
    self.b= b
    self.h= h
  def setArmature(self,tensionRebars):
    self.tensionRebars= tensionRebars
  def getAsMinFlexion(self):
    return self.tensionRebars.getAsMinFlexion(self.beton,self.exigeanceFisuration,self.h)
  def getAsMinTraction(self):
    return self.tensionRebars.getAsMinTraction(self.beton,self.exigeanceFisuration,self.h)
  def getMR(self):
    return self.tensionRebars.getMR(self.beton,self.b,self.h)
  def getVR(self,Nd,Md):
    return self.tensionRebars.getVR(self.beton,Nd,Md,self.b,self.h)
  def writeResultFlexion(self,outputFile,Nd,Md,Vd):
    famArm= self.tensionRebars
    beton= self.beton
    AsMin= self.getAsMinFlexion()
    ancrage= famArm.getBasicAnchorageLength(beton)
    outputFile.write("  Dimensions coupe; b= "+ fmt.Longs.format(self.b)+ "m, h= "+ fmt.Longs.format(self.h)+ "m\\\\\n")
    ng_rebar_def.writeRebars(outputFile,beton,famArm,AsMin)
    if(abs(Md)>0):
      MR= self.getMR()
      outputFile.write("  Verif. en flexion: Md= "+ fmt.Esf.format(Md/1e3)+ "kN m, MR= "+ fmt.Esf.format(MR/1e3)+ "kN m")
      ng_rebar_def.writeF(outputFile,"  F(M)", MR/Md)
    if(abs(Vd)>0):
      VR= self.getVR(Nd,Md)
      outputFile.write("  Vérif. eff. tranchant: Vd= "+ fmt.Esf.format(Vd/1e3)+ "kN,  VR= "+ fmt.Esf.format(VR/1e3)+ "kN")
      ng_rebar_def.writeF(outputFile,"  F(V)",VR/Vd)
  def writeResultTraction(self,outputFile,Nd):
    famArm= self.tensionRebars
    beton= self.beton
    AsMin= self.getAsMinTraction()/2
    ancrage= famArm.getBasicAnchorageLength(beton)
    ng_rebar_def.writeRebars(outputFile,beton,famArm,AsMin)
    if(abs(Nd)>0):
      print "ERROR; tension not implemented."

