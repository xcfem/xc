# -*- coding: utf-8 -*-
# Características de los aceros según el eurocódigo 3

'''
Devuelve el límite elástico del acero en función de su designación
   y del espesor de la pieza (ver tabla 3.1).
'''
def fyEC3(desig, t):
  retval= 0.0
  if(t>0.1):
    lmsg.error("Espesor de la pieza fuera de rango: ",t*1000," mm\n")

  if(desig == 235):
    retval= ifte(t<40e-3,235e6,215e6)
  elif(desig == 275):
    retval= ifte(t<40e-3,275e6,255e6)
  elif(desig == 355):
    retval= ifte(t<40e-3,355e6,335e6)
  else:
    lmsg.error("No se conoce la designación del acero: ",desig,"\n")
  return retval

'''
Devuelve la resistencia última a tracción del acero en función de su designación
   y del espesor de la pieza (ver tabla 3.1).
'''
def fuEC3(desig, t):
  retval= 0.0
  if(t>0.1):
    lmsg.error("Espesor de la pieza fuera de rango: ",t*1000," mm\n")

  if(desig == 235):
    retval= ifte(t<40e-3,360e6,340e6)
  elif(desig == 275):
    retval= ifte(t<40e-3,430e6,410e6)
  elif(desig == 355):
    retval= ifte(t<40e-3,510e6,490e6)
  else:
    lmsg.error("No se conoce la designación del acero: ",desig,"\n")
  return retval
