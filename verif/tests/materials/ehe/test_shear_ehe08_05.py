# -*- coding: utf-8 -*-

''' Computation of Vsu value. The results are compared with those obtained
from the exercise ER-CA-02 from www.areadecalculo.com. According to EHE-08.
'''

from __future__ import division
import sys

from materials.ehe import EHE_limit_state_checking
import math

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

Nd= 1224e3 #Design axial internal force.
Md= 0 #Bending moment design value
Vd= 125e3 #Effective design shear (clause 42.2.2).
Td= 0 #Torque design value.
d= 0.55 #Effective depth.
z= 0.9*d #Lever arm.
Asl= 9.425e-4
AsAct= 0 #reinforcement area activa
AsTrsv= math.pi*(6e-3/2)**2*4/0.2 #reinforcement area transversal
Es= 200e9 #Elastic modulus of the steel de la reinforcement pasiva.
Ep= Es #Elastic modulus of the steel de la reinforcement activa.
Fp= 0 #Prestressing force on the cross-section (positive if in tension).
Ae= 0.01 #Area enclosed by the mid-line of the effective hollow area.
ue= 1 #Perimeter of the mid-line of the effective hollow area.
epsilonX= EHE_limit_state_checking.getEpsilonXEHE08(Nd,Md,Vd,Td,z,Asl,AsAct,Es,Ep,Fp,Ae,ue)
gammaC= 1.5
gammaS= 1.15
fck= 30e6
fcd= fck/gammaC
fyk= 500e6
fyd= fyk/gammaS
Ac= 0.4*0.6e-4
vChi= min(2,1+math.sqrt(200/(d*1000)))
Fcv= EHE_limit_state_checking.getFcvEHE08(0.15,fck,gammaC,0.4,d,vChi,0.0,Asl,0.0)
thetaE= EHE_limit_state_checking.getCrackAngleEHE08(Nd,Md,Vd,Td,z,Asl,AsAct,Es,Ep,Fp,Ae,ue)
theta= math.radians(45)
alpha= math.radians(90)
beta= EHE_limit_state_checking.getBetaVcuEHE08(theta,thetaE)
Vcu= EHE_limit_state_checking.getVcuEHE08(fck,fcd,gammaC,0,Ac,0.4,d,z,Asl,0.0,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue)
Vu1= EHE_limit_state_checking.getVu1EHE08(fck,fcd,0,Ac,0.4,d,alpha,theta)
Vsu= EHE_limit_state_checking.getVsuEHE08(z,alpha,theta,AsTrsv,fyd)
ratio1= abs(Fcv-0.375e6)/0.375e6
ratio2= abs(Vcu-82.607e3)/82.607e3
ratio3= abs(Vu1-1320e3)/1320e3
ratio4= abs(Vsu-111.966e3)/111.966e3


##print "Fcv= ",Fcv/1e6," MPa"
##print "thetaE= ",math.degrees(thetaE)," grados"
##print "beta= ",beta
##print "Vcu= ",Vcu/1e3," kN"
##print "Vsu= ",Vsu/1e3," kN"
##print "Vu1= ",Vu1/1e3," kN"
##print "ratio1= ",ratio1
##print "ratio2= ",ratio2
##print "ratio3= ",ratio3
##print "ratio4= ",ratio4
  

import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if (ratio1<0.002) and (ratio2<0.0002) and (ratio3<1e-15) and (ratio4<1e-5):
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')

  
