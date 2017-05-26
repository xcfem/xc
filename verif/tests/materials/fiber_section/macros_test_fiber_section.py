# -*- coding: utf-8 -*-
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def extractFiberSectionProperties(fiberSec,scc):
  fibers= fiberSec.getFibers()
  global nFibers; nFibers= fibers.getNumFibers()
  global sumAreas; sumAreas= fibers.getSumaAreas(1)
  global Iz; Iz= fibers.getIz
  global Iy; Iy= fibers.getIy 
  global Pyz; Pyz= fibers.getPyz
  global zCdg; zCdg= fibers.getCdgZ()
  global yCdg; yCdg= fibers.getCdgY()
  global I1; I1= fibers.getI1(1,0,0)
  global I2; I2= fibers.getI2(1,0,0)
  global i1; i1= math.sqrt(I1/sumAreas)  # Radio de giro eje principal mayor
  global i2; i2= math.sqrt(I2/sumAreas); # Radio de giro eje principal menor
  # th1= th1; 
  global Me1; Me1= 2*fy/scc.h*I1; # Momento elástico de la sección en torno al eje principal mayor.
  global Me2; Me2= 2*fy/scc.b*I2; # Momento elástico de la sección en torno al eje principal menor.
  global SzPosG; SzPosG= fibers.getSzPos(0,0,1)
  global SyPosG; SyPosG= fibers.getSyPos(0,0,1) 

def printRatios(scc):
  print "areaTeor= ",(scc.area())
  print "sumAreas= ",(sumAreas)
  print "ratio1= ",(ratio1)
  print "yCdg= ",(yCdg)
  print "yCdgTeor= ",(yCdgTeor)
  print "ratio2= ",(ratio2)
  print "zCdg= ",(zCdg)
  print "zCdgTeor= ",(zCdgTeor)
  print "ratio3= ",(ratio3)
  print "I1= ",(I1)
  print "I1Teor= ",(scc.I1())
  print "ratio4= ",(ratio4)
  print "I2= ",(I2)
  print "scc.I2()= ",(scc.I2())
  print "ratio5= ",(ratio5)
  # print "th1= ",(th1)}
  print "i1= ",(i1)
  print "i1Teor= ",(scc.i1())
  print "ratio6= ",(ratio6)
  print "i2= ",(i2)
  print "i2Teor= ",(scc.i2())
  print "ratio7= ",(ratio7)
  print "Me1= ",(Me1)
  print "Me1Teor= ",(scc.Me1(fy))
  print "ratio8= ",(ratio8)
  print "Me2= ",(Me2)
  print "Me2Teor= ",(scc.Me2(fy))
  print "ratio9= ",(ratio9)
  print "SzPosG= ",(SzPosG)
  print "SzPosGTeor= ",(scc.S1PosG())
  print "ratio10= ",(ratio10)
  print "SyPosG= ",(SyPosG)
  print "SyPosGTeor= ",(scc.S2PosG())
  print "ratio11= ",(ratio11)
#     print "Mp1= ",(Mp1/100/1000)
  print "Mp1Teor= ",(scc.Mp1(fy)/100/1000)
  print "ratio12= ",(ratio12)
#  print "Mp2= ",(Mp2/100/1000)
  print "Mp2Teor= ",(scc.Mp2(fy)/100/1000)
  print "ratio13= ",(ratio13)


