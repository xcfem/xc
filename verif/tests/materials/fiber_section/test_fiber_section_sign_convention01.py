# -*- coding: utf-8 -*-
# home made test
# Sign convention for fiber sections.

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from postprocess import prop_statistics
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor   

#Constant positive strain.
epsilon= 3.5e-3
epsilon1= epsilon
epsilon2= epsilon
epsilon3= epsilon
epsilon4= epsilon


#Read section definition from file.
import os
pth= os.path.dirname(__file__)
#print "pth= ", pth
if(not pth):
  pth= "."
execfile(pth+"/../../aux/four_fiber_section.py")

sigma= E*epsilon
F= sigma*fiberArea
N0Teor= 4*F
My0Teor= 0.0
Mz0Teor= 0.0
R0Teor=xc.Vector([N0Teor,My0Teor,Mz0Teor])
D0Teor=xc.Vector([epsilon,0.0,0.0])


ratioN0= abs(N0-N0Teor)/N0Teor
ratioN0S= abs(N0S-N0Teor)/N0Teor
ratioMy0= abs(My0-My0Teor)
ratioMy0S= abs(My0S-My0Teor)
ratioMz0= abs(Mz0-Mz0Teor)
ratioMz0S= abs(Mz0S-Mz0Teor)
ratioR0= (R0Teor-R0).Norm()
ratioD0= (D0Teor-D0).Norm()

fourFibersSection.revertToStart()
# Positive My (section)
epsilon1= -epsilon
epsilon2= -epsilon
epsilon3= epsilon
epsilon4= epsilon
f1.getMaterial().setTrialStrain(epsilon1,0.0)
f2.getMaterial().setTrialStrain(epsilon2,0.0)
f3.getMaterial().setTrialStrain(epsilon3,0.0)
f4.getMaterial().setTrialStrain(epsilon4,0.0)
N1= fourFibersSection.getFibers().getResultant()
My1= fourFibersSection.getFibers().getMy(0.0)
Mz1= fourFibersSection.getFibers().getMz(0.0)

fourFibersSection.setupFibers()
RR= fourFibersSection.getStressResultant()
R1= xc.Vector([RR[0],RR[2],RR[1]]) # N= RR[0], My= RR[2], Mz= RR[1]
deformationPlane1= fourFibersSection.getFibers().getDeformationPlane()
fourFibersSection.setTrialDeformationPlane(deformationPlane1)
DD= fourFibersSection.getSectionDeformation()
D1= xc.Vector([DD[0],DD[2],DD[1]]) # epsilon= DD[0], Ky= DD[2], Kz= DD[1]
N1S= fourFibersSection.getN()
My1S= fourFibersSection.getMy()
Mz1S= fourFibersSection.getMz()

N1Teor= 0.0
My1Teor= 2*F*widthOverZ
Mz1Teor= 0.0
R1Teor=xc.Vector([N1Teor,My1Teor,Mz1Teor])
Ky1Teor= 2*epsilon/widthOverZ
D1Teor=xc.Vector([0.0,Ky1Teor,0.0])

ratioN1= abs(N1-N1Teor)
ratioN1S= abs(N1S-N1Teor)
ratioMy1= abs(My1-My1Teor)/My1Teor
ratioMy1S= abs(My1S-My1Teor)/My1Teor
ratioMz1= abs(Mz1-Mz1Teor)
ratioMz1S= abs(Mz1S-Mz1Teor)
ratioR1= (R1Teor-R1).Norm()
ratioD1= (D1Teor-D1).Norm()

# Positive Mz (section)
fourFibersSection.revertToStart()
epsilon1= epsilon
epsilon2= -epsilon
epsilon3= -epsilon
epsilon4= epsilon
f1.getMaterial().setTrialStrain(epsilon1,0.0)
f2.getMaterial().setTrialStrain(epsilon2,0.0)
f3.getMaterial().setTrialStrain(epsilon3,0.0)
f4.getMaterial().setTrialStrain(epsilon4,0.0)
N2= fourFibersSection.getFibers().getResultant()
My2= fourFibersSection.getFibers().getMy(0.0)
Mz2= fourFibersSection.getFibers().getMz(0.0)
deformationPlane2= fourFibersSection.getFibers().getDeformationPlane()

fourFibersSection.setupFibers()
RR= fourFibersSection.getStressResultant()
R2= xc.Vector([RR[0],RR[2],RR[1]]) # N= RR[0], My= RR[2], Mz= RR[1]
fourFibersSection.setTrialDeformationPlane(deformationPlane2)
DD= fourFibersSection.getSectionDeformation()
D2= xc.Vector([DD[0],DD[2],DD[1]]) # epsilon= DD[0], Ky= DD[2], Kz= DD[1]
N2S= fourFibersSection.getN()
My2S= fourFibersSection.getMy()
Mz2S= fourFibersSection.getMz()

N2Teor= 0.0
My2Teor= 0.0
Mz2Teor= -4*F*depthOverY/2.0 #Mz positive is in the opposite direction with respecto to the positive y-axis. ???
R2Teor=xc.Vector([N2Teor,My2Teor,Mz2Teor])
Kz2Teor= 2*epsilon/depthOverY
D2Teor=xc.Vector([0.0,0.0,-Kz2Teor]) #Negative ???

ratioN2= abs(N2-N2Teor)
ratioN2S= abs(N2S-N2Teor)
ratioMy2= abs(My2-My2Teor)
ratioMy2S= abs(My2S-My2Teor)
ratioMz2= abs(Mz2-Mz2Teor)/Mz2Teor
ratioMz2S= abs(Mz2S-Mz2Teor)/Mz2Teor
ratioR2= (R2Teor-R2).Norm()
ratioD2= (D2Teor-D2).Norm()

# Positive Mz, negative My (section)
fourFibersSection.revertToStart()
epsilon= 3.5e-3
epsilon1= epsilon
epsilon2= 0.0
epsilon3= -epsilon
epsilon4= 0.0
f1.getMaterial().setTrialStrain(epsilon1,0.0)
f2.getMaterial().setTrialStrain(epsilon2,0.0)
f3.getMaterial().setTrialStrain(epsilon3,0.0)
f4.getMaterial().setTrialStrain(epsilon4,0.0)
N3= fourFibersSection.getFibers().getResultant()
My3= fourFibersSection.getFibers().getMy(0.0)
Mz3= fourFibersSection.getFibers().getMz(0.0)
deformationPlane3= fourFibersSection.getFibers().getDeformationPlane()

fourFibersSection.setupFibers()
RR= fourFibersSection.getStressResultant()
R3= xc.Vector([RR[0],RR[2],RR[1]]) # N= RR[0], My= RR[2], Mz= RR[1]
fourFibersSection.setTrialDeformationPlane(deformationPlane3)
DD= fourFibersSection.getSectionDeformation()
D3= xc.Vector([DD[0],DD[2],DD[1]]) # epsilon= DD[0], Ky= DD[2], Kz= DD[1]
N3S= fourFibersSection.getN()
My3S= fourFibersSection.getMy()
Mz3S= fourFibersSection.getMz()

N3Teor= 0.0
My3Teor= -2*F*widthOverZ/2.0
Mz3Teor= -2*F*depthOverY/2.0
R3Teor=xc.Vector([N3Teor,My3Teor,Mz3Teor])
Ky3Teor= -epsilon/widthOverZ
Kz3Teor= epsilon/depthOverY
D3Teor=xc.Vector([0.0,Ky3Teor,-Kz3Teor]) #Negative ???

ratioN3= abs(N3-N3Teor)
ratioN3S= abs(N3S-N3Teor)
ratioMy3= abs(My3-My3Teor)
ratioMy3S= abs(My3S-My3Teor)
ratioMz3= abs(Mz3-Mz3Teor)/Mz3Teor
ratioMz3S= abs(Mz3S-Mz3Teor)/Mz3Teor
ratioR3= (R3Teor-R3).Norm()
ratioD3= (D3Teor-D3).Norm()


import math
error= math.sqrt(ratioN0**2+ratioMy0**2+ratioMz0**2+ratioN0S**2+ratioMy0S**2+ratioMz0S**2+ratioR0**2+ratioD0**2+ratioN1**2+ratioMy1**2+ratioMz1**2+ratioN1S**2+ratioMy1S**2+ratioMz1S**2+ratioR1**2+ratioD1**2+ratioN2**2+ratioMy2**2+ratioMz2**2+ratioN2S**2+ratioMy2S**2+ratioMz2S**2+ratioR2**2+ratioD2**2+ratioN3**2+ratioMy3**2+ratioMz3**2+ratioN3S**2+ratioMy3S**2+ratioMz3S**2+ratioR3**2+ratioD3**2)

print 'N0= ', N0, ' N0S= ', N0S, ' N0Teor= ', N0Teor, ' ratioN0= ', ratioN0, ' ratioN0S= ', ratioN0S
print 'My0= ', My0, ' My0S= ', My0S, ' My0Teor= ', My0Teor, ' ratioMy0= ', ratioMy0, ' ratioMy0S= ', ratioMy0S
print 'Mz0= ', Mz0, ' Mz0S= ', Mz0S, ' Mz0Teor= ', Mz0Teor, ' ratioMz0= ', ratioMz0, ' ratioMz0S= ', ratioMz0S
print 'R0= ', R0, ' R0Teor= ', R0Teor, ' ratioR0= ', ratioR0
print 'D0= ', D0, ' D0Teor= ', D0Teor, ' ratioD0= ', ratioD0
print 'N1= ', N1, ' N1S= ', N1S, ' N1Teor= ', N1Teor, ' ratioN1= ', ratioN1, ' ratioN1S= ', ratioN1S
print 'My1= ', My1, ' My1S= ', My1S, ' My1Teor= ', My1Teor, ' ratioMy1= ', ratioMy1, ' ratioMy1S= ', ratioMy1S
print 'Mz1= ', Mz1, ' Mz1S= ', Mz1S, ' Mz1Teor= ', Mz1Teor, ' ratioMz1= ', ratioMz1, ' ratioMz1S= ', ratioMz1S
print 'R1= ', R1, ' R1Teor= ', R1Teor, ' ratioR1= ', ratioR1
print 'D1= ', D1, ' D1Teor= ', D1Teor, ' ratioD1= ', ratioD1
print 'N2= ', N2, ' N2S= ', N2S, ' N2Teor= ', N2Teor, ' ratioN2= ', ratioN2, ' ratioN2S= ', ratioN2S
print 'My2= ', My2, ' My2S= ', My2S, ' My2Teor= ', My2Teor, ' ratioMy2= ', ratioMy2, ' ratioMy2S= ', ratioMy2S
print 'Mz2= ', Mz2, ' Mz2S= ', Mz2S, ' Mz2Teor= ', Mz2Teor, ' ratioMz2= ', ratioMz2, ' ratioMz2S= ', ratioMz2S
print 'R2= ', R2, ' R2Teor= ', R2Teor, ' ratioR2= ', ratioR2
print 'D2= ', D2, ' D2Teor= ', D2Teor, ' ratioD2= ', ratioD2
print 'N3= ', N3, ' N3S= ', N3S, ' N3Teor= ', N3Teor, ' ratioN3= ', ratioN3, ' ratioN3S= ', ratioN3S
print 'My3= ', My3, ' My3S= ', My3S, ' My3Teor= ', My3Teor, ' ratioMy3= ', ratioMy3, ' ratioMy3S= ', ratioMy3S
print 'Mz3= ', Mz3, ' Mz3S= ', Mz3S, ' Mz3Teor= ', Mz3Teor, ' ratioMz3= ', ratioMz3, ' ratioMz3S= ', ratioMz3S
print 'R3= ', R3, ' R3Teor= ', R3Teor, ' ratioR3= ', ratioR3
print 'D3= ', D3, ' D3Teor= ', D3Teor, ' ratioD3= ', ratioD3
print 'error= ', error

from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (error < 1e-3):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

