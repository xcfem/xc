# -*- coding: utf-8 -*-
'''Shrinkage verification test.'''

#Data for comparison from the JRC technical report: 
#"Bridge design to Eurocodes. Worked examples"
#Sect. 4.5.3
from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AOO) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AOO and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec2 import EC2_materials


#Data
#Type of concrete used in the deck slab
concrDeck=EC2_materials.EC2Concrete2021("C35/45",-35e6,1.5)
concrDeck.cemType='N'   #class N cement
RH=80                   #ambient relative humidity(%)

vCalc=[]
vComp=[]
#Shrinkage deformation at traffic opening
t=77     #age of the concrete at traffic opening
ts=1     #drying shrinkage begins at the age 1 day
Ac=3.9     #area of the concrete slab (m2)
u=11.6     #perimeter exposed to drying (m)
h0=2*Ac/u  #notional size of the member h0.
#   autogenous shrinkage
Epscbsfcm= concrDeck.getShrEpscbsfcm() # coefficient for calculating the basic shrinkage strain
vCalc.append(Epscbsfcm)
vComp.append(-7.882720305333364e-05)
Betaast=concrDeck.getShrBetaast(t)    # coefficient for calculating the autogenous shrinkage strain
vCalc.append(Betaast)
vComp.append(0.827091526168334)
Epscbs=concrDeck.getShrEpscbs(t)        # Basic shrinkage strain
vCalc.append(Epscbs)
vComp.append(-6.519731167696288e-05)
#   drying shrinkage
Epscdsfcm= concrDeck.getShrEpscdsfcm()
vCalc.append(Epscdsfcm)
vComp.append(-0.00039395623916507625)
RHeq= concrDeck.getShrRHeq()
vCalc.append(RHeq)
vComp.append(96.98289713039807) # internal relative humidity of concrete

BetaRH=concrDeck.getShrBetaRH(RH)   # Coefficient for the calculation of the basic drying shrinkage strain
vCalc.append(BetaRH)
vComp.append(-0.6800061176934002)
Betadstts= concrDeck.getShrBetadstts(t= t, ts= ts, h0= h0) # time development of drying shrinkage.
vCalc.append(Betadstts)
vComp.append(0.06913465424624372)
Epscds=concrDeck.getShrEpscds(t= t, ts= ts, h0= h0, RH= RH)   # drying shrinkage strain
vCalc.append(Epscds)
vComp.append(-1.8520665921994158e-05)

Epscs=concrDeck.getShrEpscs(t= t,ts= ts,h0= h0 , RH= RH)   #Total shrinkage 

vCalc.append(Epscs)
vComp.append(-8.371797759895704e-05)

'''
print('Betadstts: ', Betadstts)
print('Epscdsfcm: ', Epscdsfcm*1e6, 'e-6')
print('RHeq: ', RHeq)
print('BetaRH: ', BetaRH)
print('lower characteristic total shrinkage: ', 0.5*Epscs*1e6, 'e-6')
print('basic shrinkage: ', Epscbs*1e6, 'e-6')
print('drying shrinkage : ', Epscds*1e6, 'e-6')
print('total shrinkage: ', Epscs*1e6, 'e-6')
print('vCalc= ', vCalc)
print('vComp= ', vComp)
'''

sqrErr= 0.0
for i in range(0,len(vCalc)):
  sqrErr+= (vCalc[i]-vComp[i])**2
import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)+'-Epscu3'
if sqrErr<1e-8:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
