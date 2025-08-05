# -*- coding: utf-8 -*-
''' Check torsion strength of reinforced concrete section according to clause
    45.2 of Eurocode 2.

Test inspired on the example: "Eurocode 2 Worked Examples (rev A 31-03-2017) Example 6.7 Shear – Torsion interaction diagrams [EC2 clause 6.3].
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2024, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import math
import os
import xc
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data as lsd

concrete= EHE_materials.EHEConcrete("HA28",-28e6,1.5)
concrete.alfacc= 0.85
steel= EHE_materials.B500S

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
torsionParameters= EHE_limit_state_checking.compute_effective_hollow_section_parameters_rc_section(section)

# Check values of torsion parameters.
## Get effetive thickness
he= torsionParameters.he() # effective thickness of the wall of the design section.
he_ref= 0.12 # See spreadshee with the same file name in this folder.
ratio1= abs(he-he_ref)/he_ref
Ac= torsionParameters.A() # Concrete area.
Ae= torsionParameters.Ae() # Area enclosed by the middle line of the design effective hollow section (figure 45.2.1 of EHE).
ue= torsionParameters.ue() # perimeter of the middle line in the design effective hollow section Ae.

limitState= EHE_limit_state_checking.torsionResistance # Torsion strength limit state.
torsionController= limitState.getController()

# Design torsional resistance moment according to expression (6.30) of EHE.
Tu1= torsionController.calcTu1(rcSection= section, Ncd= 0.0, Ae= Ae, he= he)
Tu1_ref= 63.5299150158191e3 # See spreadshee with the same file name in this
                                # folder.
ratio2= abs(Tu1-Tu1_ref)/Tu1_ref

# Maximum torsional resist moment due to longitudinal reinforcement
Tu3= torsionController.calcTu3(rcSection= section, Ae= Ae, ue= ue)
Tu3_ref= 50.5873803219522e3 # See spreadshee with the same file name in
                                 # this folder.
ratio3= abs(Tu3-Tu3_ref)/Tu3_ref

# Maximum torsional resist moment due to transverse reinforcement
Asw= section.torsionReinf.getAs()
a_sw= Asw/section.torsionReinf.shReinfSpacing
Tu2= torsionController.calcTu2(rcSection= section, Ae= Ae)
Tu2_ref= 50.5380053718561e3 # See spreadshee with the same file name in
                                 # this folder.
ratio4= abs(Tu2-Tu2_ref)/Tu2_ref

'''
print('he= ', he*1e3, 'mm', ratio1)
print('Tu1= ', Tu1/1e3, 'kN.m', ratio2)
print('Tu3= ', Tu3/1e3, 'kN.m', ratio3)
print('Asw= ', Asw*1e6, 'mm2/m')
print('Tu2= ', Tu2/1e3, 'kN.m', ratio4)
'''

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if ((abs(ratio1)<1e-6) and (abs(ratio2)<1e-6) and (abs(ratio3)<1e-6) and (abs(ratio4)<1e-4)):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
