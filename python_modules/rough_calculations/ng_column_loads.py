# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

##      L1      L2
##   /-----/----------/
##      |       |
##   +  |  +    |     +    -
##      |       |          |
##-----------------------  | L3
##      |       |          |
##      |       |          |
##   +  |  +A   |     +    -
##      |       |          |
##      |       |          |
##------------------------ | L4
##      |       |          |
##      |       |          |
##      |       |          |
##   +  |  +    |     +    -

  
def getColumnTributaryArea(L1,L2,L3,L4):
    '''
    Tributary area of a column from the neighbour spans
    see «Números gordos en el proyecto de estructuras» page 44
    '''
    return (L1+L2)/2*(L3+L4)/2.0
  
