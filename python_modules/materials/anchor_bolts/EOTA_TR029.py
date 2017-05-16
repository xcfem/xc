# -*- coding: utf-8 -*-
''' Anchor design according to EOTA TR029'''

from materials.anchor_bolts import EOTA_TR029_tension

'''
************************************************************************
************************ Shear resistance ******************************
************************************************************************
'''

def shearResistanceWithoutLeverArm(As, fuk):
    '''
    Resistencia de un perno a cortante según el artículo 5.2.3.2 a) de EOTA TR029.

    :param As: Área de la sección transversal del perno.
    :param fuk: Valor característico de la carga de rotura a tracción.
    '''
    return 0.5*As*fuk

def shearResistanceGroupWithoutLeverArm(As, fuk, nr):
    '''
    Resistencia de un grupo de pernos a cortante según el artículo 5.2.3.2 a) de EOTA TR029.
     As: Área de la sección transversal del perno.
     fuk: Valor característico de la carga de rotura a tracción.
     n: Número de pernos del grupo.
    '''
    return 0.5*As*fuk*nr*0.8

def shearResistanceConcretePryOut(NRkp, NRkc, hEf):
    '''
    Resistencia de un perno a cortante por desprendimiento de cuña de hormigón
     según las expresiones 5.7 y 5.7a del artículo 5.2.3.3 of EOTA TR029.

     :param NRkp: Characteristic resistance of combined pull-out and
                  concrete cone failure (5.2.2.3 of EOTA TR029).
     :param NRkc: Characteristic resistance in case of concrete cone
                  failure (5.2.2.4 of EOTA TR029).
     :param hEf: Profundidad efectiva of the anchor (m).
    '''
    k= 1.0
    if(hEf>=60e-3): # 60mm
      k=2
    return k*min(NRkp,NRkc)

'''
 ************************************************************************
 *********** Shear resistance: concrete edge failure ********************
 ************************************************************************
'''

def psiReVFactor(descr):
  '''
  Coeficiente que introduce la influencia del tipo de refuerzo empleado en cracked concrete  en la expresión 5.8 de EOTA TR029, calculado según el apartado g) del artículo 5.2.3.4 de EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: Anclaje en cracked concrete o no fisurado sin reinforcement de refuerzo en el borde.
    2: Anclaje en cracked concrete con reinforcement de refuerzo en el borde (diámetro > 12mm).
    3: Anclaje en cracked concrete con reinforcement de refuerzo en el borde y estribos próximos entre si (separados menos de 10 cm).
  '''
  return ifte(descr<=1,1.0,ifte(descr<=2,1.2,1.4))

def psiEcVFactor(ev, c1):
  '''
  Coeficiente que introduce la influencia de la variación de los cortantes en los distintos pernos de un grupo, calculado según la expresión 5.8h del apartado f) del artículo 5.2.3.4 de EOTA TR029.

   :param ev: Excentricidad de la resultante de esfuerzos cortantes respecto al centro de gravedad del grupo.
   :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return min(1/(1+2*ev/3/c1),1.0)

def psiAlphaVFactor(alphaV):
  '''
  Coeficiente que introduce la influencia del ángulo entre la carga aplicada y la perpendicular al borde libre de la pieza de hormigón (menor de 90 grados) ver figura 4.7c y expresión 5.8g del apartado f) del artículo 5.2.3.4 de EOTA TR029.

  :param alphaV: Ángulo entre la carga aplicada y la perpendicular al borde libre de la pieza de hormigón.
  '''
  return max(sqrt(1.0/(cos(alphaV)^2+(sin(alphaV)/2.5)^2)),1.0)

def psiHVFactor(h, c1):
  '''
  Coeficiente que sirve para introducir en el cálculo la falta de proporcionalidad entre la disminución de la resistencia a cortante y la disminución del espesor de la pieza tal y como se asume en el cociente Ac,v/Ac,v0. ver expresión 5.8f del artículo 5.2.3.4 de EOTA TR029.
  :param h: Espesor de la pieza de hormigón.
  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return max(sqrt(1.5*c1/h),1.0)

def psiSVFactor(c1, c2):
  '''
  Coeficiente que sirve para introducir en el cálculo la perturbación en la distribución de tensiones en el hormigón que produce la presencia de un segundo borde libre próximo al anclaje. Ver expresión 5.8e del artículo 5.2.3.4 de EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param c2: Distancia desde el centro de gravedad del grupo al otro borde libre.
  '''
  return min((0.7+0.2*c2/c1),1.0)

def areaAcV0(h, c1):
  '''
  Area del cono de anclaje a cortante para un perno. Ver figura 5.6 y expresión 5.8d de EOTA TR029.

  :param h: Espesor de la pieza de hormigón.
  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  '''
  return 3*c1*min(1.5*c1,h)

def AcV2Pernos(h, c1, s2):
  '''
  Area del cono de anclaje para dos pernos. Ver figura 5.7 b de EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param h: Espesor de la pieza de hormigón.
  :param s2: Separación entre los pernos.
  '''
  return (3*c1+min(s2,3*c1))*min(1.5*c1,h)

def AcV2PernosC2(h, c1, c2, s2):
  '''
  Area del cono de anclaje para dos pernos junto a un borde libre. Ver figura 5.7 b y c de EOTA TR029.

  :param  c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param c2: Distancia desde el centro de gravedad del grupo al otro borde.
  :param h: Espesor de la pieza de hormigón.
  :param s2: Separación entre los pernos.
  '''
  return (1.5*c1+c2+min(s2,3*c1))*min(1.5*c1,h)

def k1Expr58A(descr):
  '''
  Coeficiente que introduce la influencia de la fisuración del hormigón en la expresión 5.8a de EOTA TR029.

  :param descr: Descriptor que puede tomar los valores:
    1: Anclaje en cracked concrete.
    2: Anclaje en non-cracked concrete.
  '''
  return ifte(descr<=1,1.7,2.4)

def VRkC0Expr58(k1, d, hEf, fckCube, c1):
  '''
  Valor inicial de la resistencia característica of the anchor a cortante por fallo del borde según la expresión 5.8a del apartado a del artículo 5.2.3.4 de EOTA TR029.

  :param c1: Distancia desde el centro de gravedad del grupo al borde situado frente al cortante.
  :param d: Diámetro del perno.
  :param hEf: Profundidad efectiva of the anchor.
  '''
  alpha= tonum(0.1*(hEf/c1)^1.5)
  beta= tonum(0.1*(d/c1)^0.2)
  return k1*pow(d*1000,alpha)*pow(hEf*1000,beta)*sqrt(fckCube/1e6)*pow(c1*1000,1.5)

