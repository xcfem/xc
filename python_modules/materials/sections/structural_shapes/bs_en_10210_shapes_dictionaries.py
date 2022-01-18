# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import importlib.resources
import json

bs_en_10210_shapes= dict()

# Data obtained from: https://www.steelforlifebluebook.co.uk/hfshs/ec3-ukna/section-properties-dimensions-properties/

# Section axis: 

#    SCI              XC
#  Blue book

#       ^ Z           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----


# The axis used in the Blue Book documentation are different from those used
# in XC (strong axis parallel to z axis) in other words: values for Y
# and Z axis are swapped with respect to those in the catalog.

HFSHS= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "bs_en_10210_hfshs_profiles.json") as file:
    HFSHS= json.load(file)
bs_en_10210_shapes['HFSHS']= HFSHS
