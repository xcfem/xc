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
from materials.sections import section_properties
from model import predefined_spaces
from solution import predefined_solutions
from materials.sections.fiber_section import def_column_RC_section
# from tabulate import tabulate

# Units
lbf= 4.44822 # Pound force to N
ft= 0.3048
inch= 25.4e-3
pcf= lbf/ft**3
pci= lbf/inch**3
psf= lbf/ft**2
psi= lbf/inch**2
ksi= 1000*psi

# Materials definition
## Soil materials
### Dry soil.
soil1= earth_pressure.CoulombSoil(phi= math.radians(30), rho= 105*pcf/g, rhoSat= 105*pcf/g)
soil1.Kh= 2.9*pci
soil2= earth_pressure.CoulombSoil(phi= math.radians(30), rho= 128.5*pcf/g, rhoSat= 128.5*pcf/g)
soil2.Kh= 15*pci
soil3= earth_pressure.BellSoil(phi= math.radians(1), c= 1500*psf, rho= 122.5*pcf/g, rhoSat= 122.5*pcf/g)
soil3.Kh= 87*pci
### Soil strata.
L0= -8*ft # pile wall top (m).
L1= L0+4*ft # position of the anchor (m).
L2= L1+4*ft # Soil 1 bottom, water table level at rigth (m).
L3= L2+4*ft # water table level at left (m).
L4= L3+(22-4)*ft # Soil 2 bottom (m), excavation depth.
L= L4+(31-22)*ft # pile wall tip (m).
soilLayersDepths= [L0, L1, L2, L3, L4, L]
soilLayers= [soil1, soil1, soil2, soil2, soil3, soil3, soil3]

## Pile material.
sheetPileSectionGeometry= section_properties.GenericSection2D(name= 'sheet_pile_section_geometry', area= 9.4*inch**2, I= 220.4*inch**4, W=1.0, alph= 5/6.0)
sheetPileMaterial= typical_materials.BasicElasticMaterial(E= 29e6*psi, nu= 0.3)
sheetPileSectionMaterial= typical_materials.BeamMaterialData(name= 'sheet_pile_section', section= sheetPileSectionGeometry, material= sheetPileMaterial)
## Anchor material.
anchorSectionGeometry= section_properties.GenericSection1D(name= 'anchor_section_geometry', area= 1*inch**2)
anchorMaterial= typical_materials.ElasticPerfectlyPlasticMaterial(E= 29e6*psi, nu= 0.3, fyp= 40*ksi, fyn= 40e3*ksi)
anchorSectionMaterial= typical_materials.BeamMaterialData(name= 'anchor_section', section= anchorSectionGeometry, material= anchorMaterial)

pileWall= pw.PileWall(pileSection= sheetPileSectionMaterial, soilLayersDepths= soilLayersDepths, soilLayers= soilLayers, excavationDepth= L4, pileSpacing= 1.0*ft, waterTableDepth= [L2, L3])

# Mesh generation
pileWall.genMesh()

# Get top node.
topNode= pileWall.getTopNode()

# Get node to attach the anchor
anchorNode= pileWall.getNodeAtDepth(depth= L1)


