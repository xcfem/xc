# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import scipy.optimize as opt
import math

class StressCalc(object):
  b= 10
  h= 0.8
  r= 0.05
  rp= 0.05
  As= 15700e-6
  Asp= 0
  Ec=30891e6
  Es=200000e6
  N= 30500e3
  M= 16250e3-N*(0.65-h/2.0)

  sgc= 0.0
  sgc0= 0.0
  xx= 0.0
  sgs= 0.0
  sgsp= 0.0
  

  def __init__(self,b,h,r,rp,As,Asp,Ec,Es):
    self.b= b
    self.h= h
    self.r= r
    self.rp= rp
    self.As= As
    self.Asp= Asp
    self.Ec= Ec
    self.Es= Es

    self.sgc= 0.0
    self.sgc0= 0.0
    self.xx= 0.0
    self.sgs= 0.0
    self.sgsp= 0.0

  def swap(self):
    tmp= self.As
    self.As= self.Asp
    self.Asp= tmp
    tmp= self.r
    self.r= self.rp
    self.rp= tmp
    tmp= self.sgs
    self.sgs= self.sgsp
    self.sgsp= tmp
    self.M= -self.M

  def Ac(self):
    return self.b*self.h
  def Ic(self):
    return 1/12.0*self.b*self.h**3

  def totAs(self):
    return self.As+self.Asp
  def totIs(self):
    retval= self.As*(self.h/2.0-self.r)**2+self.Asp*(self.h/2.0-self.rp)**2
    return retval
  def getYCentroidAs(self):
    return (self.As*self.getYs()+self.Asp*self.getYsp())/(self.As+self.Asp)

  def getNc(self):
    retval= 0.0
    if(self.xx>=self.h):
      retval= (self.sgc+self.sgc0)/2.0*self.Ac()
    elif(self.xx>0.0):
      retval= 1/2.0*self.sgc*self.xx*self.b 
    return retval

  def getMc(self):
    retval= 0.0
    sg= self.sgc-self.sgc0
    if(self.xx>=self.h):
      retval= -(1/2.0*self.h*sg*self.b)*h/6.0
    elif(self.xx>0.0):
      y= math.copysign(self.h/2.0-self.xx/3.0,self.M)
      retval= -self.getNc()*y
    return retval


  def getYs(self):
    return (self.h/2-self.r)
  def getNs(self):
    return self.As*self.sgs
  def getMs(self):
    return self.getNs()*self.getYs()

  def getYsp(self):
    return (self.rp-self.h/2)
  def getNsp(self):
    return self.Asp*self.sgsp
  def getMsp(self):
    return self.getNsp()*self.getYsp()

  def resistingM(self):
    Mc= self.getMc()
    retval= Mc +self.getMs()+self.getMsp()
    if(abs(self.getNc())>0.1):
      retval-= self.N*self.getYCentroidAh()
    return retval
  def resistingN(self):
    return self.getNc()+self.getNs()+self.getNsp()

  def getAh(self):
    n= self.Es/self.Ec
    return self.totAs()*n+self.b*self.h
  def getYCentroidAh(self):
    n= self.Es/self.Ec
    return n*(self.As*self.getYs()+self.Asp*self.getYsp())/(n*(self.As+self.Asp)+self.Ac())
  def getIh(self):
    n= self.Es/self.Ec
    y= self.getYCentroidAs()
    retval= self.totIs()*n+self.totAs()*y*y
    retval+= self.Ic()
    return retval

  def xElasticNeutralAxis(self):
    A= self.getAh()
    I= self.getIh()
    y= -self.N*I/(self.M*A)
    return self.h/2.0+y

  def elasticStressAs(self):
    n= self.Es/self.Ec
    centerOfMassYh= self.getYCentroidAh()
    return n*(self.N/self.getAh()+self.M/self.getIh()*(self.getYs()-centerOfMassYh))
  def elasticStressAsp(self):
    n= self.Es/self.Ec
    centerOfMassYh= self.getYCentroidAh()
    return n*(self.N/self.getAh()+self.M/self.getIh()*(self.getYsp()-centerOfMassYh))
  def elasticStressAc(self):
    centerOfMassYh= self.getYCentroidAh()
    return self.N/self.getAh()+self.M/self.getIh()*(centerOfMassYh-self.h/2.0)
  def elasticStressAc0(self):
    centerOfMassYh= self.getYCentroidAh()
    return self.N/self.getAh()+self.M/self.getIh()*(self.h/2.0-centerOfMassYh)

  def inTraction(self):
    return (self.elasticStressAs()>0.0 and self.elasticStressAsp()>0.0)
  def inCompression(self):
    return (self.elasticStressAs()<0.0 and self.elasticStressAsp()<0.0)


  def residX(self,x):
    ds= self.h-r
    dsp= self.h-rp
    self.sgc= 2*self.Ec*x*self.N/(self.b*self.Ec*x**2+(2*self.Asp-2*self.As)*self.Es*x-2*self.Asp*self.Es*self.rp+2*self.As*ds*self.Es)
    Nc= 1/2.0*self.sgc*x*self.b
    Mc= Nc*(self.h/2.0-x/3.0)
    if(self.As!=0.0):
      self.sgs= -self.sgc*self.Es*(ds-x)/(self.Ec*x)
      Ns= self.As*self.sgs
      Ms= Ns*(self.h/2-self.r)
    if(self.Asp!=0.0):
      self.sgsp= self.sgc*self.Es*(x-self.rp)/(self.Ec*x)
      Nsp= self.Asp*self.sgsp
      Msp= Nsp*(self.h/2-self.rp)
    self.xx= x
    # print "residN(x= ", x ,")= ", (self.N-Nc+Ns-Nsp)/1e3
    # print "residM(x= ", x ,")= ", (self.M+Mc-Ms+Msp)/1e3
    return self.M+Mc-Ms+Msp

  def solve(self,N, M):
    self.N= N
    self.M= M
    if(self.inTraction()):
      AT= self.totAs()
      if(AT!=0.0):
        ys= self.getYs()
        ysp= self.getYsp()
        self.sgsp= (M-N*ys)/(ysp-ys)/self.Asp
        self.sgs= (N-self.sgsp*self.Asp)/self.As
        self.sgc= 0.0
    elif(self.inCompression()):
      self.xx= 10*self.h #
      self.sgs= self.elasticStressAs()
      self.sgsp= self.elasticStressAsp()
      self.sgc= self.elasticStressAc()
      self.sgc0= self.elasticStressAc0()
    elif(abs(self.N)<1e-3): # flexion simple
      if(self.M>0):
        d= self.h-self.r
        self.xx= 0.6*d
        self.sgs= self.M/(0.8*self.As*d)
        Ns= self.getNs()
        self.sgc= -2*self.Ec*Ns*self.xx/(self.b*self.Ec*self.xx**2+2*self.Asp*self.Es*self.xx-2*self.Asp*self.Es*self.rp)
        self.sgsp= self.sgc*self.Es*(self.xx-self.rp)/(self.Ec*self.xx)
      else:      
        d= self.h-self.rp
        ds= self.h-self.r
        self.xx= 0.6*d
        self.sgsp= -self.M/(0.8*self.Asp*0.9*self.h)
        Nsp= self.getNsp()
        self.sgc= -2*self.Ec*Nsp*self.xx/(self.b*self.Ec*self.xx**2+2*self.As*self.Es*self.xx-2*self.As*self.Es*self.r)
        self.sgs= self.sgc*self.Es*(ds-self.xx)/(self.Ec*self.xx)
      self.sgc0= 0.0
    else: #Flexion compuesta
      sg1= self.N/self.Ac()+self.M/self.Ic()*self.h/2
      sg2= self.N/self.Ac()-self.M/self.Ic()*self.h/2
      sgMax= max(sg1,sg2)
      sgMin= min(sg1,sg2)
      mx= max(abs(sgMax),abs(sgMin))
      if(mx<=1e6):
        self.sgc= self.elasticStressAc()
        self.sgs= self.elasticStressAs()
        self.sgsp= self.elasticStressAsp()
        self.xx= self.xElasticNeutralAxis()
      else:
        #self.xx= opt.newton(s.residX,self.h/2.0)
        if(self.M>=0):
          #self.xx= opt.bisect(s.residX,self.rp,self.h,xtol= 0.01)
          xx1= self.xElasticNeutralAxis()
          sgc1= self.elasticStressAc()
          sgs1= self.elasticStressAs()
          sgsp1= self.elasticStressAsp()
          d= self.h-self.r
          xx2= 0.6*d
          sgs2= self.M/(0.8*self.As*d)
          Ns2= self.getNs()
          sgc2= -2*self.Ec*Ns2*xx2/(self.b*self.Ec*xx2**2+2*self.Asp*self.Es*xx2-2*self.Asp*self.Es*self.rp)
          sgsp2= self.sgc*self.Es*(xx2-self.rp)/(self.Ec*xx2)
          self.xx= (xx1+xx2)/2.0
          self.sgc= (sgc1+sgc2)/2.0
          self.sgs= (sgs1+sgs2)/2.0
          self.sgsp= (sgsp1+sgsp2)/2.0
        else:
          self.swap()
          #self.xx= opt.bisect(s.residX,self.rp,self.h,xtol= 0.01)
          xx1= self.xElasticNeutralAxis()
          sgc1= self.elasticStressAc()
          sgs1= self.elasticStressAs()
          sgsp1= self.elasticStressAsp()
          d= self.h-self.r
          xx2= 0.6*d
          sgs2= self.M/(0.8*self.As*d)
          Ns2= self.getNs()
          sgc2= -2*self.Ec*Ns2*xx2/(self.b*self.Ec*xx2**2+2*self.Asp*self.Es*xx2-2*self.Asp*self.Es*self.rp)
          sgsp2= self.sgc*self.Es*(xx2-self.rp)/(self.Ec*xx2)
          self.xx= (xx1+xx2)/2.0
          self.sgc= (sgc1+sgc2)/2.0
          self.sgs= (sgs1+sgs2)/2.0
          self.sgsp= (sgsp1+sgsp2)/2.0
          self.swap()
    #self.verif()

  def verif(self):
    NR= s.resistingN()
    errN= abs(self.N-NR)
    pc= errN/max(abs(self.N),1)
    if(pc>0.05):
      print "ERROR N= ", N, " NR= ", NR, "errN= ", errN, "(",pc*100,"%)"
    MR= s.resistingM()
    errM= abs(self.M-MR)
    pc= errM/max(abs(self.M),1)
    if(pc>0.05):
      print "ERROR M= ", M, " MR= ", MR, "errM= ", errM, "(",pc*100,"%)"
