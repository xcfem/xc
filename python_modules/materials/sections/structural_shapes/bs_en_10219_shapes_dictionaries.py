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

# Section axis: 

#  EN 10219           XC
#  

#       ^ Z           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----


# The axis used in the EN 10219 standard are different from those used
# in XC (strong axis parallel to z axis) in other words: values for Y
# and Z axis are swapped with respect to those in the catalog.

shapes= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "bs_en_10219_profiles.json") as file:
    shapes= json.load(file)
