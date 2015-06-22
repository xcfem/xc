# -*- coding: utf-8 -*-

import math

class ConcreteSIA262(object):
  name= ""
  fck= 20e6
  gammaC= 1.5
  Dmax= 35e-3
  def __init__(self, name, fck):
    self.name= name
    self.fck= fck
  def nufc(self):
    return min(math.pow(30e6/self.fck,1/3.0),1.0)
  def fcm(self):
    return self.fck+8e6
  def fcd(self):
    return self.fck/self.gammaC
  def fctm(self):
    return 0.3*math.pow(self.fck/1e6,2/3.0)*1e6
  def fctk005(self):
    # SIA 262 section 3.1.2.2.5 (7)
    return 0.7*self.fctm()
  def fctk095(self):
    # SIA 262 section 3.1.2.2.5 (8)
    return 1.3*self.fctm()
  def fctd(self,t,favourable):
    # SIA 262 sections 4.4.1.3 et 4.4.1.4
    kt= 1.0/(1.0+0.5*t) #(99)
    retval= 0.0
    if favourable:
      retval= kt*self.fctm() #(98)
    else:
      retval= kt*self.fctk095() #(99)
    return retval
  def taucd(self):
    return 0.3*math.sqrt(self.fck/1e6)*1e6/self.gammaC
  def kDmax(self):
    return 48/(self.Dmax*1e3+16)

c12_15= ConcreteSIA262("C12/15",12e6)
c16_20= ConcreteSIA262("C16/20",16e6)
c20_25= ConcreteSIA262("C20/25",20e6)
c25_30= ConcreteSIA262("C25/30",25e6)
c30_37= ConcreteSIA262("C30/37",30e6)
c35_45= ConcreteSIA262("C35/45",35e6)
c40_50= ConcreteSIA262("C40/50",40e6)
c45_55= ConcreteSIA262("C45/55",45e6)
c50_60= ConcreteSIA262("C50/60",50e6)

def reductionFactorKT(t):
  return 1/(1+0.5*t)
