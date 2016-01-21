# -*- coding: utf-8 -*-
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
#ng_simple_bending_reinforcement

import math
import logging

logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

def neutralFiberDepth(M,fcd,b,d):
  c= 0.85*fcd*b
  T= c*(d-math.sqrt(d**2-2*M/c))
  xpl= T/c
  assert xpl<=d, "xpl bigger than section depth."
  return xpl

def Mu(As,fcd,fsd,b,d):
  T= As*fsd
  z= 0.9*d
  c= 0.85*fcd*b
  xpl= T/c
  assert xpl<=d, "xpl bigger than section depth."
  return T*z

def Mlim(fcd,b,d):
  return 0.33*fcd*b*d**2 

def AsSimpleBending(M,fcd,fsd,b,d):
  Ml= Mlim(fcd,b,d)
  if(M>Ml):
    logging.warning('compression reinforcement needed Ml= '+ str(Ml/1e3) + ' kN m < '+ str(M/1e3)+ ' kN m') 
    T= 1e9
  else:
    c= 0.85*fcd*b
    T= c*(d-math.sqrt(d**2-2*M/c))
    xpl= T/c
    assert xpl<=d, "xpl bigger than section depth."
  return T/fsd
