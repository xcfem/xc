# -*- coding: utf-8 -*-
#Creep verification test. 
#Data for comparison from the JRC technical report: 
#"Bridge design to Eurocodes. Worked examples"
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

Ac=3.9     #area of the concrete slab (m2)
u=11.6     #perimeter exposed to drying (m)
h0mm=2*Ac/u*1000    #notional size of the member h0 (mm)
RH=80               #ambient relative humidity(%)
t=1e6               #long term
#age of concrete in days at loading
t0concreting=22         #mean value t0 of all the slab concreting phases
t0shrinkage=1           #shrinkage is assumed to begin at the age of 1 day
t0equipments=88         #age of concrete when non-structural bridge equipments are loaded
t0settlement=50         #a settlement is assumed to occur at t0=50 days

t0=t0settlement                 #age of concrete in days at loading
alfa1=concrDeck.getCreepAlfa1() #Coefficient for the calculation of the creep coefficient
ratio1=round(alfa1,2)-0.87
alfa2=concrDeck.getCreepAlfa2() #Coefficient for the calculation of the creep coefficient
ratio2=round(alfa2,2)-0.96
alfa3=concrDeck.getCreepAlfa3() #Coefficient for the calculation of the creep coefficient
ratio3=round(alfa3,2)-0.90
fiRH=concrDeck.getCreepFiRH(RH,h0mm) #factor to allow for the effect of relative humidity
                                     #on the notional creep coefficient
ratio4=round(fiRH,2)-1.15
betafcm=concrDeck.getCreepBetafcm()  #factor to allow for the effect of concrete strength
                                     #on the notional creep coefficient
ratio5=round(betafcm,2)-2.56
betat0=concrDeck.getCreepBetat0(t0)  #factor to allow for the effect of concrete age at loading
                                     #on the notional creep coefficient
ratio6=round(betat0,2)-0.44
fi0=concrDeck.getCreepFi0(t0,RH,h0mm) #notational creep coefficient for the calculation 
                                      #of the creep coefficient
ratio7=round(fi0,2)-1.29
betactt0=concrDeck.getCreepBetactt0(t,t0,RH,h0mm) #coefficient to describe the development of 
                                                  #creep with time after loading
ratio8=round(betactt0,2)-1.00

#Creep coefficients:
fitt0Concreting=concrDeck.getCreepFitt0(t,t0concreting,RH,h0mm)
ratio9=round(fitt0Concreting,2)-1.51
fitt0Shrinkage=concrDeck.getCreepFitt0(t,t0shrinkage,RH,h0mm)
ratio10=round(fitt0Shrinkage,2)-2.68
fitt0Equipments=concrDeck.getCreepFitt0(t,t0equipments,RH,h0mm)
ratio11=round(fitt0Equipments,2)-1.15
fitt0Settlement=concrDeck.getCreepFitt0(t,t0settlement,RH,h0mm)
ratio12=round(fitt0Settlement,2)-1.29

comp=[ratio1,ratio2,ratio3,ratio4,ratio5,ratio6,ratio7,ratio8,ratio9,ratio10,ratio11,ratio12]

sqrErr= 0.0
for i in range(0,len(comp)):
  sqrErr+= comp[i]**2
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)+'-fcm'
if sqrErr<1e-6:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

