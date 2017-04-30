# -*- coding: utf-8 -*-

from __future__ import division
from materials.ehe import EHE_concrete

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

fckDat=[20,25,30,35,40,45,50,55,60,70,80,90,100]
fcmCalc=[]
fctmCalc=[]
fctk005Calc=[]
fctk095Calc=[]
EcmCalc=[]
Epsc2Calc=[]
Epscu2Calc=[]
ExpNCalc=[]

for i in range(len(fckDat)):
    name='H'+str(fckDat[i])
    fck=-1*fckDat[i]*1e6 #[Pa][-]
    concr=EHE_concrete.EHEConcrete(name,fck,1.5)
    fcm=concr.getFcm()/(-1e6)
    fcmCalc.append(fcm)
    fctm=round(concr.getFctm()/1e6,1)
    fctmCalc.append(fctm)
    fctk005=round(concr.getFctk005()/1e6,1)
    fctk005Calc.append(fctk005)
    fctk095=round(concr.getFctk095()/1e6,1)
    fctk095Calc.append(fctk095)
    concr.typeAggregate='Q'
    Ecm=round(concr.getEcm()/1e9,0)
    EcmCalc.append(Ecm)
    Epsc2=round(concr.getEpsc2()*(-1e3),1)
    Epsc2Calc.append(Epsc2)
    Epscu2=round(concr.getEpscu2()*(-1e3),1)
    Epscu2Calc.append(Epscu2)
    ExpN=round(concr.getExpN(),1)
    ExpNCalc.append(ExpN)


#Test Fcm
fcmDat=[28,33,38,43,48,53,58,63,68,78,88,98,108]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fcmDat)):
  sqrErr+= (fcmDat[i]-fcmCalc[i])**2
import os
fname= os.path.basename(__file__)+'-fcm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

#Test Fctm
fctmDat=[2.2,2.6,2.9,3.2,3.5,3.8,4.1,4.3, 4.5, 4.9, 5.2, 5.5, 5.8]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctmDat)):
  sqrErr+= (fctmDat[i]-fctmCalc[i])**2
import os
fname= os.path.basename(__file__)+'-fctm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


#Test Fctk005
fctk005Dat=[1.5,1.8,2.0,2.2,2.5,2.7,2.9,3.0,3.1, 3.4, 3.6, 3.9, 4.1]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctk005Dat)):
  sqrErr+= (fctk005Dat[i]-fctk005Calc[i])**2
import os
fname= os.path.basename(__file__)+'-fctk005'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
#Test Fctk095
fctk095Dat=[2.9,3.3,3.8,4.2,4.6,4.9,5.3, 5.6, 5.8, 6.3, 6.7, 7.2, 7.5]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctk095Dat)):
  sqrErr+= (fctk095Dat[i]-fctk095Calc[i])**2
import os
fname= os.path.basename(__file__)+'-fctk095'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

#Test Ecm
EcmDat=[26.0, 27.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 36.0, 38.0, 39.0, 40.0] #[GPa]
sqrErr= 0.0
for i in range(0,len(EcmDat)):
  sqrErr+= (EcmDat[i]-EcmCalc[i])**2
import os
fname= os.path.basename(__file__)+'-Ecm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."


#Test Epsc2
Epsc2Dat=[2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.2,2.3,2.4,2.5,2.5,2.6] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epsc2Dat)):
  sqrErr+= (Epsc2Dat[i]-Epsc2Calc[i])**2
import os
fname= os.path.basename(__file__)+'-Epsc2'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

#Test Epscu2
Epscu2Dat=[3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.2, 3.0, 2.7,2.6,2.6,2.6] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epscu2Dat)):
  sqrErr+= (Epscu2Dat[i]-Epscu2Calc[i])**2
import os
fname= os.path.basename(__file__)+'-Epscu2'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

#Test ExpN
ExpNDat=[2.0,2.0,2.0,2.0,2.0,2.0,2.0,1.8,1.6,1.5,1.4,1.4,1.4]
sqrErr= 0.0
for i in range(0,len(ExpNDat)):
  sqrErr+= (ExpNDat[i]-ExpNCalc[i])**2
import os
fname= os.path.basename(__file__)+'-ExpN'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

