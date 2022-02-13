# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2022, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import importlib.resources
import json
from materials.awc_nds import AWCNDS_materials

# Data obtained from: https://apawood-europe.org/wp-content/uploads/2020/05/Z725-I-joists.pdf

jsonDict= None
with importlib.resources.open_text("materials.awc_nds.aux", "pr-400_i-joists.json") as file:
    jsonDict= json.load(file)

pr400_i_joists= dict()

for key in jsonDict:
    sectionData= jsonDict[key]
    h= sectionData['h']
    Mr= sectionData['Mr']
    Vr= sectionData['Vr']
    pr400_i_joists[key]= AWCNDS_materials.TJIJoistSection(name= key, h= h, Ms= Mr, Vs= Vr)
