# -*- coding: utf-8 -*-
''' Check torsion strength of reinforced concrete section according to clause
    6.3 of Eurocode 2.

Reference values obtained from "Eurocode 2 Worked Examples (rev A 31-03-2017) Example 6.7 Shear – Torsion interaction diagrams [EC2 clause 6.3].
'''

from __future__ import print_function
from __future__ import division
from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2025, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import xc
from materials.ec2 import EC2_materials
from materials.ec2 import EC2_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data as lsd

concrete= EC2_materials.EC2Concrete("C28-35",-28e6,1.5, alphacc= 0.85)
steel= EC2_materials.S500B

# Define reinforced concrete section.
cover= 25e-3
section= def_simple_RC_section.RCRectangularSection(name='test', sectionDescr= 'dummy section for testing torsion ULS checking', width= 0.4, depth= 0.6, concrType= concrete, reinfSteelType= steel)
## Main reinforcement.
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 16e-3, nRebars= 3, nominalCover= cover)])
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 16e-3, nRebars= 3, nominalCover= cover)])
## Torsion reinforcement.
torsionLongitudinalReinforcementArea= 1788e-6
section.torsionReinf= def_simple_RC_section.TorsionReinforcement(familyName= "torsionReinf", A1= torsionLongitudinalReinforcementArea, nShReinfBranches= 2, areaShReinfBranch= math.pi*(4e-3/2.0)**2, shReinfSpacing= 0.133686, angThetaConcrStruts= math.radians(21.8))

## Compute section geometry
### XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
### Compute section geometry.
section.defSectionGeometry(preprocessor= preprocessor, matDiagType= 'd')

# Compute torsion parameters.
torsionParameters= EC2_limit_state_checking.compute_effective_hollow_section_parameters_rc_section(section)

# Check values of torsion parameters.
## Get effetive thickness
tef= torsionParameters.tef() # effective thickness of the wall of the design section.
tef_ref= 0.12 # See spreadshee with the same file name in this folder.
ratio1= abs(tef-tef_ref)/tef_ref
Ac= torsionParameters.A() # Concrete area.
Ak= torsionParameters.Ak() # Area enclosed by the middle line of the design effective hollow section (figure 6.11 of EC2).
uk= torsionParameters.uk() # perimeter of the middle line in the design effective hollow section Ak.

limitState= EC2_limit_state_checking.torsionResistance # Torsion strength limit state.
torsionController= limitState.getController()

# Design torsional resistance moment according to expression (6.30) of EC2.
TRd_max= torsionController.calcTRd_max(rcSection= section, NEd= 0.0, Ac= Ac, Ak= Ak, tef= tef)
TRD_max_ref= 94.0242742234122e3 # See spreadshee with the same file name in this
                                # folder.
ratio2= abs(TRd_max-TRD_max_ref)/TRD_max_ref

# Maximum torsional resist moment with no shear reinf. – cl.6.3.2(5)
Trdc= torsionController.calcTrdc(rcSection= section, Ak= Ak, tef= tef)
Trdc_ref= 41.6399852502584e3 # See spreadshee with the same file name in this
                             # folder.
ratio3= abs(Trdc-Trdc_ref)/Trdc_ref

# Maximum torsional resist moment due to longitudinal reinforcement
Tasl_max= torsionController.calcTasl_max(rcSection= section, Ak= Ak, uk= uk)
Tasl_max_ref= 50.5873803219522e3 # See spreadshee with the same file name in
                                 # this folder.
ratio4= abs(Tasl_max-Tasl_max_ref)/Tasl_max_ref

# Maximum torsional resist moment due to transverse reinforcement
Asw= section.torsionReinf.getAs()
a_sw= Asw/section.torsionReinf.shReinfSpacing
Tasw_max= torsionController.calcTasw_max(rcSection= section, Ak= Ak)
Tasw_max_ref= 50.5380053718561e3 # See spreadshee with the same file name in
                                 # this folder.
ratio5= abs(Tasw_max-Tasw_max_ref)/Tasw_max_ref

'''
print('tef= ', tef*1e3, 'mm', ratio1)
print('TRd_max= ', TRd_max/1e3, 'kN.m', ratio2)
print('Trdc= ', Trdc/1e3, 'kN.m', ratio3)
print('Tasl_max= ', Tasl_max/1e3, 'kN.m', ratio4)
print('Asw= ', Asw*1e6, 'mm2/m')
print('Tasw_max= ', Tasw_max/1e3, 'kN.m', ratio5)
'''

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if ((abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6) and (abs(ratio4)<1e-6)and (abs(ratio5)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
