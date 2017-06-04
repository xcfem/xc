# -*- coding: utf-8 -*-
# Comprobación de una sección de hormigón frente a cortante.

import math

def getFcvEH91(fcd):
  '''
  Devuelve el valor de fcv (resistencia virtual a cortante del hormigón)
   según la antigua EH-91.

  :param fcd: Design compressive strength of concrete.
  :param b: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  
  '''
  fcdKpcm2= fcd*9.81/1e6
  return 0.5*sqrt(fcdKpcm2)/9.81*1e6


def getVu1(fcd, Nd, Ac, b0, d, alpha, theta):
  '''
  Devuelve el valor de Vu1 (cortante de agotamiento por compresión oblicua del alma)
   según el artículo 44.2.3.1 de EHE.

  :param fcd: Design compressive strength of concrete.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil.
  :param alpha: Angle between the shear rebars and the member axis (figure 44.2.3.1.a EHE).
  :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  
  '''
  f1cd= 0.6*fcd
  sgpcd= (Nd/Ac)
  K= min(5/3*(1+sgpcd/fcd),1)
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  return K*f1cd*b0*d*(ctgTheta+cotg(alpha))/(1+ctgTheta^2)

def getFcv(fact, fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  '''
  Devuelve el valor de fcv (resistencia virtual a cortante del hormigón)
     para piezas CON Ó SIN reinforcement de cortante, según el artículos 44.2.3.2.1 y  44.2.3.2.2 de EHE.
  
  :param fact: Factor que toma el valor 0.12 para piezas SIN reinforcement de cortante y 0.10 para piezas CON
  reinforcement de cortante.
  :param fck: Concrete characteristic compressive strength.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
  :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
    
  '''
  sgpcd= Nd/Ac
  chi= 1+sqrt(200/(d*1000)) # d HA DE EXPRESARSE EN METROS.
  rol= min((AsPas+AsAct*fpd/fyd)/(b0*d),0.02)
  return fact*chi*(rol*fck/1e4)^(1/3)*1e6-0.15*sgpcd


def getVu2SIN(fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  '''
  Devuelve el valor de Vu2 (cortante de agotamiento por tracción en el alma)
     para piezas SIN reinforcement de cortante, según el artículo 44.2.3.2.1 de EHE.
  
  :param fck: Concrete characteristic compressive strength.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
  :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
   
  '''
  return getFcv(0.12,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)*b0*d

def getVcu(fck, Nd, Ac, b0, d, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd):
  '''
  Devuelve el valor de Vcu (contribución del hormigón a la resistencia a cortante) para piezas CON reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.

  :param fck: Concrete characteristic compressive strength.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
  :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
  :param sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
  :param sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.    
  '''
  fcv= getFcv(0.10,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd) 
  fctm= fctMedEHE08(fck) 
  ctgThetaE= min(max(0.5,sqrt(fctm-sgxd/fctm-sgyd)),2.0)
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  beta= 0.0
  if(ctgTheta<ctgThetaE):
    beta= (2*ctgTheta-1)/(2*ctgThetaE-1)
  else:
    beta= (ctgTheta-2)/(ctgThetaE-2)
  return fcv*b0*d*beta

def getVsu(z, alpha, theta, AsTrsv, fyd):
  '''
  Devuelve el valor de Vsu (contribución de la reinforcement transversal de alma a la resistencia a cortante)
   para piezas CON reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
  :param z: Lever arm.
  :param alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
  :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  :param AsTrsv: Área de la reinforcement transversal.
  :param fyd: Resistencia de cálculo de la reinforcement transversal.
  
  '''
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  fyalphad= min(fyd,400e6) # comentario al artículo 40.2 EHE
  return z*sin(alpha)*(cotg(alpha)+ctgTheta)*AsTrsv*fyalphad


def getVu2(fck, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
  '''
  Devuelve el valor de Vu2 para piezas CON o SIN reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.

  :param fck: Concrete characteristic compressive strength.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  :param z: Lever arm.
  :param alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
  :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
  :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
  :param sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
  :param sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.
  :param AsTrsv: Área de la reinforcement transversal.
  :param fyd: Resistencia de cálculo de la reinforcement transversal.
  '''
  vcu= 0.0
  vsu= 0.0
  if(AsTrsv>0.0):
    vcu= getVcu(fck,Nd,Ac,b0,d,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd)
    vsu= getVsu(z,alpha,theta,AsTrsv,fydTrsv)
  else:
    vcu= getVu2SIN(fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
    vsu= 0.0
  return vcu+vsu

def getVu(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
  '''
  Devuelve el valor de Vu= max(Vu1,Vu2) para piezas CON o SIN reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
  :param fck: Concrete characteristic compressive strength.
  :param Nd: Axil de cálculo (positivo si es de tracción).
  :param Ac: Area total de la sección de hormigón.
  :param b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  :param d: Canto útil (meters).
  :param z: Lever arm.
  :param alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
  :param theta: Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
  :param AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
  :param AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
  a una distancia mayor que el depth útil de la sección en estudio.
  :param fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
  :param sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
  :param sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.
  :param AsTrsv: Área de la reinforcement transversal.
  :param fyd: Resistencia de cálculo de la reinforcement transversal.
  '''
  vu1= getVu1(fcd,Nd,Ac,b0,d,alpha,theta)
  vu2= getVu2(fck,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  return min(vu1,vu2)

# Comprobación a cortante
def cumpleCortante(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
  vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  if(Vrd<=vu):
    return True
  else:
    return False

# Aprovechamiento a cortante.
def aprovCortante(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv, Vrd):
  vu= getVu(fck,fcd,Nd,Ac,b0,d,z,alpha,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd,AsTrsv,fydTrsv)
  return Vrd/vu

class ParamsCortante(object):
  '''Define las propiedades del registro que contiene los parámetros de cálculo
   a cortante.'''
  def __init__(self):
    self.concreteArea= 0.0 # Area total de la sección de hormigón.
    self.widthMin= 0.0 # Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
    self.depthUtil= 0.0 # Canto útil (meters).
    self.brazoMecanico= 0.0 # Lever arm (meters).
    self.areaRebarTracc= 0.0 # Área de la reinforcement longitudinal pasiva traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
    self.areaTendonesTracc= 0.0 # Área de la reinforcement activa longitudinal pasiva traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
    self.areaShReinfBranchsTrsv= 0.0 # Área de la reinforcement transversal.
    self.sigmaXD= 0.0 # Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
    self.sigmaYD= 0.0 # Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.

    self.angAlpha= math.pi/2 # Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
    self.angTheta= math.pi/6 # Angle between the concrete compressed struts and the member axis (figure 44.2.3.1.a EHE).
    self.cortanteUltimo= 0.0

  # Calcula el valor del cortante último de la sección.
  def calcCortanteUltimo(self, concreteFibersSet, rebarFibersSet,tensionedRebarsFiberSet, fck, fcd, fyd, fpd, fydTrsv):
    self.concreteArea= concreteFibersSet.getArea()
    self.widthMin= concreteFibersSet.getAnchoMecanico() # Mejorar (en secciones no convexas estará mal).
    self.depthUtil= concreteFibersSet.getCantoUtil()
    self.brazoMecanico= concreteFibersSet.getBrazoMecanico()
    self.areaRebarTracc= tensionedRebarsFiberSet.getArea
    # areaTendonesTracc= 

    self.sigmaXD= N/area+Mz/Iz*yCdg+My/Iy*zCdg
    self.cortanteUltimo= getVu(fck,fcd,N,self.concreteArea,self.widthMin,self.depthUtil,self.brazoMecanico,self.angAlpha,self.angTheta,self.areaRebarTracc,fyd,self.areaTendonesTracc,fpd,self.sigmaXD,self.sigmaYD,AsTrsv,self.areaShReinfBranchsTrsv,fydTrsv)

  # Imprime los parámetros de cortante de la sección.
  def printParams(self):
    print "Area de las reinforcement traccionadas; As= ",self.areaRebarTracc*1e4," cm2"
    print "Área de la reinforcement transversal; AsTrsv= ",self.areaShReinfBranchsTrsv*1e4," cm2"
    print "Tensión normal de cálculo = sigmaXD",self.sigmaXD/1e6," MPa"
    print "Canto útil; d= ",self.depthUtil," m"
    print "Ancho mínimo; b0= ",self.widthMin," m"
    print "Lever arm; z= ",self.brazoMecanico," m"
    print "cortanteUltimo; Vu= ",self.cortanteUltimo/1e3," kN"

