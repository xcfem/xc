# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

'''
European I beams

WARNING Axis definition different from those of Eurocode 3:

   ARCELOR          XC

                    ^ Y                    
                    |

    -----         -----
      |             | 
      | -> Y        | -> Z
      |             |
    -----         -----

      |
      v Z

So:
   Z is the cross-section MAJOR axis in XC (minor in EC3).
   Y is the cross-section MINOR axis in XC (major in EC3).

'''

from materials.structural_shapes.arcelor import perfiles_ipe_arcelor as ipe
import EC3Profile as EC3prof

class EC3IPEProfile(EC3prof.EC3Profile,ipe.IPEProfile):
  """IPE profile with Eurocode 3 verification routines."""
  def __init__(self,steel,name):
    ''' Constructor.

    :param steel: steel material.
    :param name: shape name (i.e. IPE_600)
    '''
    EC3prof.EC3Profile.__init__(self,'rolled')
    ipe.IPEProfile.__init__(self,steel,name)
