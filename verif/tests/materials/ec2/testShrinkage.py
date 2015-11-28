# -*- coding: utf-8 -*-

#*    Nov. 2015   Ana Ortega    *

#Shrinkage verification test. 
#Data for comparison from the JRC technical report: 
#"Bridge design to Eurocodes. Worked examples"
#Sect. 4.5.3
from __future__ import division
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
vCalc.append(round(Epscainf*(-1e3),7))
vComp.append(6.25e-05)
Betaast=concrDeck.getShrBetaast(t)    #coefficient for calculating the autogenous shrinkage strain
vCalc.append(round(Betaast,3))
vComp.append(0.827)
Epsca=concrDeck.getShrEpsca(t)        #Autogenous shrinkage strain
vCalc.append(round(Epsca*(-1e3),7))
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
vCalc.append(round(Epscd0*(-1e3),6))
vComp.append(2.53e-4)
Kh=concrDeck.getShrKh(h0mm)         #coefficient  for the calculation of the drying shrinkage strain
vCalc.append(round(Kh,2))
vComp.append(0.7)
Betadstts=concrDeck.getShrBetadstts(t,ts,h0mm)   #coefficient  for the calculation of the drying shrinkage strain
                                                 #(note: erroneus result in technical report)
vCalc.append(round(Betadstts,2))
vComp.append(1.00)
Epscd=concrDeck.getShrEpscd(t,ts,RH,h0mm)   #Drying shrinkage strain
                                            #(note: erroneus result in technical report)
vCalc.append(round(Epscd*(-1e3),6))
vComp.append(1.76e-4)
Epscs=concrDeck.getShrEpscs(t,ts,RH,h0mm)   #Total shrinkage 
                                            #(note: erroneus result in technical report)
vCalc.append(round(Epscs*(-1e3),6))
vComp.append(2.28e-4)

sqrErr= 0.0
for i in range(0,len(vCalc)):
  sqrErr+= (vCalc[i]-vComp[i])**2
import os
fname= os.path.basename(__file__)+'-Epscu3'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
