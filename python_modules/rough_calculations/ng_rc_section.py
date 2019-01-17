# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from rough_calculations import ng_rebar_def
from materials.sia262 import SIA262_materials
from postprocess.reports import common_formats as fmt
from miscUtils import LogMessages as lmsg


class RCSection(object):
  tensionRebars= None
  concrete= SIA262_materials.c25_30
  b= 0.25
  h= 0.25
  def __init__(self,tensionRebars,concrete,b,h):
    self.tensionRebars= tensionRebars
    self.concrete= concrete
    self.b= b
    self.h= h
    self.stressLimitUnderPermanentLoads= 230e6
  def setArmature(self,tensionRebars):
    self.tensionRebars= tensionRebars
  def getMinReinfAreaUnderFlexion(self):
    return self.tensionRebars.getMinReinfAreaUnderFlexion(self.concrete,self.h)
  def getMinReinfAreaUnderTension(self):
    return self.tensionRebars.getMinReinfAreaUnderTension(self.concrete,self.h)
  def getMR(self):
    return self.tensionRebars.getMR(self.concrete,self.b,self.h)
  def getVR(self,Nd,Md):
    return self.tensionRebars.getVR(self.concrete,Nd,Md,self.b,self.h)
  def writeResultFlexion(self,outputFile,Nd,Md,Vd):
    famArm= self.tensionRebars
    concrete= self.concrete
    AsMin= self.getMinReinfAreaUnderFlexion()
    ancrage= famArm.getBasicAnchorageLength(concrete)
    outputFile.write("  Dimensions coupe; b= "+ fmt.Longs.format(self.b)+ "m, h= "+ fmt.Longs.format(self.h)+ "m\\\\\n")
    ng_rebar_def.writeRebars(outputFile,concrete,famArm,AsMin)
    if(abs(Md)>0):
      MR= self.getMR()
      outputFile.write("  Verif. en flexion: Md= "+ fmt.Esf.format(Md/1e3)+ " kN m, MR= "+ fmt.Esf.format(MR/1e3)+ "kN m")
      ng_rebar_def.writeF(outputFile,"  F(M)", MR/Md)
    if(abs(Vd)>0):
      VR= self.getVR(Nd,Md)
      outputFile.write("  Vérif. eff. tranchant: Vd= "+ fmt.Esf.format(Vd/1e3)+ "kN,  VR= "+ fmt.Esf.format(VR/1e3)+ "kN")
      ng_rebar_def.writeF(outputFile,"  F(V)",VR/Vd)
  def writeResultTraction(self,outputFile,Nd):
    famArm= self.tensionRebars
    concrete= self.concrete
    AsMin= self.getMinReinfAreaUnderTension()/2
    ancrage= famArm.getBasicAnchorageLength(concrete)
    ng_rebar_def.writeRebars(outputFile,concrete,famArm,AsMin)
    if(abs(Nd)>0):
      lmsg.error("ERROR; tension not implemented.")
  def writeResultCompression(self,outputFile,Nd,AsTrsv):
    ''' Results for compressed rebars.

    :param AsTrsv: Rebar area in transverse direction.
     '''
    famArm= self.tensionRebars #Even if they're not in tension...
    concrete= self.concrete
    AsMin= 0.2*AsTrsv # 20% of the transversal area.
    ng_rebar_def.writeRebars(outputFile,concrete,famArm,AsMin)
    if(abs(Nd)!=0.0):
      lmsg.error("ERROR; not implemented.")
      
  def writeResultStress(self,outputFile,M):
    '''Cheking of stresses under permanent loads (SIA 262 fig. 31)'''
    concrete= self.concrete
    if(abs(M)>0):
      stress= M/(0.9*self.h*self.tensionRebars.getAs())
      outputFile.write("  Verif. contraintes: M= "+ fmt.Esf.format(M/1e3)+ " kN m, $\sigma_s$= "+ fmt.Esf.format(stress/1e6)+ " MPa\\\\\n")
      outputFile.write("    $\sigma_{lim}$= "+ fmt.Esf.format(self.stressLimitUnderPermanentLoads/1e6)+ " MPa")
      ng_rebar_def.writeF(outputFile,"  F($\sigma_s$)", self.stressLimitUnderPermanentLoads/stress)
