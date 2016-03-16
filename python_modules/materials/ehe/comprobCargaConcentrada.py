# -*- coding: utf-8 -*-
'''
Comprobación de cargas concentradas sobre macizos, según el artículo
61 de la EHE-08
'''

# Devuelve el valor de f3cd
def getF3cd(Ac, Ac1, fcd):
  return min(sqrt(Ac/Ac1),3.3)*fcd

'''
Devuelve el valor último de la carga que puede soportar una pieza
de seccion Ac siendo el área cargada Ac1 (ver figura 61.1.a página 302 EHE-08)
'''
def getNuCargaConcentrada(Ac, Ac1, fcd):
  return Ac1*getF3cd(Ac,Ac1,fcd)

'''
Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
lado a (ver figura 61.1.a página 302 EHE-08)
  a: Dimensión de la sección.
  a1: Dimensión del área cargada paralela a a.
  Nd: Carga concentrada.
  
'''
def getUad(a, a1, Nd):
  return 0.25*((a-a1)/a)*Nd

'''
Devuelve el área necesaria para la reinforcement paralela al
lado a (ver figura 61.1.a página 302 EHE-08)
  a: Dimensión de la sección.
  a1: Dimensión del área cargada paralela a a.
  Nd: Carga concentrada.
  fyd: Límite elástico del acero.
  
'''
def getAreaArmadAd(a, a1, Nd, fyd):
  return getUad(a,a1,Nd)/min(fyd,400e6)


'''
Devuelve la capacidad mecánica necesaria para la reinforcement paralela al
lado b (ver figura 61.1.a página 302 EHE-08)
  b: Dimensión de la sección.
  b1: Dimensión del área cargada paralela a b.
  Nd: Carga concentrada.
  
'''
def getUbd(b, b1, Nd):
  return 0.25*((b-b1)/b)*Nd

'''
Devuelve el área necesaria para la reinforcement paralela al
lado b (ver figura 61.1.a página 302 EHE-08)
  b: Dimensión de la sección.
  b1: Dimensión del área cargada paralela a a.
  Nd: Carga concentrada.
  fyd: Límite elástico del acero.
  
'''
def getAreaArmadBd(b, b1, Nd, fyd):
  return getUad(b,b1,Nd)/min(fyd,400e6)
