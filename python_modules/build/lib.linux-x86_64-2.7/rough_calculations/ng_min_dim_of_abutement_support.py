#----------------------------------------------------------------------------
#  programa XC; cálculo mediante el método de los elementos finitos orientado
#  a la solución de problemas estructurales.
#
#  Copyright (C)  Luis Claudio Pérez Tato
#
#  El programa deriva del denominado OpenSees <http:#opensees.berkeley.edu>
#  desarrollado por el «Pacific earthquake engineering research center».
#
#  Salvo las restricciones que puedan derivarse del copyright del
#  programa original (ver archivo copyright_opensees.txt) este
#  software es libre: usted puede redistribuirlo y/o modificarlo 
#  bajo los términos de la Licencia Pública General GNU publicada 
#  por la Fundación para el Software Libre, ya sea la versión 3 
#  de la Licencia, o (a su elección) cualquier versión posterior.
#
#  Este software se distribuye con la esperanza de que sea útil, pero 
#  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
#  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
#  Consulte los detalles de la Licencia Pública General GNU para obtener 
#  una información más detallada. 
#
# Debería haber recibido una copia de la Licencia Pública General GNU 
# junto a este programa. 
# En caso contrario, consulte <http:#www.gnu.org/licenses/>.
#----------------------------------------------------------------------------
#ng_min_dim_of_abutment_support.py

import sys

def getLg(soilClass):
  '''
  From a length greater than de distance "lg" the soil mouvement
  can bi consideread as completely uncorrelated.
  '''
  retval= 300
  if(soilClass == "A"):
    retval= 600
  elif(soilClass == "B"):
    retval= 500
  elif(soilClass == "C"):
    retval= 400
  elif(soilClass == "D"):
    retval= 300
  elif(soilClass == "E"):
    retval= 500
  else:
    sys.stderr.write("Unknown soil type: "+soilClass)
  return retval;

def getUgd(soilClass, quakeZone,bridgeClass):
  '''
  Returns the design value for soil displacement.
  soilClass: A, B, C, D or E.
  quakeZone: ZI, ZII, ZIIa, ZIIIb
  bridgeClass: COI, COII, COIII
  '''
  retval= 17e-2
  if(soilClass == "A"):
    if(quakeZone == "ZI"):
      retval= 2e-2
    elif(quakeZone == "Z2"):
      retval= 4e-2
    elif(quakeZone == "Z3a"):
      retval= 5e-2
    elif(quakeZone == "Z3b"):
      retval= 6e-2
    else:
      sys.stderr.write("Unknown quake zone: "+quakeZone)
  elif(soilClass == "B"):
    if(quakeZone == "ZI"):
      retval= 4e-2
    elif(quakeZone == "Z2"):
      retval= 6e-2
    elif(quakeZone == "Z3a"):
      retval= 8e-2
    elif(quakeZone == "Z3b"):
      retval= 10e-2
    else:
      sys.stderr.write("Unknown quake zone: "+quakeZone)
  elif(soilClass == "C"):
    if(quakeZone == "ZI"):
      retval= 5e-2
    elif(quakeZone == "Z2"):
      retval= 7e-2
    elif(quakeZone == "Z3a"):
      retval= 9e-2
    elif(quakeZone == "Z3b"):
      retval= 11e-2
    else:
      sys.stderr.write("Unknown quake zone: "+quakeZone)
  elif(soilClass == "D"):
    if(quakeZone == "ZI"):
      retval= 6e-2
    elif(quakeZone == "Z2"):
      retval= 11e-2
    elif(quakeZone == "Z3a"):
      retval= 14e-2
    elif(quakeZone == "Z3b"):
      retval= 17e-2
    else:
      sys.stderr.write("Unknown quake zone: "+quakeZone)
  elif(soilClass == "E"):
    if(quakeZone == "ZI"):
      retval= 4e-2
    elif(quakeZone == "Z2"):
      retval= 7e-2
    elif(quakeZone == "Z3a"):
      retval= 9e-2
    elif(quakeZone == "Z3b"):
      retval= 11e-2
    else:
      sys.stderr.write("Unknown quake zone: "+quakeZone)
  else:
    sys.stderr.write("Unknown soil type: "+soilClass)
  if(bridgeClass == "COII"):
    retval*=1.2
  elif(bridgeClass == "COIII"):
    retval*=1.4
  return retval;
  
def getBminPontFlotant(dAbutFixedPoint,soilClass,quakeZone,bridgeClass):
  '''
  Returns the minimal dimension of abutment support to avoid
  the risk of bridge deck falling during a quake. See "Évaluation
  parasismique des ponts-routes existants" Office féderal des routes
  page 48).
  dAbutFixedPoint: Distance between the abutment and the fixed point.
  soilClass: A, B, C, D or E.
  quakeZone: ZI, ZII, ZIIa, ZIIIb
  bridgeClass: COI, COII, COIII
  '''
  lg= getLg(soilClass)
  ugd= getUgd(soilClass, quakeZone,bridgeClass)
  return 0.2+min((1.3+2*dAbutFixedPoint/lg),3.3)*ugd
  
