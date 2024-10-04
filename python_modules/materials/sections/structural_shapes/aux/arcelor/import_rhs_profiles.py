# -*- coding: utf-8 -*-
''' Import SHS shape data from Arcelor catalog and decorate them with some auxiliary data.

'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2024, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import json

# Load raw catalog data.
shapesDict= None
with open("arcelor_raw_rhs_profiles.json") as f:
    shapesDict= json.load(f)

# Compute auxiliary data for CFRHS shapes.
for item in shapesDict:
    shape= shapesDict[item]
    A= shape['A']
    E= shape['E']
    nu= shape['nu']
    b= shape['b']
    h= shape['h']
    e= shape['e']
    shape['alpha']= 1.0 # 0.5*5/6.0
    shape['G']= E/(2*(1+nu))
    shape['Avy']= 2*0.7*h*e
    shape['Avz']= 2*0.7*b*e
    shape['Wyel']= shape['Wzel']
    shape['Wypl']= shape['Wzpl']

fNameOut= '../arcelor_rhs_profiles.json'
jsonFile= open(fNameOut, "w")
jsonFile.write(json.dumps(shapesDict))
jsonFile.close()
