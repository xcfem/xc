# -*- coding: utf-8 -*-

import scipy.interpolate

# Returns adherence stress (Pa) for concrete type (tableau 19 SIA 262).
x= [12e6 , 16e6,20e6 ,25e6 ,30e6 ,35e6 ,40e6 ,45e6 ,50e6]
y= [1.5e6,1.8e6,2.1e6,2.4e6,2.7e6,3.0e6,3.3e6,3.6e6,3.8e6]
adherenceStress= scipy.interpolate.interp1d(x,y)

def getBasicAnchorageLength(phi,fck, fsd):
  fbd= adherenceStress(fck)
  """Returns anchorage length."""
  return max(fsd/fbd/4.0,25.0)*phi
     
