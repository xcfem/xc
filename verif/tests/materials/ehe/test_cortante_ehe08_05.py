# -*- coding: utf-8 -*-

#Test de funcionamiento del cálculo de Vsu.
#El resultado se compara con el obtenidos del ejercicio ER-CA-02 de
#www.areadecalculo.com. Según la norma EHE-08».

from __future__ import division
import sys

from materials.ehe import comprobVEHE08
import math

Nd=1224e3 #Axil de cálculo.
Md=0 #Momento de cálculo.
Vd=125e3 #Cortante efectivo de cálculo (artículo 42.2.2).
Td=0 #Torsor de cálculo.
d=0.55 #Canto útil.
z=0.9*d #Brazo mecánico.
Asl=9.425e-4
AsAct=0 #Área de la armadura activa
AsTrsv=math.pi*(6e-3/2)**2*4/0.2 #Área de la armadura transversal
Es=200e9 #Módulo elástico del acero de la armadura pasiva.
Ep=Es #Módulo elástico del acero de la armadura activa.
Fp=0 #Fuerza producida por el pretensado en la sección (positiva si es de tracción).
Ae=0.01 #Área encerrada por la línea media de la sección hueca eficaz.
ue=1 #Perímetro de la línea media de la sección hueca eficaz.
epsilonX=comprobVEHE08.getEpsilonXEHE08(Nd,Md,Vd,Td,z,Asl,AsAct,Es,Ep,Fp,Ae,ue)
gammaC= 1.5
gammaS= 1.15
fck=30e6
fcd=fck/gammaC
fyk=500e6
fyd=fyk/gammaS
Ac=0.4*0.6e-4
vChi=min(2,1+math.sqrt(200/(d*1000)))
Fcv=comprobVEHE08.getFcvEHE08(0.15,fck,gammaC,0.4,d,vChi,0.0,Asl,0.0)
thetaE=comprobVEHE08.getAnguloInclinacionFisurasEHE08(Nd,Md,Vd,Td,z,Asl,AsAct,Es,Ep,Fp,Ae,ue)
theta=math.radians(45)
alpha=math.radians(90)
beta=comprobVEHE08.getBetaVcuEHE08(theta,thetaE)
Vcu=comprobVEHE08.getVcuEHE08(fck,fcd,gammaC,0,Ac,0.4,d,z,Asl,0.0,theta,Nd,Md,Vd,Td,Es,Ep,Fp,Ae,ue)
Vu1=comprobVEHE08.getVu1EHE08(fck,fcd,0,Ac,0.4,d,alpha,theta)
Vsu=comprobVEHE08.getVsuEHE08(z,alpha,theta,AsTrsv,fyd)
ratio1=abs(Fcv-0.375e6)/0.375e6
ratio2=abs(Vcu-82.607e3)/82.607e3
ratio3=abs(Vu1-1320e3)/1320e3
ratio4=abs(Vsu-111.966e3)/111.966e3


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
  

if (ratio1<0.002) and (ratio2<0.0002) and (ratio3<1e-15) and (ratio4<1e-5):
    print "test cortante EHE-08 05: ok."
else:
    print "test cortante EHE-08 05: ERROR."
  
