#-*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2017, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import math
import sys
from scipy.optimize import minimize

#Basic functions.
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

class archGeometry(object):
  """
  Arch deometric definition:
   coefPolArch=[f,j,k,r]: Coefficients of polynomial y=fx^4+jx^3+kx^2+rx+u (u=0)
   XRot=[xA,xB,xC,xD]:  X coordinate rotules A,B,C,D [m]
   arcThick: arch thickness [m]
   arcSpan: arch span [m]
  """
  coefPolArch=[0,0,0,0] #[f,j,k,r] coefficients of polynomial y=fx^4+jx^3+kx^2+rx+u (u=0)
  XRot=[0,0,0,0]        #[xA,xB,xC,xD] X coordinates rotules
  arcThick=0            #arch thickness (m)
  arcSpan=15            #arch span [m]
  arcEffL=4             #arch effective length [m]
  def __init__(self,coefPolArch=[0,0,0,0],XRot=[0,0,0,0],arcThick=0,arcSpan=15,arcEffL=4):
    self.coefPolArch=coefPolArch
    self.XRot=XRot
    self.arcThick=arcThick
    self.arcSpan=arcSpan
    self.arcEffL=arcEffL
  def getYRot(self):
    """YRot=[yA,yB,yC,yD]: Y coordinate of rotules A,B,C,D [m]"""
    YRot=[]
    for x in self.XRot:
      YRot.append(yAxis(self.coefPolArch[0],self.coefPolArch[1],self.coefPolArch[2],self.coefPolArch[3],x))
    return YRot
  def getGammaD(self):
    """angle at rotule D """
    x=self.XRot[3]
    return -calcGamma(self.coefPolArch[0],self.coefPolArch[1],self.coefPolArch[2],self.coefPolArch[3],x)
  def getDistxAC(self):
    return self.XRot[2]-self.XRot[0]
  def getDistxBD(self):
    return self.XRot[1]-self.XRot[3]
  def getDistxAB(self):
    return self.XRot[1]-self.XRot[0]
  def getYKeystone(self):
    """Y coordinate in keystone of arch (at arcSpan/2)""" 
    x=self.arcSpan/2
    return yAxis(self.coefPolArch[0],self.coefPolArch[1],self.coefPolArch[2],self.coefPolArch[3],x)
  def printResults(self):
    print "Overture de la voûte L= ", self.arcSpan," m"
    print "Paramètres de l'équation polynomiale: "
    print "  f= ", self.coefPolArch[0]
    print "  j= ", self.coefPolArch[1]
    print "  k= ", self.coefPolArch[2]
    print "  r= ", self.coefPolArch[3]
    print "  u= ", 0
    print "Distance séparant les rotules A et C; a= ",self.getDistxAC()," m"
    print "Distance séparant les rotules D et B; b= ",self.getDistxBD()," m"
    print "Angle determiné par la perpendiculaire à la tangente à l'axe en D et la verticale (voir 6.2 et A 10.3); gammaD= ",self.getGammaD(), " rad"
    print "Hauteur des rotules: "
    print "  hA= ",self.getYRot()[0], " m"
    print "  hB= ",self.getYRot()[1], " m"
    print "  hC= ",self.getYRot()[2], " m"
    print "  hD= ",self.getYRot()[3], " m"
    print "  LR= ",self.getDistxAB(), " m"
    print "Épaisseur de l'arc d= ",self.arcThick, " m"
    print "Largueur efficace de l'arc; v= ",self.arcEffL, " m"
    print "Flèche: h(L/2)= ",self.getYKeystone(), " m"
  

class FillingCharacteristics(object): 
  angPhi= math.radians(30)   #angle de frottement interne
  cohesion= 0                #cohésion
  mp= 0.33 #Correction factor.
  mc= 0.01 #Correction factor.
  alpha= 0.726
  beta= 6.095
  swFill=18e3   #specific weight of filling material [N/m3]
  swSupStr=20e3 #specific weight or superstructure [N/m3]
  fillThick=9  #thickness of filling material in the endpoint of the arch
  eqThickRoad=0.5  #equivalent thickness of road material
  def __init__(self,a= math.radians(30.0),c= 0.0,mp= 0.33,mc= 0.01,alpha= 0.726,beta= 6.095,swFill=18e3,swSupStr=20e3,fillThick=9,eqThickRoad=0.5):
    self.angPhi= a
    self.cohesion= c
    self.mp= mp
    self.mc= mc
    self.alpha=alpha
    self.beta=beta
    self.swFill=swFill
    self.swSupStr=swSupStr
    self.fillThick=fillThick
    self.eqThickRoad=eqThickRoad
  def getKp(self):
    """coefficient de poussée des terres"""
    return pow(math.tan(math.pi/4+self.angPhi/2),2)
  def getKc(self):
    return 2*math.sqrt(self.getKp())
  def printResults(self):
    print "Hauteur de remplissage à la naissance de l'arc; hR= ",self.fillThick, " m"
    print "Hauteur équivalente de la voie de roulement; hS= ",self.eqThickRoad, " m"
    print "Premier paramètre de la courbe contrainte - déformation parabolique; alpha= ",self.alpha
    print "Second paramètre de la courbe contrainte - déformation parabolique; beta= ",self.beta
    print "Poids propre du matériau de remplissage; PPR= ",self.swFill/1000," kN/m3"
    print "Poids propre de la voie de roulement; PPS= ",self.swSupStr/1000," kN/m3"

class trafficLoad(object):
  delta= math.radians(30)
  fillThickKeys= 1.5 # Hauteur du remplissage sur la clé de la voûte (m).
  Q= 160000 # Charge ponctuelle due au trafic (N).
  qrep= 0.005e6 # Charge uniformément repartie due au trafic (Pa).
  def __init__(self,delta= math.radians(30),fillThickKeys= 1.5,Q= 160000,qrep= 0.005e6):
    self.delta=delta
    self.fillThickKeys=fillThickKeys
    self.Q=Q
    self.qrep=qrep

class permLoadResult(object):
  """Permanent load resultants"""
  gm= archGeometry()
  fc= FillingCharacteristics()
  def __init__(self,gm,fc):
    self.gm= gm
    self.fc= fc
  def getEta(self):
    """Résultante des charges permanentes sur l'overture active de l'arc (fig. 6.9) [N]"""
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*self.gm.getDistxAB()+self.fc.swFill*(self.fc.fillThick*self.gm.getDistxAB()+aux1(self.gm.coefPolArch[0],self.gm.coefPolArch[1],self.gm.coefPolArch[2],self.gm.coefPolArch[3],self.gm.XRot[1],self.gm.XRot[0])))

  def getEtaW(self):
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*pow(self.gm.getDistxAB(),2)/2+self.fc.swFill*(self.fc.fillThick*pow(self.gm.getDistxAB(),2)/2-self.gm.coefPolArch[0]*pow(self.gm.XRot[1],6)/30-self.gm.coefPolArch[1]*pow(self.gm.XRot[1],5)/20-self.gm.coefPolArch[2]*pow(self.gm.XRot[1],4)/12-self.gm.coefPolArch[3]*pow(self.gm.XRot[1],3)/6-self.gm.coefPolArch[0]*pow(self.gm.XRot[0],6)/6-self.gm.coefPolArch[1]*pow(self.gm.XRot[0],5)/5+self.gm.coefPolArch[0]*pow(self.gm.XRot[0],5)*self.gm.XRot[1]/5-self.gm.coefPolArch[2]*pow(self.gm.XRot[0],4)/4+self.gm.coefPolArch[1]*pow(self.gm.XRot[0],4)*self.gm.XRot[1]/4-self.gm.coefPolArch[3]*pow(self.gm.XRot[0],3)/3+self.gm.coefPolArch[2]*pow(self.gm.XRot[0],3)*self.gm.XRot[1]/3+self.gm.coefPolArch[3]*pow(self.gm.XRot[0],2)*self.gm.XRot[1]/2))

  def getPhi(self):
    """résultante des charges permanentes solicitant la portion d'arc comprise entre les rotules A et C (fig. 6.9) [N]"""
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*self.gm.getDistxAC()+self.fc.swFill*(self.fc.fillThick*self.gm.getDistxAC()+aux1(self.gm.coefPolArch[0],self.gm.coefPolArch[1],self.gm.coefPolArch[2],self.gm.coefPolArch[3],self.gm.XRot[2],self.gm.XRot[0])))

  def getPsi(self):
    """résultante des charges permanentes solicitant la portion d'arc comprise entre les rotules D et B (fig. 6.9) [N] """
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*self.gm.getDistxBD()+self.fc.swFill*(self.fc.fillThick*self.gm.getDistxBD()+aux1(self.gm.coefPolArch[0],self.gm.coefPolArch[1],self.gm.coefPolArch[2],self.gm.coefPolArch[3],self.gm.XRot[1],self.gm.XRot[3])))

  def getPhiS(self):
    """moment de flexion induit par la résultante phi de la charge permanente entre A et C, par rapport à la rotule C (fig. 6.9) [Nm] """
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*pow(self.gm.getDistxAC(),2)/2+self.fc.swFill*(self.fc.fillThick*pow(self.gm.getDistxAC(),2)/2-self.gm.coefPolArch[0]*pow(self.gm.XRot[2],6)/30-self.gm.coefPolArch[1]*pow(self.gm.XRot[2],5)/20-self.gm.coefPolArch[2]*pow(self.gm.XRot[2],4)/12-self.gm.coefPolArch[3]*pow(self.gm.XRot[2],3)/6-self.gm.coefPolArch[0]*pow(self.gm.XRot[0],6)/6-self.gm.coefPolArch[1]*pow(self.gm.XRot[0],5)/5+self.gm.coefPolArch[0]*pow(self.gm.XRot[0],5)*self.gm.XRot[2]/5-self.gm.coefPolArch[2]*pow(self.gm.XRot[0],4)/4+self.gm.coefPolArch[1]*pow(self.gm.XRot[0],4)*self.gm.XRot[2]/4-self.gm.coefPolArch[3]*pow(self.gm.XRot[0],3)/3+self.gm.coefPolArch[2]*pow(self.gm.XRot[0],3)*self.gm.XRot[2]/3+self.gm.coefPolArch[3]*pow(self.gm.XRot[0],2)*self.gm.XRot[2]/2))

  def getPsiT(self):
    """moment de flexion induit par la résultante psi de la charge permanente entre D et B, par rapport à la rotule D (fig. 6.9) [Nm] """
    return self.gm.arcEffL*(self.fc.swSupStr*self.fc.eqThickRoad*pow(self.gm.getDistxBD(),2)/2+self.fc.swFill*(self.fc.fillThick*pow(self.gm.getDistxBD(),2)/2-self.gm.coefPolArch[0]*pow(self.gm.XRot[3],6)/30-self.gm.coefPolArch[1]*pow(self.gm.XRot[3],5)/20-self.gm.coefPolArch[2]*pow(self.gm.XRot[3],4)/12-self.gm.coefPolArch[3]*pow(self.gm.XRot[3],3)/6-self.gm.coefPolArch[0]*pow(self.gm.XRot[1],6)/6-self.gm.coefPolArch[1]*pow(self.gm.XRot[1],5)/5+self.gm.coefPolArch[0]*pow(self.gm.XRot[1],5)*self.gm.XRot[3]/5-self.gm.coefPolArch[2]*pow(self.gm.XRot[1],4)/4+self.gm.coefPolArch[1]*pow(self.gm.XRot[1],4)*self.gm.XRot[3]/4-self.gm.coefPolArch[3]*pow(self.gm.XRot[1],3)/3+self.gm.coefPolArch[2]*pow(self.gm.XRot[1],3)*self.gm.XRot[3]/3+self.gm.coefPolArch[3]*pow(self.gm.XRot[1],2)*self.gm.XRot[3]/2))

  def getR(self):
    """résultant de la poussée laterale entre les rotules B et D [N]"""
    return self.gm.arcEffL*(self.fc.getKp()*self.fc.mp*(self.fc.eqThickRoad*self.fc.swSupStr*(self.gm.getYRot()[3]-self.gm.getYRot()[1])+self.fc.swFill*self.fc.fillThick*(self.gm.getYRot()[3]-self.gm.getYRot()[1])-self.fc.swFill*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2))+self.fc.cohesion*self.fc.mc*self.fc.getKc()*(self.gm.getYRot()[3]-self.gm.getYRot()[1]))

  def getRzB(self):
    """moment de flexion induit par la résultant de la poussée laterale entre les rotules B et D, par rapport à la rotule B [Nm]"""
    return self.gm.arcEffL*(self.fc.getKp()*self.fc.mp*(self.fc.eqThickRoad*self.fc.swSupStr*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2-self.gm.getYRot()[1]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])))+self.fc.swFill*(self.fc.fillThick*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2)-self.fc.fillThick*self.gm.getYRot()[1]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])-pow(self.gm.getYRot()[3],3)/3+pow(self.gm.getYRot()[1],3)/3+self.gm.getYRot()[1]*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2))+self.fc.cohesion*self.fc.mc*self.fc.getKc()*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2-self.gm.getYRot()[1]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])))


  def getRzD(self):
    """moment de flexion induit par la résultant de la poussée laterale entre les rotules B et D, par rapport à la rotule D [Nm]"""
    return self.gm.arcEffL*(self.fc.getKp()*self.fc.mp*(self.fc.eqThickRoad*self.fc.swSupStr*(self.gm.getYRot()[3]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])-self.gm.getYRot()[3]*self.gm.getYRot()[3]/2+self.gm.getYRot()[1]*self.gm.getYRot()[1]/2))+self.fc.swFill*(self.fc.fillThick*self.gm.getYRot()[3]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])-self.fc.fillThick*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2)-self.gm.getYRot()[3]*(self.gm.getYRot()[3]*self.gm.getYRot()[3]/2-self.gm.getYRot()[1]*self.gm.getYRot()[1]/2)+pow(self.gm.getYRot()[3],3)/3-pow(self.gm.getYRot()[1],3)/3)+self.fc.cohesion*self.fc.mc*self.fc.getKc()*(self.gm.getYRot()[3]*(self.gm.getYRot()[3]-self.gm.getYRot()[1])-self.gm.getYRot()[3]*self.gm.getYRot()[3]/2+self.gm.getYRot()[1]*self.gm.getYRot()[1]/2))

  def printResults(self,fillChar):
    print "Résultante de la charge permanente (voir 6.3 et A 11.1); eta= ",self.getEta()," N"
    print "Moment de flexion induit par la resultante de la charge permanente, par rapport à la rotule B  (voir 6.4 et A 11.2); etaW= ",self.getEtaW()," N.m"
    print "Résultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules A et C (voir 6.5 et A 11.3); phi= ",self.getPhi()," N"
    print "Moment de flexion induit par la resultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules A et C, par rapport à la rotule C (voir 6.6 et A 11.4); phiS= ",self.getPhiS()," Nm"
    print "Résultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules D et B (voir 6.7 et A 11.5); psi= ",self.getPsi," N"
    print "Moment de flexion induit  par la resultante de la charge permanente appliquée sur la portion d'arc comprise entre les rotules B et D, par rapport à la rotule D (voir 6.8 et A 11.6); psiT= ",self.getPsiT()," Nm"
    print "Facteur de correction; mp= ",fillChar.mp
    print "Coefficient de pousée des terres (voir 6.10); Kp= ",fillChar.getKp()
    print "Facteur de correction; mc= ",fillChar.mc
    print "Coefficient de pousée des terres (voir 6.11); Kc= ",fillChar.getKc()
    print "Resultante de la force horizontale (voir 6.12 et A 12.1); R= ",self.getR()/1e3, " kN"
    print "Moment de flexion induit par la résultante de la force horizontale R para rapport a la rotule B (voir 6.13 et A 12.2); RzB= ", self.getRzB(), " N"
    print "Moment de flexion induit par la résultante de la force horizontale R para rapport a la rotule D (voir 6.14 et A 12.3); RzD= ", self.getRzD(), " N"
    

class trafficLoadResult(object):
  """Traffic load resultants"""
  gm= archGeometry()
  tl= trafficLoad()
  def __init__(self,gm,tl):
    self.gm= gm
    self.tl= tl

  def getvQt(self):
    return vQtrans(self.gm.arcEffL,self.tl.delta,self.tl.fillThickKeys)

  def getqtrans(self):
    """Charge de trafic uniformemente [N/m] répartie après diffussion transversale (voir 6-15)"""
    return self.tl.Q/self.getvQt()

  def getlQt(self):
    return lQtrans(self.gm.getDistxAB(),self.tl.delta,self.tl.fillThickKeys)

  def getX(self):
    """Charge de trafic ponctuelle aprés diffusion longitudinale et transversale (voir 6.18) [Pa]"""
    return self.getqtrans()/self.getlQt()

  def printResults(self):
    #print "vQt= ",vQt," m"
    print "Charge de trafic uniformemente répartie après diffussion transversale (voir 6-15) qtrans= ",self.getqtrans()," N/m"
    #print "lQt= ",lQt," m"
    print "Charge de trafic ponctuelle aprés diffusion longitudinale et transversale (void 6.18); X= ",self.getX()/1e6," MPa"

class resistance(object):
  Nadmis=0    #Effort axial admisible [N]
  gm= archGeometry()
  fc= FillingCharacteristics()
  tl=trafficLoad()
  plR= permLoadResult(gm,fc)
  tlR= trafficLoadResult(gm,tl)
  def __init__(self,Nadmis,gm,fc,tl,plR,tlR):
    self.Nadmis=Nadmis
    self.gm= gm
    self.fc= fc
    self.tl= tl
    self.plR= plR
    self.tlR= tlR

  def getMadmis(self):
    """Moment de flexion admis (voir 5.17 et A 7.15) [Nm]"""
    return diagInteraction(self.Nadmis,self.gm.arcThick,self.gm.arcEffL,self.fc.alpha,self.fc.beta)
  def getE(self):
    return calcE6p27(self.tlR.getX(),self.tl.qrep,self.gm.arcSpan,self.gm.getDistxAB(),self.gm.arcEffL,self.tlR.getlQt(),self.gm.getDistxAC(),self.gm.getDistxBD(),self.gm.getYRot()[0],self.gm.getYRot()[1],self.gm.getYRot()[2],self.gm.getYRot()[3],self.gm.XRot[0])
  def getF(self):
    return calcF6p28(self.plR.getR(),self.gm.getDistxAB(),self.gm.getDistxAC(),self.gm.getDistxBD(),self.plR.getEta(),self.plR.getPhiS(),self.plR.getEtaW(),self.plR.getPsiT(),self.getMadmis(),self.getMadmis(),self.getMadmis(),self.plR.getRzB(),self.plR.getRzD(),self.gm.getYRot()[0],self.gm.getYRot()[1],self.gm.getYRot()[2],self.gm.getYRot()[3])
  def getG(self):
    return calcG6p29(self.tlR.getX(),self.tl.qrep,self.gm.arcSpan,self.gm.getDistxAB(),self.gm.arcEffL,self.tlR.getlQt(),self.gm.getDistxAC(),self.gm.getDistxBD(),self.gm.getYRot()[0],self.gm.getYRot()[1],self.gm.getYRot()[2],self.gm.getYRot()[3],self.gm.XRot[0],self.gm.getGammaD())
  def getH(self):
    return calcH6p30(self.gm.getDistxAB(),self.gm.getDistxAC(),self.plR.getEta(),self.plR.getPsi(),self.plR.getPhiS(),self.plR.getEtaW(),self.getMadmis(),self.getMadmis(),self.getMadmis(),self.plR.getRzB(),self.gm.getYRot()[0],self.gm.getYRot()[1],self.gm.getYRot()[2],self.gm.getYRot()[3],self.gm.getGammaD())
  def getSafCoef(self):
    """Safety coefficient - Multiplicateur limite des charges utiles (voir 6.32)"""
    return calcn6p32(self.fc.alpha,self.fc.beta,self.gm.arcThick,self.gm.arcEffL,self.getE(),self.getF(),self.getG(),self.getH())
  def getMinimFunc(self,x):
    self.gm.XRot=x
    return self.getSafCoef()

  def minimize(self):
    L= self.gm.arcSpan
    x0=[L/6.0,L*5.0/6.0,L*2.0/6.0,L*4.0/6.0] 
    res=minimize(self.getMinimFunc,x0,method='Nelder-Mead')
    return res

  def printResults(self):
    self.gm.printResults()
    self.fc.printResults()
    self.plR.printResults(self.fc)
    self.tlR.printResults()
    print "Effort normal admis; Nadmis= ",self.Nadmis/1e3," kN"
    print "Moment de flexion admis (voir 5.17 et A 7.15); Madmis= ",self.getMadmis()/1e3," kN m"
    print "Changement de variable:"
    print "  (voir 6.27 et A 13.22); E= ",self.getE()
    print "  (voir 6.28 et A 13.23); F= ",self.getF()
    print "  (voir 6.29 et A 13.34); G= ",self.getG()
    print "  (voir 6.30 et A 13.35); H= ",self.getH()
    print "Multiplicateur limite des charges utiles (voir 6.32); n= ",self.getSafCoef()

