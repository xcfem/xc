# -*- coding: utf-8 -*-
''' Nonlinear soil spring model inspired in the example 2 of the publication "User's Guide: Computer Program for Winkler Soil-Structure Interaction Analysis of Sheet-Pile Walls (CWALSSI)" by William P. Dawkins, Oklahoma State University U.S. Army Corps of Engineers.
'''

import math
import geom
import xc
from scipy.constants import g
from geotechnics import earth_pressure
from geotechnics import frictional_cohesive_soil as fcs
from geotechnics.earth_retaining import pile_wall as pw
from materials import typical_materials
from model import predefined_spaces
from solution import predefined_solutions
from materials.sections.fiber_section import def_column_RC_section
from materials.ehe import EHE_materials
from materials.ehe import EHE_limit_state_checking
from tabulate import tabulate

# Units
lbf= 4.44822 # Pound force to N
ft= 0.3048
inch= 25.4e-3
pcf= lbf/ft**3
pci= lbf/inch**3
psf= lbf/ft**2

# Materials definition
## Soil materials
### Dry soil.
soil1= earth_pressure.RankineSoil(phi= math.radians(30), rho= 105*pcf/g, rhoSat= 105*pcf/g)
soil1.Kh= 2.9*pci
soil2= earth_pressure.RankineSoil(phi= math.radians(30), rho= 128.5*pcf/g, rhoSat= 128.5*pcf/g)
soil2.Kh= 15*pci
soil3= earth_pressure.CoulombSoil(phi= math.radians(1), c= 1500*psf, rho= 122.5*pcf/g, rhoSat= 122.5*pcf/g)
soil2.Kh= 15*pci
print(1500*psf/1e3)

