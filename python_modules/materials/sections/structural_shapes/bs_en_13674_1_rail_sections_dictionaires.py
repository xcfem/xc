# -*- coding: utf-8 -*-
''' EN 13674-1 Flat bottom rails >46kg/m.'''


__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "

import importlib.resources
import json

# Section axis: 

#    British              XC
#  Steel catalog

#       ^ Y           ^ Y                    
#       |             |

#     -----         -----
#       |             | 
#       | -> X        | -> Z
#       |             |
#     -----         -----

# The axis used in the British Steel catallog are different from those used
# in XC (strong axis parallel to z axis) in other words: values for X
# and Z axis are swapped with respect to those in the catalog.

shapes= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "bs_en_13674-1_rail_sections.json") as file:
    shapes= {'RAIL':json.load(file)}
