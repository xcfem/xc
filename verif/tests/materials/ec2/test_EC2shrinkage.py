# -*- coding: utf-8 -*-
#Shrinkage verification test. 
#Data for comparison from the JRC technical report: 
#"Bridge design to Eurocodes. Worked examples"
#Sect. 4.5.3
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_concrete


#Data
#Type of concrete used in the deck slab
concrDeck=EC2_concrete.EC2Concrete("C35/45",-35e6,1.5)
concrDeck.cemType='N'   #class N cement
RH=80                   #ambient relative humidity(%)

vCalc=[]
vComp=[]
#Shrinkage deformation at traffic openning
t=77     #age of the concrete at traffic openning
ts=1     #drying shrinkage begins at the age 1 day
Ac=3.9     #area of the concrete slab (m2)
u=11.6     #perimeter exposed to drying (m)
h0mm=2*Ac/u*1000    #notional size of the member h0 (mm)
#   autogenous shrinkage
Epscainf=concrDeck.getShrEpscainf(t)  #coefficient for calculating the autogenous shrinkage strain
vCalc.append(round(Epscainf*(-1),7))
vComp.append(6.25e-05)
Betaast=concrDeck.getShrBetaast(t)    #coefficient for calculating the autogenous shrinkage strain
vCalc.append(round(Betaast,3))
vComp.append(0.827)
Epsca=concrDeck.getShrEpsca(t)        #Autogenous shrinkage strain
vCalc.append(round(Epsca*(-1),7))
vComp.append(5.17e-05)
#   drying shrinkage
BetaRH=concrDeck.getShrBetaRH(RH)   #Coefficient for the calculation of the basic drying shrinkage strain
vCalc.append(round(BetaRH,3))
vComp.append(0.756)
Alfads1=concrDeck.getShrAlfads1()   #Coefficient for the calculation of the basic drying shrinkage strain
vCalc.append(Alfads1)
vComp.append(4)
Alfads2=concrDeck.getShrAlfads2()   #Coefficient for the calculation of the basic drying shrinkage strain
vCalc.append(Alfads2)
vComp.append(0.12)
Epscd0=concrDeck.getShrEpscd0(RH)   #Basic drying shrinkage strain
vCalc.append(round(Epscd0*(-1),6))
vComp.append(2.53e-4)
Kh=concrDeck.getShrKh(h0mm)         #coefficient  for the calculation of the drying shrinkage strain
vCalc.append(round(Kh,2))
vComp.append(0.7)
Betadstts=concrDeck.getShrBetadstts(t,ts,h0mm)   #coefficient  for the calculation of the drying shrinkage strain

vCalc.append(round(Betadstts,1))
vComp.append(0.1)
Epscd=concrDeck.getShrEpscd(t,ts,RH,h0mm)   #Drying shrinkage strain

vCalc.append(round(Epscd*(-1),7))
vComp.append(1.8e-5)
Epscs=concrDeck.getShrEpscs(t,ts,RH,h0mm)   #Total shrinkage 

vCalc.append(round(Epscs*(-1),5))
vComp.append(7.0e-5)

'''
print vCalc
print vComp
'''
sqrErr= 0.0
for i in range(0,len(vCalc)):
  sqrErr+= (vCalc[i]-vComp[i])**2
import os
fname= os.path.basename(__file__)+'-Epscu3'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
