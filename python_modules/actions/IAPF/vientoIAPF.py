# -*- coding: utf-8 -*-
# Macros para facilitar el cálculo de la acción del viento según la IAPF

'''
Parámetro k en función del grado de aspereza del entorno
   según IAPF (cuadro 2.4).
'''
def ParamKIAPF(gae):
  if(gae==0):
    return 0.16 
  elif(gae==1):
    return 0.17 
  elif(gae==2):
    return 0.19 
  elif(gae==3):
    return 0.22 
  elif(gae==4):
    return 0.23 
  else:
    print "Error en el grado de aspereza del entorno"
    return 0.0

'''
Altura mínima z_min en función del grado de aspereza del entorno
   según IAPF (cuadro 2.4).
'''
def ZminIAPF(gae):
  if(gae==0):
    return 1 
  elif(gae==1):
    return 1 
  elif(gae==2):
    return 2 
  elif(gae==3):
    return 5 
  elif(gae==4):
    return 10 
  else:
    print "Error en el grado de aspereza del entorno"
    return 0.0

'''
Longitud de la rugosidad z_0 en función del grado de aspereza del entorno
   según IAPF (cuadro 2.4).
'''
def Z0IAPF(gae):
  if(gae==0):
    return 0.003 
  elif(gae==1):
    return 0.01 
  elif(gae==2):
    return 0.05 
  elif(gae==3):
    return 0.3 
  elif(gae==4):
    return 1 
  else:
    print "Error en el grado de aspereza del entorno"
    return 0.0

'''
Valor del alpha en función del grado de aspereza del entorno
   según IAPF (cuadro 2.4).
'''
def alphaIAPF(gae):
  if(gae==0):
    return 0.38 
  elif(gae==1):
    return 0.44 
  elif(gae==2):
    return 0.52 
  elif(gae==3):
    return 0.61 
  elif(gae==4):
    return 0.67 
  else:
    print "Error en el grado de aspereza del entorno"
  return 0.0

# Factor de altura según apartado 2.3.7.2 IAPF.
def CzIAPF(gae, z):
  kzCzIAPF= ParamKIAPF(gae)
  z0CzIAPF= Z0IAPF(gae)
  zminCzIAPF= ZminIAPF(gae)
  if(z>zminCzIAPF):
    return kzCzIAPF*log(z/z0CzIAPF) 
  else:
    return kzCzIAPF*log(zminCzIAPF/z0CzIAPF) 

# Factor de ráfaga según apartado 2.3.7.2 IAPF.
def CgIAPF(kz, Cz, Ct):
  return sqrt(1+(7*kz/Cz/Ct))

'''
Coeficiente de arrastre para empuje horizontal sobre tableros
de alma llena según apartado 2.3.7.5.1 IAPF.
'''
def CdTableroAlmaLlenaIAPF(B, h, angAlma):
  coefRedCdIAPF= max(0.7,1-(5/1000*rad2deg(angAlma)))
  tmpCdIAPF= 2.5-0.3*B/h
  tmpCdIAPF= max(tmpCdIAPF,0.3)
  tmpCdIAPF= min(tmpCdIAPF,2.4)*coefReductor
  return tmpCdIAPF

'''
Empuje horizontal por unidad de longitud sobre el tablero
de alma llena según apartado 2.3.7.3 IAPF.
'''
def FHkPorMetroIAPF(Cd, h, Vc):
  rhoAire= 1.25
  return Cd*h*0.5*rhoAire*sqr(Vc)

'''
Empuje vertical por unidad de longitud sobre el tablero
de alma llena según apartado 2.3.7.5.2 IAPF.
'''
def FVkPorMetroIAPF(b, Vc):
  rhoAire= 1.25
  return 0.5*b*0.5*rhoAire*sqr(Vc)

