# -*- coding: utf-8 -*-
''' Slipstream effect according to EC1-2 clause 6.6.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es"


from actions.railway_traffic import ec1_slipstream_effect

# The expressions are exactly the same of the EC1-2:

q1k= ec1_slipstream_effect.q1k
q2k= ec1_slipstream_effect.q2k
q3k= ec1_slipstream_effect.q3k
