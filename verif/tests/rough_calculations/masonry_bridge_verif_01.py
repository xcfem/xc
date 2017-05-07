#-*- coding: utf-8 -*-
from __future__ import division

# Ultimate load of a masonry bridge according to thesis of Alix Grandjean: "Capacité portante de ponts en arc en maçonnerie de pierre naturelle". École Polytechnique Féderale de Lausanne.

#See Annexe 14 page 235

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import sys
import rough_calculations.masonryVault

#Coefficients of polynomial
f= -1.47e-3 #-1.47e-12
j= 4.4e-2 #4.4e-8
k= -5.46e-1 #-5.46e-4
r= 3.238

#x coordinates of the hinges.
xA= 3.156
xC= 6.175
xD= 10.996
xB= 14.001

d= 1 # arch thickness.
v= 4 # arch width
L= 15 #arch span

hR= 9 #??
hS= 0.5 #??

#Material parameters.
alpha= 0.726 #??
beta= 6.095 #??

#Self weight
PPR= 18e3 #N/m3
PPS= 20e3 #N/m3

#Geometric parameters.
a= xC-xA
b= xB-xD
# Warning!. We change the sign of the
# angle to make it equal to that in
# the example (Thesis. Annexe 14. page 235).
gammaD= -rough_calculations.masonryVault.calcGamma(f,j,k,r,xD)
hA= rough_calculations.masonryVault.yAxis(f,j,k,r,xA)
hC= rough_calculations.masonryVault.yAxis(f,j,k,r,xC)
hD= rough_calculations.masonryVault.yAxis(f,j,k,r,xD)
hB= rough_calculations.masonryVault.yAxis(f,j,k,r,xB)
LR= xB-xA
hHalfL= rough_calculations.masonryVault.yAxis(f,j,k,r,L/2)

#Filling characteristics
angPhi= math.radians(30)
cohesion= 0
mp= 0.33 #Correction factors.
mc= 0.01

#Modèle de charge du traffic routier.
delta= math.radians(30)
hRcle= 1.5 # Hauteur du remplissage sur la clé de la voûte (m).
Q= 160000 # Charge ponctuelle due au trafic (N).
qrep= 0.005e6 # Charge uniformément repartie due au trafic (Pa).



# Loads
eta= v*(PPS*hS*LR+PPR*(hR*LR+rough_calculations.masonryVault.aux1(f,j,k,r,xB,xA)))
phi= v*(PPS*hS*a+PPR*(hR*a+rough_calculations.masonryVault.aux1(f,j,k,r,xC,xA)))
psi= v*(PPS*hS*b+PPR*(hR*b+rough_calculations.masonryVault.aux1(f,j,k,r,xB,xD)))
etaW= v*(PPS*hS*pow(LR,2)/2+PPR*(hR*pow(LR,2)/2-f*pow(xB,6)/30-j*pow(xB,5)/20-k*pow(xB,4)/12-r*pow(xB,3)/6-f*pow(xA,6)/6-j*pow(xA,5)/5+f*pow(xA,5)*xB/5-k*pow(xA,4)/4+j*pow(xA,4)*xB/4-r*pow(xA,3)/3+k*pow(xA,3)*xB/3+r*pow(xA,2)*xB/2))
phiS= v*(PPS*hS*pow(a,2)/2+PPR*(hR*pow(a,2)/2-f*pow(xC,6)/30-j*pow(xC,5)/20-k*pow(xC,4)/12-r*pow(xC,3)/6-f*pow(xA,6)/6-j*pow(xA,5)/5+f*pow(xA,5)*xC/5-k*pow(xA,4)/4+j*pow(xA,4)*xC/4-r*pow(xA,3)/3+k*pow(xA,3)*xC/3+r*pow(xA,2)*xC/2))
psiT= v*(PPS*hS*pow(b,2)/2+PPR*(hR*pow(b,2)/2-f*pow(xD,6)/30-j*pow(xD,5)/20-k*pow(xD,4)/12-r*pow(xD,3)/6-f*pow(xB,6)/6-j*pow(xB,5)/5+f*pow(xB,5)*xD/5-k*pow(xB,4)/4+j*pow(xB,4)*xD/4-r*pow(xB,3)/3+k*pow(xB,3)*xD/3+r*pow(xB,2)*xD/2))

#Earth pressure of filling.
Kp= pow(math.tan(math.pi/4+angPhi/2),2)
Kc= 2*math.sqrt(Kp)

R= v*(Kp*mp*(hS*PPS*(hD-hB)+PPR*hR*(hD-hB)-PPR*(hD*hD/2-hB*hB/2))+cohesion*mc*Kc*(hD-hB))

RzB= v*(Kp*mp*(hS*PPS*(hD*hD/2-hB*hB/2-hB*(hD-hB)))+PPR*(hR*(hD*hD/2-hB*hB/2)-hR*hB*(hD-hB)-pow(hD,3)/3+pow(hB,3)/3+hB*(hD*hD/2-hB*hB/2))+cohesion*mc*Kc*(hD*hD/2-hB*hB/2-hB*(hD-hB)))

RzD= v*(Kp*mp*(hS*PPS*(hD*(hD-hB)-hD*hD/2+hB*hB/2))+PPR*(hR*hD*(hD-hB)-hR*(hD*hD/2-hB*hB/2)-hD*(hD*hD/2-hB*hB/2)+pow(hD,3)/3-pow(hB,3)/3)+cohesion*mc*Kc*(hD*(hD-hB)-hD*hD/2+hB*hB/2))

vQt= rough_calculations.masonryVault.vQtrans(v,delta,hRcle)
qtrans= Q/vQt
lQt= rough_calculations.masonryVault.lQtrans(a,delta,hRcle)
X= qtrans/lQt

# Résistance
Nadmis= -1.25e6 #Effort axial admisible
Madmis= rough_calculations.masonryVault.diagInteraction(Nadmis,d,v,alpha,beta)
E= rough_calculations.masonryVault.calcE6p27(X,qrep,L,LR,v,lQt,a,b,hA,hB,hC,hD,xA)
F= rough_calculations.masonryVault.calcF6p28(R,LR,a,b,eta,phiS,etaW,psiT,Madmis,Madmis,Madmis,RzB,RzD,hA,hB,hC,hD)
G= rough_calculations.masonryVault.calcG6p29(X,qrep,L,LR,v,lQt,a,b,hA,hB,hC,hD,xA,gammaD)
H= rough_calculations.masonryVault.calcH6p30(LR,a,eta,psi,phiS,etaW,Madmis,Madmis,Madmis,RzB,hA,hB,hC,hD,gammaD)
n= rough_calculations.masonryVault.calcn6p32(alpha,beta,d,v,E,F,G,H)

#rough_calculations.masonryVault.printVouteResults(L, f, j, k, r, a,b,gammaD,hA,hB,hC,hD,LR,d,v,hR,hS,hHalfL,alpha,beta,PPR,PPS,eta,etaW,phi,phiS,psi,psiT,mp,Kp,mc,Kc,R, RzB, RzD,qtrans,X,Nadmis,Madmis,E,F,G,H,n)

nTeor= 8.47812250571
ratio1= (n-nTeor)/nTeor
import os
from miscUtils import LogMessages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5:
  print "test ",fname,": ok."
else:
  lmsg.error(fname+' ERROR.')
