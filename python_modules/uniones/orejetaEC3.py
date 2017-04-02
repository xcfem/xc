# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def getAMinOrejetaEC3(F, gammaM0, t, d0, fy):
  '''
  Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el valor mínimo de "a" (anchura del acero frente al
   ojal, ver figura en EC3)
   F: Tracción en la orejeta.
   gammaM0: Coeficiente de minoración del acero.
   t: Espesor de la orejeta.
   d0: Diámetro del agujero.
   fy: Límite elástico del acero
  
  '''
  return F*gammaM0/(2*t*fy)+2*d0/3


def getCMinOrejetaEC3(F, gammaM0, t, d0, fy):
  '''
  Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el valor mínimo de "c" (anchura del acero a los 
   lados del ojal, ver figura en EC3)
   F: Tracción en la orejeta.
   gammaM0: Coeficiente de minoración del acero.
   t: Espesor de la orejeta.
   d0: Diámetro del agujero.
   fy: Límite elástico del acero
  
  '''
  return F*gammaM0/(2*t*fy)+d0/3


def getResistCortanteBulonEC3(A, gammaM2, fup):
  '''
  Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el valor de la resistencia a cortante del bulón
   A: Área del bulón.
   gammaM2: Coeficiente de minoración del acero.
   fup: Resistencia última a tracción del acero.
  
  '''
  return 0.6*A*fup/gammaM2

def getFAplastamientoOjalEC3(t, d, fy, gammaM0):
  '''
  Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve la carga para la que se produce el aplastamiento
   del ojal.
   Se supone que el bulón no es reemplazable. (ver tabla 3.10, segundo 
   modo de fallo)
   t: Espesor de la chapa que tiene el ojal.
   d: Diámetro del taladro.
   fy: Límite elástico del material.
   gammaM0: Coeficiente de minoración del acero.
  
  '''
  return 1.5*t*d*fy/gammaM0

def getMSolicBulonEC3(F, b, a, c):
  '''
  Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el momento máximo que solicita al bulón a partir de las
   dimensiones de la pieza en horquilla y la pieza en orejeta. (ver figura 3.11)
   F: Valor de cálculo de la fuerza que solicita la unión.
   b: Espesor de la chapa de la orejeta.
   a: Espesor de las chapas de las horquillas.
   c: Huelgo entre cada lado de la horguilla y la orejeta.
  
  '''
  return F/8*(b+4*c+2*a)

def getMResistBulonEC3(W, fy, gammaM0):
  '''
   Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el momento máximo que puede resistir el bulón.
   (ver tabla 3.10, tercer modo de fallo)
   W: Modulo resistente (elástico) de la sección.
   fy: Límite elástico del material.
   gammaM0: Coeficiente de minoración del acero.
  
  '''
  return 1.5*W*fy/gammaM0

def getFactorCapacidadBulonEC3(M, MR, V, VR):
  '''
   Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   This function returns a value lesser than 1 if the bolt can
   resist the combination of bending moment and shear force.
   (see table 3.10, fourth failure mode)
   M: Bending moment.
   MR: Resisting bending moment.
   V: Shear force.
   VR: Resisting shear force.
  '''
  return sqr(M/MR)+sqr(V/VR)



