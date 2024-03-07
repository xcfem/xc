# -*- coding: utf-8 -*-
''' Micropile cross sectin strength test. '''
from __future__ import print_function
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials.ec3 import EC3_materials
from materials.api import API_materials
from materials.ehe import EHE_materials
from geotechnics.foundations import guia_micropilotes_oc as guia

#Materials
## Steel material
steel= API_materials.M80
steel.gammaM= 1.10
## Profile geometry
pipeShape= EC3_materials.MicropileTubeShape(steel= steel, name= 'MP_101.6x9')
## Mortar
mortar= EHE_materials.HA25
## Rebar
rebar= None

# Micropile
# soilAggressivity: soil aggresivity descriptor
#                   ('unalteredNatural', 'pollutedNatural', 'aggresiveNatural',
#                    'backfill', 'aggresiveBackfill') according to table 2.4.
soilAggressivity= 'unalteredNatural'
# designLife: design service life of the micropile (in years).
designLife= 50
# Fe: influence of the execution technique taken from table 3.5.
Fe= 1.0
# Fuc: reduction factor of the cross-section area due to the  type of the
#      union taken from table 3.4.
Fuc= 1.0
micropileSection= guia.MicropileSection(diameter= 185e-3, mortarMaterial= mortar, pipeSection= pipeShape, axialRebar= None, soilAggressivity= soilAggressivity, designLife= designLife, Fe= Fe, Fuc= Fuc)

# Check results
## Axial force.
Nd= -53.3e3
NcRd= micropileSection.getNcRd(C_R= 12.0)
NcRdRef= 819e3
ratio1= abs(NcRd-NcRdRef)/NcRdRef

## Bending moment.
Md= 16.8e3
McRd= micropileSection.getMcRd(Fuf= 0.8)
McRdRef= 29e3
ratio2= abs(McRd-McRdRef)/McRdRef

## Shear force.
Vd= 51e3
VcRd= micropileSection.getVcRd()
VcRdRef= 454.29e3
ratio3= abs(VcRd-VcRdRef)/VcRdRef


resCheckBending= micropileSection.getBendingEfficiency(Nd= Nd, Md= Md, Vd= Vd, C_R= 12.0, Fuf= 0.8)
bendingEfficiency= resCheckBending
bendingEfficiencyRef= -Nd/NcRdRef+Md/McRdRef+Vd/VcRdRef
ratio4= abs(bendingEfficiency-bendingEfficiencyRef)/bendingEfficiencyRef

'''
print('NcRd= ', NcRd/1e3, 'kN', ratio1)
print('McRd= ', McRd/1e3, 'kN m', ratio2)
print('VcRd= ', VcRd/1e3, 'kN', ratio3)
print('bendingEfficiency= ', bendingEfficiency)
print('bendingEfficiencyRef= ', bendingEfficiencyRef, ratio4)
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if abs(ratio1)<1e-4 and abs(ratio2)<.05 and abs(ratio3)<.05 and abs(ratio4)<.05:
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
