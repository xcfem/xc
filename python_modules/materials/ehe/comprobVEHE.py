# -*- coding: utf-8 -*-
# Comprobación de una sección de hormigón frente a cortante.

import math

'''
Devuelve el valor de fcv (resistencia virtual a cortante del hormigón)
   según la antigua EH-91
fcd: Resistencia de cálculo del hormigón a compresión.
b: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
  
'''
def getFcvEH91(fcd):
  fcdKpcm2= fcd*9.81/1e6
  return 0.5*sqrt(fcdKpcm2)/9.81*1e6


'''
Devuelve el valor de Vu1 (cortante de agotamiento por compresión oblicua del alma)
   según el artículo 44.2.3.1 de EHE.

fcd: Resistencia de cálculo del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil.
alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
theta: Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
  
'''
def getVu1(fcd, Nd, Ac, b0, d, alpha, theta):
  f1cd= 0.6*fcd
  sgpcd= (Nd/Ac)
  K= min(5/3*(1+sgpcd/fcd),1)
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  return K*f1cd*b0*d*(ctgTheta+cotg(alpha))/(1+ctgTheta^2)

'''
Devuelve el valor de fcv (resistencia virtual a cortante del hormigón)
   para piezas CON Ó SIN reinforcement de cortante, según el artículos 44.2.3.2.1 y  44.2.3.2.2 de EHE.

fact: Factor que toma el valor 0.12 para piezas SIN reinforcement de cortante y 0.10 para piezas CON
reinforcement de cortante.
fck: Resistencia característica del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
  
'''
def getFcv(fact, fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  sgpcd= Nd/Ac
  chi= 1+sqrt(200/(d*1000)) # d HA DE EXPRESARSE EN METROS.
  rol= min((AsPas+AsAct*fpd/fyd)/(b0*d),0.02)
  return fact*chi*(rol*fck/1e4)^(1/3)*1e6-0.15*sgpcd


'''
Devuelve el valor de Vu2 (cortante de agotamiento por tracción en el alma)
   para piezas SIN reinforcement de cortante, según el artículo 44.2.3.2.1 de EHE.
fck: Resistencia característica del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
  
'''
def getVu2SIN(fck, Nd, Ac, b0, d, AsPas, fyd, AsAct, fpd):
  return getFcv(0.12,fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)*b0*d

'''
Devuelve el valor de Vcu (contribución del hormigón a la resistencia a cortante)
   para piezas CON reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
fck: Resistencia característica del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
theta: Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.
  
'''
def getVcu(fck, Nd, Ac, b0, d, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd):
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

'''
Devuelve el valor de Vsu (contribución de la reinforcement transversal de alma a la resistencia a cortante)
   para piezas CON reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
z: Brazo mecánico.
alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
theta: Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
AsTrsv: Área de la reinforcement transversal.
fyd: Resistencia de cálculo de la reinforcement transversal.
  
'''
def getVsu(z, alpha, theta, AsTrsv, fyd):
  ctgTheta= min(max(cotg(theta),0.5),2.0)
  fyalphad= min(fyd,400e6) # comentario al artículo 40.2 EHE
  return z*sin(alpha)*(cotg(alpha)+ctgTheta)*AsTrsv*fyalphad


'''
Devuelve el valor de Vu2 para piezas CON o SIN reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
fck: Resistencia característica del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
z: Brazo mecánico.
alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
theta: Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.
AsTrsv: Área de la reinforcement transversal.
fyd: Resistencia de cálculo de la reinforcement transversal.
  
'''
def getVu2(fck, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
  vcu= 0.0
  vsu= 0.0
  if(AsTrsv>0.0):
    vcu= getVcu(fck,Nd,Ac,b0,d,theta,AsPas,fyd,AsAct,fpd,sgxd,sgyd)
    vsu= getVsu(z,alpha,theta,AsTrsv,fydTrsv)
  else:
    vcu= getVu2SIN(fck,Nd,Ac,b0,d,AsPas,fyd,AsAct,fpd)
    vsu= 0.0
  return vcu+vsu

'''
Devuelve el valor de Vu= max(Vu1,Vu2) para piezas CON o SIN reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
fck: Resistencia característica del hormigón a compresión.
Nd: Axil de cálculo (positivo si es de tracción).
Ac: Area total de la sección de hormigón.
b0: Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
d: Canto útil expresado en metros.
z: Brazo mecánico.
alpha: Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
theta: Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
AsPas: Área de la de la reinforcement longitudinal pasiva traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fyd: Resistencia de cálculo de las reinforcement pasivas (artículo 38.3 EHE).
AsAct: Área de la de la reinforcement longitudinal activa traccionada anclada
a una distancia mayor que el depth útil de la sección en estudio.
fpd: Resistencia de cálculo del acero de las reinforcement activas (artículo 38.6 EHE).
sgxd: Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
sgyd: Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.
AsTrsv: Área de la reinforcement transversal.
fyd: Resistencia de cálculo de la reinforcement transversal.
  
'''
def getVu(fck, fcd, Nd, Ac, b0, d, z, alpha, theta, AsPas, fyd, AsAct, fpd, sgxd, sgyd, AsTrsv, fydTrsv):
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

'''
Devuelve el valor de Vu= max(Vu1,Vu2) para piezas CON o SIN reinforcement de cortante, según el artículo 44.2.3.2.2 de EHE.
fyd: Resistencia de cálculo de la reinforcement transversal.
  
'''

class ParamsCortante(object):
  '''Define las propiedades del registro que contiene los parámetros de cálculo
   a cortante.'''
  areaHormigon= 0.0 # Area total de la sección de hormigón.
  widthMin= 0.0 # Anchura neta mínima del elemento a comprobar, definida de acuerdo con el artículo 40.3.5.
  depthUtil= 0.0 # Canto útil expresado en metros.
  brazoMecanico= 0.0 # Brazo mecánico expresado en metros.
  areaRebarTracc= 0.0 # Área de la reinforcement longitudinal pasiva traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
  areaTendonesTracc= 0.0 # Área de la reinforcement activa longitudinal pasiva traccionada anclada a una distancia mayor que el depth útil de la sección en estudio.
  areaShReinfBranchsTrsv= 0.0 # Área de la reinforcement transversal.
  sigmaXD= 0.0 # Tensión normal de cálculo en el centro de gravedad de la sección paralela a la directriz de la pieza considerando el hormigón NO FISURADO (art. 44.2.3.2).
  sigmaYD= 0.0 # Tensión normal de cálculo en el centro de gravedad de la sección paralela al esfuerzo cortante.

  angAlpha= math.pi/2 # Ángulo de las reinforcement de cortante con el eje de la pieza (figura 44.2.3.1.a EHE).
  angTheta= math.pi/6 # Ángulo entre las bielas de compresión del hormigón y el eje de la pieza (figura 44.2.3.1.a EHE).
  cortanteUltimo= 0.0

  # Calcula el valor del cortante último de la sección.
  def calcCortanteUltimo(self, setFibrasHormigon, setFibrasArmadura,setFibrasArmaduraTraccion, fck, fcd, fyd, fpd, fydTrsv):
    self.areaHormigon= setFibrasHormigon.getArea()
    widthMin= setFibrasHormigon.getAnchoMecanico() # Mejorar (en secciones no convexas estará mal).
    self.depthUtil= setFibrasHormigon.getCantoUtil()
    self.brazoMecanico= setFibrasHormigon.getBrazoMecanico()
    self.areaRebarTracc= setFibrasArmaduraTraccion.getArea
    # areaTendonesTracc= 

    self.sigmaXD= N/area+Mz/Iz*yCdg+My/Iy*zCdg
    self.cortanteUltimo= getVu(fck,fcd,N,self.areaHormigon,self.widthMin,self.depthUtil,self.brazoMecanico,self.angAlpha,self.angTheta,self.areaRebarTracc,fyd,self.areaTendonesTracc,fpd,self.sigmaXD,self.sigmaYD,AsTrsv,self.areaShReinfBranchsTrsv,fydTrsv)

  # Imprime los parámetros de cortante de la sección.
  def printParams(self):
    print "Area de las reinforcement traccionadas; As= ",self.areaRebarTracc*1e4," cm2"
    print "Área de la reinforcement transversal; AsTrsv= ",self.areaShReinfBranchsTrsv*1e4," cm2"
    print "Tensión normal de cálculo = sigmaXD",self.sigmaXD/1e6," MPa"
    print "Canto útil; d= ",self.depthUtil," m"
    print "Ancho mínimo; b0= ",self.widthMin," m"
    print "Brazo mecánico; z= ",self.brazoMecanico," m"
    print "cortanteUltimo; Vu= ",self.cortanteUltimo/1e3," kN"

