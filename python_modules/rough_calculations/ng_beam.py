# -*- coding: utf-8 -*-
# Base for beam formulas

from __future__ import division

#
#  |----> x
#
#  |--------------------
#

class Beam(object):

  def __init__(self,E= 1.0, I= 1.0, l= 1.0):
    self.E= E
    self.I= I
    self.l= l

  def EI(self):
    return self.E*self.I
