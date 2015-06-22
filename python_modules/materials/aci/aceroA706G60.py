# -*- coding: utf-8 -*-

from materials import reinforcingSteel


# Diagrama tensión deformación del acero de armar según EHE.
def sigmas(eps, fy, ey, Es, Esh):
  if(eps>0):
    if(eps<ey):
      return (Es*eps)
    else:
      return (fy+(eps-ey)*Esh) 
  else:
    if(eps>-(ey)):
      return (Es*eps) 
    else:
      return (-fy+(eps-ey)*Esh) 

# Diagrama característico tensión deformación del acero de armar según EHE.
def sigmaKAceroArmar(eps, mat):
  return sigmas(eps,mat.fyk,mat.eyk,mat.Es,mat.Esh)

# Diagrama de cálculo tensión deformación del acero de armar según EHE.
def sigmaDAceroArmar(eps, mat):
  return sigmas(eps,mat.fyd,mat.eyd,mat.Es,mat.Esh)

# A706G60= 
# \A706G60
#   {
#     fyk= 420e6
#     emax= 0.08 #Deformación máxima
  


