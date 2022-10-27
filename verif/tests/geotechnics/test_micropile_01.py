# -*- coding: utf-8 -*-
''' Micropile test adapted from the web page:

   https://calculocivil.com/es/estructura/micropilote/calc
   Micropilotes en cimientos. Guía para el proyecto y la ejecución de micropilotes en obras de carreteras. Ministerio de Fomento. 2009. Spain.
'''
__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2022, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
from materials.ec3 import EC3_materials
from materials.api import API_materials
from materials.ehe import EHE_materials
from geotechnics.foundations import pile
from geotechnics.foundations import guia_micropilotes_oc as guia

# Material
## Steel pipe.
pipeSteel= API_materials.M80
gammaM0= 1.10
pipeSteel.gammaM= gammaM0
pipeShape= EC3_materials.MicropileTubeShape(steel= pipeSteel, name= 'MP_88.9x7.5')
## Mortar
mortar= EHE_materials.HA25
## Rebar
rebar= EHE_materials.rebar32_B500S

# Micropile
# soilAggressivity: soil aggresivity descriptor
#                   ('unalteredNatural', 'pollutedNatural', 'aggresiveNatural',
#                    'backfill', 'aggresiveBackfill') according to table 2.4.
soilAggressivity= 'unalteredNatural'
# designLife: design service life of the micropile (in years).
designLife= 50
# Fe: influence of the execution technique taken from table 3.5.
Fe= 1.5
# Fuc: reduction factor of the cross-section area due to the  type of the
#      union taken from table 3.4.
Fuc= 0.5
micropile= guia.Micropile(pileSet= None, pileDiam= 152.4e-3, soilLayers= pile.SoilLayers(), mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= rebar, pileType= 'endBearing', soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

# Axial strength
## C_R: adimensional coefficient from the table 3.6 of the "Guía".
## Fe: influence of the execution technique taken from table 3.5 of the "Guía".
NcRd= micropile.getNcRd(C_R= 10)
refNcRd= 402.94225618128195e3
ratio1= abs(NcRd-refNcRd)/refNcRd

'''
print('Nc,Rd= ', NcRd/1e3,'kN')
print('ratio1= ', ratio1)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-5:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
