# -*- coding: utf-8 -*-
''' Check torsion strength of reinforced concrete section according to clause
    6.3 of Eurocode 2.

Reference values obtained from "Eurocode 2 Worked Examples (rev A 31-03-2017) Example 6.7 Shear – Torsion interaction diagrams [EC2 clause 6.3].
'''

from __future__ import print_function
from __future__ import division
from materials.ehe import EHE_materials

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
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

concrete= EC2_materials.C30
concrete.alfacc= 0.85
steel= EC2_materials.S450B

# Define reinforced concrete section.
cover= 25e-3
section= def_simple_RC_section.RCRectangularSection(name='test', sectionDescr= 'dummy section for testing computation of torsion parameters', width= 0.3, depth= 0.5, concrType= concrete, reinfSteelType= steel)
## Main reinforcement.
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 16e-3, nRebars= 3, nominalCover= cover)])
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 16e-3, nRebars= 3, nominalCover= cover)])
## Torsion reinforcement.
torsionLongitudinalReinforcementArea= 4*math.pi*(12e-3/2.0)**2
section.torsionReinf= def_simple_RC_section.TorsionReinforcement(familyName= "torsionReinf", A1= torsionLongitudinalReinforcementArea, nShReinfBranches= 2, areaShReinfBranch= math.pi*(12e-3/2)**2, shReinfSpacing= 0.30, angThetaConcrStruts= math.radians(26.56))

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
tef_ref= 0.09375  # See spreadshee with the same file name in this folder.
ratio1= abs(tef-tef_ref)/tef_ref
Ac= torsionParameters.A() # Concrete area.
Ak= torsionParameters.Ak() # Area enclosed by the middle line of the design effective hollow section (figure 6.11 of EC2).
uk= torsionParameters.uk() # perimeter of the middle line in the design effective hollow section Ak.

limitState= EC2_limit_state_checking.torsionResistance # Torsion strength limit state.
torsionController= limitState.getController()

TRd_max= torsionController.calcTRd_max(rcSection= section, NEd= 0.0, Ac= Ac, Ak= Ak, tef= tef)
TRD_max_ref= 56.3993367912455e3 # See spreadshee with the same file name in this
                                # folder.
ratio2= abs(TRd_max-TRD_max_ref)/TRD_max_ref

'''
print('cover: c= ', section.minCover*1e3, 'mm')
print('Ak= ', Ak*1e6, 'mm2')
print('tef= ', tef*1e3, 'mm', ratio1)
print('TRd_max= ', TRd_max/1e3, 'kN.m', ratio2)
'''

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if ((abs(ratio1)<1e-6) and abs(ratio2)<1e-6):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
