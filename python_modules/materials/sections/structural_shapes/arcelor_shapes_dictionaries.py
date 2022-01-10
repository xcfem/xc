# -*- coding: utf-8 -*-
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2018, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.ortega@ciccp.es "


import importlib.resources
import json

''' ARCELOR's structural steel shapes (metric units).'''

# European I beams

# Section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Control shear distortion constants:
#    alpha-> alphaZ,alphaY

'''
    b: width  [m]
    h: height [m]
    e: thickness [m]
    P: mass per metre [kg/m]
    A: area of section [m2]
    Iz: second moment of area about strong axis [m4]
    Wzel: elastic modulus about strong axis [m4]
    Wzpl: plastic modulus about strong axis [m4]
    iz: radius of gyration about strong axis [m] 
    Iy: second moment of area about weak axis [m4]
    Wyel: elastic modulus about weak axis [m4]
    Wypl: plastic modulus about weak axis [m4]
    iy: radius of gyration about weak axis [m] 
    It: torsional constant J [m4]
    E: elastic modulus [N/m2]
    nu: Poisson's modulus
'''

arcelor_shapes= dict()

IPE= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_ipe_profiles.json") as file:
    IPE= json.load(file)
arcelor_shapes['IPE']= IPE

# Arcelor IPN steel shapes.

# Section axis:


#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#   alpha-> alphaY,alphaZ}

IPN= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_ipn_profiles.json") as file:
    IPN= json.load(file)
arcelor_shapes['IPN']= IPN


# Arcelor HE steel shapes.

# Axis of the section:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#    alpha-> alphaY,alphaZ

HE= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_he_profiles.json") as file:
    HE= json.load(file)
arcelor_shapes['HE']= HE


# Arcelor HL extra wide flange beams.

# Axis of the section:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# XXX Check shear distortion constants:
#    alpha-> alphaY,alphaZ

HL= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_hl_profiles.json") as file:
    HL= json.load(file)
arcelor_shapes['HL']= HL

# Arcelor UPN steel shapes.

# Cross section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#       ---           ---
#       |             | 
#       | -> Y        | -> Z
#       |             |
#       ---           ---

#       |
#       v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

UPN= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_upn_profiles.json") as file:
    UPN= json.load(file)
arcelor_shapes['UPN']= UPN

# Arcelor L steel shapes.

L= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_l_profiles.json") as file:
    L= json.load(file)
arcelor_shapes['L']= L

# Arcelor U shaped sheet pile.

# Cross section axis:

#    ARCELOR                   XC

#                              ^ Y                    
#                              |

#       -----                -----
#      /     \              /     \
#     /       \ -> Y       /       \ | -> Z
#    /         \          /         \
# ---           ---    ---           ---

#         |
#         v Z

# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# Warning: area in m2/m, inertia in m4/m, Wz in m3/m and so on


AU= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_au_profiles.json") as file:
    AU= json.load(file)
arcelor_shapes['AU']= AU

#Code to import data.
# dataList= [('P',6,''),('h',2,'e-3'),('b',1,'e-3'),('s',4,'e-3'),('t',3,'e-3'),('A',5,'e-4'),('Iz',8,'e-8'),('Wzel',9,'e-6'),('Wzpl',11,'e-8'),('Szel',10,'e-6')]
# import csv
# import sys

# f = open('aux/au_sheet_pile.csv', 'rt')
# try:
#     reader = csv.reader(f)
#     for row in reader:
#         output= "AU['"+row[0]+"']= {'nmb':'"+row[0]+"', "
#         for t in dataList:
#             output+= "'"+t[0]+"':"+row[t[1]]+t[2]+', '
#         output+= "'E':2.1e+11, 'nu':0.3}"
#         print(output)
# finally:
#     f.close()

# Arcelor square hollow tubes.
# Cross section axis:

#    ARCELOR          XC

#                     ^ Y                    
#                     |

#     -----         -----
#       |             | 
#       | -> Y        | -> Z
#       |             |
#     -----         -----

#       |
#       v Z


# The axis used in Arcelor documentation are different from those used in XC
# (strong axis parallel to z axis) in other words: values for Y and Z axis 
# are swapped with respect to those in the catalog.

# In this case the inertia tensor is cylindrical so, in its plane, both
# values are identical: no need to exchange.


SHS= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_shs_profiles.json") as file:
    SHS= json.load(file)
arcelor_shapes['SHS']= SHS

'''
XXX Check shear distortion constants
   alpha-> alphaZ,alphaY
'''

# Arcelor rectangular hollow tubes.

RHS= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_rhs_profiles.json") as file:
    RHS= json.load(file)
arcelor_shapes['RHS']= RHS

# Tata profiles

# Tata steel circular hollow tubes.
CHS= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_chs_profiles.json") as file:
    CHS= json.load(file)
arcelor_shapes['CHS']= CHS


# ARCELOR's hot rolled round steel bars.
R= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_r_profiles.json") as file:
    R= json.load(file)
arcelor_shapes['R']= R

# ARCELOR's UK universal columns.
UC= None
with importlib.resources.open_text("materials.sections.structural_shapes.aux", "arcelor_uc_profiles.json") as file:
    UC= json.load(file)
arcelor_shapes['UC']= UC

