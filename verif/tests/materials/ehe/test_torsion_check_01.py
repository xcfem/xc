# -*- coding: utf-8 -*-
''' Check torsion strength of reinforced concrete section.

Reference values obtained from "Prontuario Informático del Hormigón Estructural 3.1.9 según EHE-08.
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
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from materials.sections.fiber_section import def_simple_RC_section
from postprocess import limit_state_data as lsd

concrete= EHE_materials.HA30
steel= EHE_materials.B500S

# Define reinforced concrete section.
section= def_simple_RC_section.RCRectangularSection(name='test', sectionDescr= 'dummy section for testing computation of torsion parameters', width= 1.1, depth= 3.4, concrType= concrete, reinfSteelType= steel)
## Main reinforcement.
section.negatvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 32e-3, nRebars= 6, width= 1.1, nominalCover= 40e-3)])
section.positvRebarRows= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.ReinfRow(rebarsDiam= 32e-3, nRebars= 6, width= 1.1, nominalCover= 40e-3)])
## Torsion reinforcement.
section.torsionReinf= def_simple_RC_section.TorsionReinforcement(familyName= "torsionReinf", A1= 31.41592e-4, nShReinfBranches= 2, areaShReinfBranch= math.pi*(12e-3/2)**2, shReinfSpacing= 0.30, angThetaConcrStruts= math.pi/4.0)


## Compute section geometry
### XC problem.
feProblem= xc.FEProblem()
preprocessor=  feProblem.getPreprocessor
### Compute section geometry.
section.defSectionGeometry(preprocessor= preprocessor, matDiagType= 'd')

# Compute torsion parameters.
torsionParameters= EHE_limit_state_checking.computeEffectiveHollowSectionParametersRCSection(section)

# Check values of torsion parameters.
## Get effetive thickness
he= torsionParameters.he() # effective thickness of the wall of the design section.
Ae= torsionParameters.Ae() # Area enclosed by the middle line of the design effective hollow section (figure 45.2.1).
ue= torsionParameters.ue() # perimeter of the middle line in the design effective hollow section Ae.
limitState= EHE_limit_state_checking.torsionResistance # Torsion strength limit state.
torsionController= limitState.getController()

Tu1= torsionController.calcTu1(rcSection= section, Ncd= 0.0, Ae= Ae, he= he, alpha= 0.6)
Tu2= torsionController.calcTu2(rcSection= section, Ae= Ae)
Tu3= torsionController.calcTu3(rcSection= section, Ae= Ae, ue= ue)
Tu= torsionController.calcTu(rcSection= section, Ncd= 0.0, Ae= Ae, he= he, ue= ue, alpha= 0.6)

# Check results.
ratio1= abs(Tu1-6110.5e3)/6110.5e3
# ratio2= abs(Tu2-2053.5323679628596e3)/2053.5323679628596e3
ratio3= abs(Tu3-700.2e3)/700.2e3

'''
print('concrete cover; c= ', section.minCover, 'm')
print('effective thickness; he= ', he, 'm')
print('area enclosed by the middle line of the design effective hollow section; Ae= ', Ae, 'm2')
print('perimeter of the middle line of the design effective hollow section; ue= ', ue, 'm')
print('ultimate torsional force that compressed struts can resist: Tu1= ', Tu1/1e3,'kN.m', ratio1)
print('ultimate torsional force which transverse reinforcement can resist: Tu2= ', Tu2/1e3,'kN.m')
print('ultimate torsional force which longitudinal reinforcements can resist: Tu3= ', Tu3/1e3,'kN.m', ratio3)
print('ultimate torsional force; Tu= ', Tu/1e3,'kN.m')
'''

fname= os.path.basename(__file__)
from misc_utils import log_messages as lmsg
if ((abs(ratio1)<1e-2) and abs(ratio3)<1e-2):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')
