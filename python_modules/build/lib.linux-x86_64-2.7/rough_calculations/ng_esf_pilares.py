# -*- coding: utf-8 -*-
from __future__ import division

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
  
