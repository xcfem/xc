#----------------------------------------------------------------------------
#  programa XC; cálculo mediante el método de los elementos finitos orientado
#  a la solución de problemas estructurales.
#
#  Copyright (C)  Luis Claudio Pérez Tato
#
#  El programa deriva del denominado OpenSees <http:#opensees.berkeley.edu>
#  desarrollado por el «Pacific earthquake engineering research center».
#
#  Salvo las restricciones que puedan derivarse del copyright del
#  programa original (ver archivo copyright_opensees.txt) este
#  software es libre: usted puede redistribuirlo y/o modificarlo 
#  bajo los términos de la Licencia Pública General GNU publicada 
#  por la Fundación para el Software Libre, ya sea la versión 3 
#  de la Licencia, o (a su elección) cualquier versión posterior.
#
#  Este software se distribuye con la esperanza de que sea útil, pero 
#  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
#  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
#  Consulte los detalles de la Licencia Pública General GNU para obtener 
#  una información más detallada. 
#
# Debería haber recibido una copia de la Licencia Pública General GNU 
# junto a este programa. 
# En caso contrario, consulte <http:#www.gnu.org/licenses/>.
#----------------------------------------------------------------------------
#masonry_bridge.py

import math

def yAxis(f,j,k,r,x):
  "Polynomial interpolation for arc's axis."
  return f*math.pow(x,4)+j*math.pow(x,3)+k*math.pow(x,2)+r*x

def calcGamma(f,j,k,r,x):
  "Angle of tangent to arc at x."
  return math.atan(4*f*math.pow(x,3)+3*j*math.pow(x,2)+2*k*x+r)

def aux1(f,j,k,r,x1,x0):
  "Aux function."
  return -f*(pow(x1,5)-pow(x0,5))/5-j*(pow(x1,4)-pow(x0,4))/4-k*(pow(x1,3)-pow(x0,3))/3-r*(pow(x1,2)-pow(x0,2))/2

def aux2(f,j,k,r,x2):
  "Aux function."
  return -f*math.pow(x2,6)/30-j*math.pow(x2,5)/20-k*math.pow(x2,4)/12-r*math.pow(x2,3)/6-f*math.pow(x2,6)/6-j*math.pow(x2,5)/5

def vQtrans(v,delta,hRcle):
  "Largeur participante pour la charge transversale (voir 6.5 et figure 6.13)."
  retval= v
  if hRcle<0.8/math.tan(delta):
    retval= 2+0.4+hRcle*math.tan(delta)
  else:
    retval= 4
  retval= min(retval,v)
  return retval

def lQtrans(a,delta,hRcle):
  "Longueur participante pour la charge transversale (voir eqs. 6.16 et 6.17)."
  return min(0.4+2*hRcle*math.tan(delta),2*a)

def diagInteraction(N,d,v,alpha,beta):
  "Moment de flexion qui correspond au axil N au diagramme d'interaction."
  return -N/2*(-3*d*1000*beta**2*v*1000+math.sqrt(3)*beta**2*d*1000*v*1000-2*alpha*N-2*math.sqrt(3)*alpha*N)/(beta**2*v*1000*(-3+math.sqrt(3)))/1000

def calcE6p27(X,qrep,L,LR,v,l,a,b,hA,hB,hC,hD,xA):
  "Grandeur E pour le changement de variable selon l'equation 6.27"
  num1= -LR*v*l**2/8+a*l*v*(LR-a)
  denom1= a*(hA-hB)+LR*(hC-hA)
  fact1= b/a*(hC-hA)+hD-hB
  num2A= -LR*v*a**2/2
  num2BA= a*v*(L/2-xA)
  num2BB= LR-L/4+xA/2
  num2B= num2BA*num2BB
  num2= num2A+num2B
  sum1= X*(num1/denom1*fact1-b*l*v+b*v*l**2/8/a)
  sum2= qrep*(num2/denom1*fact1+b*a*v/2-b*v*(L/2-xA))
  retval= (sum1+sum2)
  # print "****************************"
  # print "num1= ",num1
  # print "denom1= ",denom1
  # print "fact1= ", fact1
  # print "LR= ",LR, " L= ",L," v= ",v," a= ", a, " xA= ",xA 
  # print "num2A= ",num2A
  # print "num2BA= ",num2BA
  # print "num2BB= ",num2BB
  # print "num2B= ",num2B
  # print "num2= ", num2
  # print "sum1= ", sum1
  # print "sum2= ", sum2
  # print "****************************"
  return retval

def calcF6p28(R,LR,a,b,eta,phiS,etaW,psiT,MA,MB,MC,RzB,RzD,hA,hB,hC,hD):
  "Grandeur F pour le changement de variable selon l'equation 6.28"
  sum1= -b*eta
  sum2= b*(MA+MC+phiS)/a
  num3= a*(etaW+MA+MB+RzB)-LR*(MA+MC+phiS)
  denom3= a*(hA-hB)+LR*(hC-hA)
  fact3= (b*(hC-hA)/a+hD-hB)
  sum3= num3/denom3*fact3
  sum4= -R*(hD-hB)-MB+psiT+RzD
  retval= sum1+sum2+sum3+sum4
  return retval

def calcG6p29(X,qrep,L,LR,v,l,a,b,hA,hB,hC,hD,xA,gammaD):
  "Grandeur G pour le changement de variable selon l'equation 6.29"
  num1= -LR*v*l**2/8+a*l*v*(LR-a)
  denom1= a*(hA-hB)+LR*(hC-hA)
  fact1= -math.cos(gammaD)+math.sin(gammaD)*(hC-hA)/a
  num2A= -LR*v*a**2/2
  num2BA= a*v*(L/2-xA)
  num2BB= LR-L/4+xA/2
  num2B= num2BA*num2BB
  num2= num2A+num2B
  sum1= X*(num1/denom1*fact1+math.sin(gammaD)*l*v*(l/8/a-1))
  sum2= qrep*(num2/denom1*fact1+v*math.sin(gammaD)*(a/2-L/2+xA))
  retval= (sum1+sum2)
  # print "======================="
  # print "X= ", X
  # print "qrep= ", qrep
  # print "LR= ", LR
  # print "l= ", l
  # print "v= ", v
  # print "xA= ", xA
  # print "hA= ",hA, " m"
  # print "hC= ",hC, " m"
  # print "hD= ",hD, " m"
  # print "hB= ",hB, " m"
  # print "gammaD= ", gammaD
  # print "sum1= ", sum1
  # print "sum2= ", sum2
  # print "retval= ", retval
  # print "======================="
  return retval

def calcH6p30(LR,a,eta,psi,phiS,etaW,MA,MB,MC,RzB,hA,hB,hC,hD,gammaD):
  "Grandeur H pour le changement de variable selon l'equation 6.30"
  num1A= a*(etaW+MA+MB+RzB)
  num1B= LR*(MA+MC+phiS)
  num1= num1A-num1B
  denom1= a*(hA-hB)+LR*(hC-hA)
  fact1= -math.cos(gammaD)+math.sin(gammaD)*(hC-hA)/a
  sum1= num1/denom1*fact1
  sum2= math.sin(gammaD)*(psi-eta+(MA+MC+phiS)/a)
  retval= sum1+sum2
  # print "****************************"
  # print "etaW= ",etaW
  # print "num1= ",num1
  # print "num1A= ",num1A
  # print "num1B= ",num1B
  # print "denom1= ",denom1
  # print "fact1= ", fact1
  # print "LR= ",LR, " L= ",L," v= ",v," a= ", a, " xA= ",xA 
  # print "sum1= ", sum1
  # print "sum2= ", sum2
  # print "retval= ", retval
  # print "****************************"
  return retval

def calcn6p32(alpha,beta,d,v,E,F,G,H):
  "Multiplicateur de la charge utile."
  dM= d*1000 #Units 
  vM= v*1000
  EM= E*1000
  FM= F*1000
  fact= 1.0/2.0/G/G/alpha/(6+4*math.sqrt(3))
  sum= -3*G*dM*beta*beta*vM
  sum-= 12*G*alpha*H
  sum-= 8*math.sqrt(3)*G*alpha*H
  sum-= 6*EM*beta*beta*vM
  radicand= 9*(G*dM*beta*beta*vM)**2
  radicand+= 36*G*dM*pow(beta,4)*vM*vM*EM
  radicand+= 144*G*alpha*H*EM*beta*beta*vM
  radicand+= 96*G*alpha*H*math.sqrt(3)*EM*beta*beta*vM
  radicand+= 36*EM*EM*pow(beta,4)*vM*vM
  radicand-= 144*G*G*alpha*FM*beta*beta*vM
  pp= 96*G*G*alpha*math.sqrt(3)*FM*beta*beta*vM
  radicand-= 96*G*G*alpha*math.sqrt(3)*FM*beta*beta*vM
  #print "radicand= ",radicand
  sum+= math.sqrt(radicand)
  # print "======================="
  # print "alpha= ", alpha
  # print "beta= ", beta
  # print "dM= ", dM
  # print "vM= ", vM
  # print "EM= ",EM
  # print "FM= ",FM
  # print "G= ",G
  # print "H= ",H
  # print "sum= ", sum
  # print "fact= ", fact
  # print "n= ",fact*sum
  return fact*sum 

def printVouteResults(L,f,j,k,r,a,b,gammaD,hA,hB,hC,hD,LR,d,v,hR,hS,hHalfL,alpha,beta,PPR,PPS,eta,etaW,phi,phiS,psi,psiT,mp,Kp,mc,Kc,R,RzB,RzD,qtrans,X,Nadmis,Madmis,E,F,G,H,n):
  print "Overture de la voûte L= ", L," m"
  print "Paramètres de l'équation polynomiale: "
  print "  f= ", f
  print "  j= ", j
  print "  k= ", k
  print "  r= ", r
  print "  u= ", 0
  print "Distance séparant les rotules A et C; a= ",a," m"
  print "Distance séparant les rotules D et B; b= ",b," m"
  print "Angle determiné par la perpendiculaire à la tangente à l'axe en D et la verticale (voir 6.2 et A 10.3); gammaD= ",gammaD, " rad"
  print "Hauteur des rotules: "
  print "  hA= ",hA, " m"
  print "  hB= ",hB, " m"
  print "  hC= ",hC, " m"
  print "  hD= ",hD, " m"
  print "  LR= ",LR, " m"
  print "Épaisseur de l'arc d= ",d, " m"
  print "Largueur efficace de l'arc; v= ",v, " m"
  print "Hauteur de remplissage à la naissance de l'arc; hR= ",hR, " m"
  print "Hauteur équivalente de la voie de roulement; hS= ",hS, " m"
  print "Flèche: h(L/2)= ",hHalfL, " m"
  print "Premier paramètre de la courbe contrainte - déformation parabolique; alpha= ",alpha
  print "Second paramètre de la courbe contrainte - déformation parabolique; beta= ",beta
  print "Poids propre du matériau de remplissage; PPR= ",PPR/1000," kN/m3"
  print "Poids propre de la voie de roulement; PPS= ",PPS/1000," kN/m3"
  print "Résultante de la charge permanente (voir 6.3 et A 11.1); eta= ",eta," N"
  print "Moment de flexion induit par la resultante de la charge permanente, par rapport à la rotule B  (voir 6.4 et A 11.2); etaW= ",etaW," Nm"
  print "Résultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules A et C (voir 6.5 et A 11.3); phi= ",phi," N"
  print "Moment de flexion induit par la resultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules A et C, par rapport à la rotule C (voir 6.6 et A 11.4); phiS= ",phiS," Nm"
  print "Résultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules D et B (voir 6.7 et A 11.5); psi= ",psi," N"
  print "Moment de flexion induit  par la resultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules B et D, par rapport à la rotule D (voir 6.8 et A 11.6); psiT= ",psiT," Nm"
  print "Facteur de correction; mp= ",mp
  print "Coefficient de pousée des terres (voir 6.10); Kp= ",Kp
  print "Facteur de correction; mc= ",mc
  print "Coefficient de pousée des terres (voir 6.11); Kc= ",Kc
  print "Resultante de la force horizontale (voir 6.12 et A 12.1); R= ",R/1e3, " kN"
  print "Moment de flexion induit par la résultante de la force horizontale R para rapport a la rotule B (voir 6.13 et A 12.2); RzB= ", RzB, " N"
  print "Moment de flexion induit par la résultante de la force horizontale R para rapport a la rotule D (voir 6.14 et A 12.3); RzD= ", RzD, " N"
  #print "vQt= ",vQt," m"
  print "Charge de trafic uniformemente répartie après diffussion transversale (voir 6-15) qtrans= ",qtrans," N/m"
  #print "lQt= ",lQt," m"
  print "Charge de trafic ponctuelle aprés diffusion longitudinale et transversale (void 6.18); X= ",X/1e6," MPa"
  print "Effort normal admis; Nadmis= ",Nadmis/1e3," kN"
  print "Moment de flexion admis (voir 5.17 et A 7.15); Madmis= ",Madmis/1e3," kNm"
  print "Changement de variable:"
  print "  (voir 6.27 et A 13.22); E= ",E
  print "  (voir 6.28 et A 13.23); F= ",F
  print "  (voir 6.29 et A 13.34); G= ",G
  print "  (voir 6.30 et A 13.35); H= ",H
  print "Multiplicateur limite des charges utiles (voir 6.32); n= ",n
