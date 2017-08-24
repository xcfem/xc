# -*- coding: utf-8 -*-

from __future__ import division
from materials.ec2 import EC2_materials
import os
from miscUtils import LogMessages as lmsg

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2015, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

fckDat=[12,16,20,25,30,35,40,45,50,55,60,70,80,90]
fcmCalc=[]
fctmCalc=[]
fctk005Calc=[]
fctk095Calc=[]
EcmCalc=[]
Epsc1Calc=[]
Epscu1Calc=[]
Epsc2Calc=[]
Epscu2Calc=[]
ExpNCalc=[]
Epsc3Calc=[]
Epscu3Calc=[]

for i in range(len(fckDat)):
    name='C'+str(fckDat[i])
    fck=-1*fckDat[i]*1e6 #[Pa][-]
    concr= EC2_materials.EC2Concrete(name,fck,1.5)
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
    Epsc1=round(concr.getEpsc1()*(-1e3),1)
    Epsc1Calc.append(Epsc1)
    Epscu1=round(concr.getEpscu1()*(-1e3),1)
    Epscu1Calc.append(Epscu1)
    Epsc2=round(concr.getEpsc2()*(-1e3),1)
    Epsc2Calc.append(Epsc2)
    Epscu2=round(concr.getEpscu2()*(-1e3),1)
    Epscu2Calc.append(Epscu2)
    ExpN=round(concr.getExpN(),1)
    ExpNCalc.append(ExpN)
    if concr.fckMPa()<=50:
        Epsc3=round(concr.getEpsc3()*(-1e3),2)
    else:
        Epsc3=round(concr.getEpsc3()*(-1e3),1)
    Epsc3Calc.append(Epsc3)
    Epscu3=round(concr.getEpscu3()*(-1e3),1)
    Epscu3Calc.append(Epscu3)
#Test Fcm
fcmDat=[20,24,28,33,38,43,48,53,58,63,68,78,88,98]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fcmDat)):
  sqrErr+= (fcmDat[i]-fcmCalc[i])**2
fname= os.path.basename(__file__)+'-fcm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


#Test Fctm
fctmDat=[1.6,1.9,2.2,2.6,2.9,3.2,3.5,3.8,4.1,4.2,4.4,4.6,4.8,5.0]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctmDat)):
  sqrErr+= (fctmDat[i]-fctmCalc[i])**2
fname= os.path.basename(__file__)+'-fctm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')


#Test Fctk005
fctk005Dat=[1.1,1.3,1.5,1.8,2.0,2.2,2.5,2.7,2.9,3.0,3.0,3.2,3.4,3.5]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctk005Dat)):
  sqrErr+= (fctk005Dat[i]-fctk005Calc[i])**2
fname= os.path.basename(__file__)+'-fctk005'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
#Test Fctk095
fctk095Dat=[2.0,2.5,2.9,3.3,3.8,4.2,4.6,4.9,5.3,5.5,5.7,6.0,6.3,6.6]  #[MPa]
sqrErr= 0.0
for i in range(0,len(fctk095Dat)):
  sqrErr+= (fctk095Dat[i]-fctk095Calc[i])**2
fname= os.path.basename(__file__)+'-fctk095'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Ecm
EcmDat=[27,29,30,31,33,34,35,36,37,38,39,41,42,44]  #[GPa]
sqrErr= 0.0
for i in range(0,len(EcmDat)):
  sqrErr+= (EcmDat[i]-EcmCalc[i])**2
fname= os.path.basename(__file__)+'-Ecm'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epsc1
Epsc1Dat=[1.8,1.9,2.0,2.1,2.2,2.2,2.3,2.4,2.5,2.5,2.6,2.7,2.8,2.8]  #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epsc1Dat)):
  sqrErr+= (Epsc1Dat[i]-Epsc1Calc[i])**2
fname= os.path.basename(__file__)+'-Epsc1'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epscu1
Epscu1Dat=[3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.2,3.0,2.8,2.8,2.8] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epscu1Dat)):
  sqrErr+= (Epscu1Dat[i]-Epscu1Calc[i])**2
fname= os.path.basename(__file__)+'-Epscu1'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epsc2
Epsc2Dat=[2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.2,2.3,2.4,2.5,2.6] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epsc2Dat)):
  sqrErr+= (Epsc2Dat[i]-Epsc2Calc[i])**2
fname= os.path.basename(__file__)+'-Epsc2'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epscu2
Epscu2Dat=[3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.1,2.9,2.7,2.6,2.6] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epscu2Dat)):
  sqrErr+= (Epscu2Dat[i]-Epscu2Calc[i])**2
fname= os.path.basename(__file__)+'-Epscu2'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test ExpN
ExpNDat=[2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,2.0,1.8,1.6,1.4,1.4,1.4]
sqrErr= 0.0
for i in range(0,len(ExpNDat)):
  sqrErr+= (ExpNDat[i]-ExpNCalc[i])**2
fname= os.path.basename(__file__)+'-ExpN'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epsc3
Epsc3Dat=[1.75,1.75,1.75,1.75,1.75,1.75,1.75,1.75,1.75,1.8,1.9,2.0,2.2,2.3] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epsc3Dat)):
  sqrErr+= (Epsc3Dat[i]-Epsc3Calc[i])**2
fname= os.path.basename(__file__)+'-Epsc3'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

#Test Epscu3
Epscu3Dat=[3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.5,3.1,2.9,2.7,2.6,2.6] #[per thousand]
sqrErr= 0.0
for i in range(0,len(Epscu3Dat)):
  sqrErr+= (Epscu3Dat[i]-Epscu3Calc[i])**2
fname= os.path.basename(__file__)+'-Epscu3'
if sqrErr<1e-8:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
