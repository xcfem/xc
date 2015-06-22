# -*- coding: utf-8 -*-

'''
Devuelve:
    1: Si la sección está en tracción simple o compuesta (todas las fibras traccionadas).
    2: Si la sección está en flexotracción (fibras traccionadas y comprimidas).
    3: Si la sección está en compresión simple o compuesta (todas las fibras comprimidas).

    epsCMin: Deformación mínima del hormigón.
    epsCMax: Deformación máxima del acero.
'''
def tipoSolicitacion(epsCMin, epsSMax):
  if(epsCMin>0.0): # Tracción simple o compuesta.
    return 1 
  else: # Hay hormigón comprimido.
    if(epsSMax>0): # Hay acero traccionado.
      return 2  # Flexotracción
    else:
      return 3  # Compresión simple o compuesta.

def strTipoSolicitacion(tipoSol):
  if(tipoSol==1): # Tracción simple o compuesta.
    return "tracción simple o compuesta"
  else:
    if(tipoSol==2): # Flexotracción.
      return "flexotracción" 
    else:
      if(tipoSol==3): # Compresión simple o compuesta.
        return "compresión simple o compuesta"
      else: 
        return "falla"
