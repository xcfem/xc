# -*- coding: utf-8 -*-
# Comprobación de una sección de hormigón frente a tensiones normales.

# Comprobación en flexotracción
def cumpleFlexotraccion(epsCMin, epsSMax):
  retval= False
  if(epsCMin>=-3.5e-3): # Minimum concrete strain OK.
    if(epsSMax<=10e-3): # Maximum reinforcing steel strain OK.
      retval= True
  return retval

# Aprovechamiento sección en flexotracción
def aprovFlexotraccion(epsCMin,epsSMax):
  return (max(epsCMin/-3.5e-3,epsSMax/10e-3))

# Comprobación en compresión simple o compuesta
def cumpleCompresionCompuesta(epsCMin,epsCMax):
  retval= False
  if(epsCMin>=(-3.5e-3 - 3/4*epsCMax)): # Si cumple deformación mínima del hormigón.
    if(epsCMax>=-2e-3): # Si cumple deformación máxima del hormigón.
      retval= True
  return retval

# Aprovechamiento sección en compresión simple o compuesta
def aprovCompresionCompuesta(epsCMin,epsCMax):
  return (max(epsCMin/(-3.5e-3 - 3/4*epsCMax),epsCMax/-2e-3))


# Comprobación en tracción simple o compuesta
def cumpleTraccionCompuesta(epsSMax):
  retval= False
  if(epsSMax<=10e-3): # Maximum reinforcing steel strain OK.
    retval= True
  return retval

# Aprovechamiento sección en tracción simple o compuesta.
def aprovTraccionCompuesta(epsSMax):
  return (epsSMax/10e-3)

def aprovTN(tipoSol, epsCMin, epsCMax, epsSMax):
  retval= 0.0
  if(tipoSol==1): # Tracción simple o compuesta.
    retval= aprovTraccionCompuesta(epsSMax)
  else:
    if(tipoSol==2): # Flexotracción.
      retval= aprovFlexotraccion(epsCMin,epsSMax)
    else:
      if(tipoSol==3): # Compresión simple o compuesta.
        retval= aprovCompresionCompuesta(epsCMin,epsCMax)
      else:
        retval= -100
  return retval

