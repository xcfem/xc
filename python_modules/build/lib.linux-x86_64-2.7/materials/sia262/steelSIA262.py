# -*- coding: utf-8 -*-

import math
import scipy.interpolate

x= [50e-3,100e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,435e6,435e6,435e6,435e6]
courbeA= scipy.interpolate.interp1d(x,y)

x= [50e-3,100e-3,130e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,435e6,400e6,340e6,280e6,260e6]
courbeB= scipy.interpolate.interp1d(x,y)

x= [50e-3,55e-3,100e-3,150e-3,200e-3,250e-3,300e-3]
y= [435e6,435e6,290e6,230e6,190e6,160e6,140e6]
courbeC= scipy.interpolate.interp1d(x,y)


def limitationContraintes(exigence,s):
  if(exigence=="A"):
    return courbeA(s)
  elif (exigence=="B"):
    return courbeB(s)
  elif (exigence=="C"):
    return courbeC(s)
  else:
    print "Value for exigence: '",exigence,"' unknown." 

class ReinforcingSteelSIA262(object):
  name= ""
  fsk= 500e6
  ks= 1.05
  eud= 0.02
  gammaS= 1.15
  def __init__(self, name,fsk,ks,eud):
    self.name= name
    self.fsk= fsk
    self.ks= ks
    self.eud= eud
  def __repr__(self):
    return name
  def fsd(self):
    return self.fsk/self.gammaS
  def Es(self):
    return 205e9

B500A= ReinforcingSteelSIA262("B500A",500e6,1.05,0.02)
B500B= ReinforcingSteelSIA262("B500B",500e6,1.08,0.045)
B500C= ReinforcingSteelSIA262("B500C",500e6,1.15,0.065)
B700B= ReinforcingSteelSIA262("B700B",700e6,1.08,0.045)

diametres= [6e-3,8e-3,10e-3,12e-3,14e-3,16e-3,18e-3,20e-3,22e-3,26e-3,30e-3,34e-3,40e-3]

section_barres_courantes={}

for d in diametres:
  section_barres_courantes[d]= math.pi*(d/2.0)**2

def numBars(AsNec):
  retval= []
  for d in diametres:
    a= math.pi*(d/2.0)**2
    n= math.ceil(AsNec/a)
    retval.append((d,n))
  return retval
