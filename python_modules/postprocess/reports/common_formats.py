# -*- coding: utf-8 -*-
''' Common use format strings.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT), Ana Ortega (AO_O)"
__copyright__= "Copyright 2019, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.ortega.ort@gmail.com"

Force= "{:6.2f}" # six places two decimals.
Esf= Force
Stress= "{:6.2f}"
Pressure= Stress
Diam= "{:2.0f}"
Length= "{:4.2f}"
Speed= Length
Acceleration= Length
LengthMM= "{:4.0f}" # Length in mm (without decimals).
Area= "{:6.2f}"
Inertia= "{:6.4f}" # six places four decimals.
Factor= "{:4.2f}"
Displacement= "{:4.2f}"
Angle= "{:4.2f}"

fmt3_1f= '{:3.1f}'
fmt4_1f= '{:4.1f}'
fmt4_2f= '{:4.2f}'
fmt5_2f= '{:5.2f}'
fmt5_3f= '{:5.3f}'
fmt6_3f= '{:6.3f}'
