# -*- coding: utf-8 -*-
''' Design of hinges under general loading.

   Based on the article:
   Markic T, Kaufmann W.
   Modeling and design of concrete hinges under general loading. 
   Structural Concrete. 2023;24(4):5122–49. 
   https://doi.org/10.1002/suco.202201110
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2025, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from materials import concrete_hinge

ch2= concrete_hinge.ConcreteHinge(d1= 38e-3, d= 0.38, b1= 1.05, b= 1.2, ht= 8e-3)

# Compute geometry capacity factor.
geom_cf= ch2.getGeometryEfficiency(threshold= 1.06)


print(geom_cf)
