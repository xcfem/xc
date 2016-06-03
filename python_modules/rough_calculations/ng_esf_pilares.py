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

  
def getAreaInfluenciaPilar(L1,L2,L3,L4):
    '''
    Área de influencia de un pilar a partir de las luces de los vanos anejos
    ver «Números gordos en el proyecto de estructuras» página 44
    '''
    return (L1+L2)/2*(L3+L4)/2
  
