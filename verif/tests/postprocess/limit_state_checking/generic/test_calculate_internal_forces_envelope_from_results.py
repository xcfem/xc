# -*- coding: utf-8 -*-
''' Test computation of internal forces envelope from the results stored
    in a JSON file.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import json
from postprocess import def_vars_control as vc
from import_export import convert_internal_forces as cif

pth= os.path.dirname(__file__)
if(not pth):
  pth= "."

internalForcesFName= pth+"/../../../aux/internal_forces/intForce_ULS_9565.json"

with open(internalForcesFName, 'r') as f:
    internalForcesDict= json.load(f)


# elementTypesDict= cif.get_internal_forces_dict_element_types(internalForcesDict)
# print(elementTypesDict)


internalForcesEnvelope= cif.compute_internal_forces_envelope(internalForcesDict)
refValues= {9565: {0: {'N+': -803399.3726648442, 'comb_N+': 'ULS132', 'N-': -1263113.6363397604, 'comb_N-': 'ULS125', 'Vy+': 5329.327278171984, 'comb_Vy+': 'ULS132', 'Vy-': -42646.53368434588, 'comb_Vy-': 'ULS125', 'Vz+': 408474.2771087506, 'comb_Vz+': 'ULS124a6', 'Vz-': 162009.29574894827, 'comb_Vz-': 'ULS132', 'T+': 100.33836375103087, 'comb_T+': 'ULS132', 'T-': -1730.831664521533, 'comb_T-': 'ULS124a6', 'My+': 1133983.5992531823, 'comb_My+': 'ULS124a6', 'My-': 546949.4934210766, 'comb_My-': 'ULS132', 'Mz+': 10908.695560030143, 'comb_Mz+': 'ULS133', 'Mz-': -14731.6048655386, 'comb_Mz-': 'ULS122'}, 1: {'N+': -775284.0535285535, 'comb_N+': 'ULS132', 'N-': -1234998.3172034698, 'comb_N-': 'ULS125', 'Vy+': 5329.327278171984, 'comb_Vy+': 'ULS132', 'Vy-': -42580.84170252325, 'comb_Vy-': 'ULS125', 'Vz+': 407838.2745436477, 'comb_Vz+': 'ULS124a6', 'Vz-': 162009.29574894827, 'comb_Vz-': 'ULS132', 'T+': 100.33836375103087, 'comb_T+': 'ULS132', 'T-': -1730.831664521533, 'comb_T-': 'ULS124a6', 'My+': 1684327.9163680356, 'comb_My+': 'ULS124a6', 'My-': 765397.4274991003, 'comb_My-': 'ULS132', 'Mz+': 42939.53045983922, 'comb_Mz+': 'ULS125', 'Mz-': 3510.549006586207, 'comb_Mz-': 'ULS132'}}}

# print(internalForcesEnvelope)

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if (refValues==internalForcesEnvelope):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

