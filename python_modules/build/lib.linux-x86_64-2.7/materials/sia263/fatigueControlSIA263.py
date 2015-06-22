# -*- coding: utf-8 -*-
'''
Fatique control SIA263:2003
'''

def lambdaMax(longDet):
  ''' returns lambdaMax value form figure 52 (F.2.2 SIA263:2003) '''
  if(longDet>=20):
    return 2.0
  else:
    return (2.0-2.5)/(20.0-5.0)*(longDet-5.0)+2.5
