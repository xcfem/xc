# -*- coding: utf-8 -*-
''' EN 1337-3:2005 (E) Table 3 — Standard sizes for bearings type B.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2025, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import importlib.resources
import json

en_1337_bearings= dict()

round_bearings= None
with importlib.resources.open_text("materials.en_1337.aux", "table_3_en_1337-3_2005_round_elastomeric_bearings.json") as file:
    round_bearings= json.load(file)
en_1337_bearings['round_bearings']= round_bearings

rectangular_bearings= None
with importlib.resources.open_text("materials.en_1337.aux", "table_3_en_1337-3_2005_rectangular_elastomeric_bearings.json") as file:
    rectangular_bearings= json.load(file)
en_1337_bearings['rectangular_bearings']= rectangular_bearings
