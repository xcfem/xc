# -*- coding: utf-8 -*-
''' Classes and functions for limit state control according to SIA 263.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

'''
Fatigue control SIA263:2003
'''
def lambdaMax(longDet):
  ''' returns lambdaMax value form figure 52 (F.2.2 SIA263:2003) '''
  if(longDet>=20):
    return 2.0
  else:
    return (2.0-2.5)/(20.0-5.0)*(longDet-5.0)+2.5
