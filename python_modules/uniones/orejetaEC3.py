# -*- coding: utf-8 -*-

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
def getAMinOrejetaEC3(F, gammaM0, t, d0, fy):
  return F*gammaM0/(2*t*fy)+2*d0/3


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
def getCMinOrejetaEC3(F, gammaM0, t, d0, fy):
  return F*gammaM0/(2*t*fy)+d0/3


'''
Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el valor de la resistencia a cortante del bulón
   A: Área del bulón.
   gammaM2: Coeficiente de minoración del acero.
   fup: Resistencia última a tracción del acero.
  
'''
def getResistCortanteBulonEC3(A, gammaM2, fup):
  return 0.6*A*fup/gammaM2

'''
Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve la carga para la que se produce el aplastamiento del ojal.
   Se supone que el bulón no es reemplazable. (ver tabla 3.10, segundo modo de fallo)
   t: Espesor de la chapa que tiene el ojal.
   d: Diámetro del taladro.
   fy: Límite elástico del material.
   gammaM0: Coeficiente de minoración del acero.
  
'''
def getFAplastamientoOjalEC3(t, d, fy, gammaM0):
    return 1.5*t*d*fy/gammaM0

'''
Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el momento máximo que solicita al bulón a partir de las
   dimensiones de la pieza en horquilla y la pieza en orejeta. (ver figura 3.11)
   F: Valor de cálculo de la fuerza que solicita la unión.
   b: Espesor de la chapa de la orejeta.
   a: Espesor de las chapas de las horquillas.
   c: Huelgo entre cada lado de la horguilla y la orejeta.
  
'''
def getMSolicBulonEC3(F, b, a, c):
    return F/8*(b+4*c+2*a)

'''
Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el momento máximo que puede resistir el bulón.
   (ver tabla 3.10, tercer modo de fallo)
   W: Modulo resistente (elástico) de la sección.
   fy: Límite elástico del material.
   gammaM0: Coeficiente de minoración del acero.
  
'''
def getMResistBulonEC3(W, fy, gammaM0):
    return 1.5*W*fy/gammaM0

'''
Dimensionamiento de una orejeta según el apartado 3.13 del EC3 parte 1_8
   Esta función devuelve el un valor menor que 1 si la combinación de 
   flector y cortante en el bulón puede ser resistida por el mismo.
   (ver tabla 3.10, cuarto modo de fallo)
   M: Momento que solicita al bulón.
   MR: Momento que puede resistir el bulón.
   V: Cortante que actúa sobre el bulón.
   VR: Cortante que puede resistir el bulon.
  
'''
def getFactorCapacidadBulonEC3(M, MR, V, VR):
    return sqr(M/MR)+sqr(V/VR)



