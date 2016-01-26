# -*- coding: utf-8 -*-
from __future__ import division
import sys
from materials import typical_materials


class sectionProperties(object):
  """Abstract section properties (area, moments of inertia,...)"""
  def __init__(self,name,E,nu):
    self.nmb= name
    self.E= E
    self.nu= nu
  def A(self):
    """ Section's area."""
    raise "Abstract method, please override"
    return 0.0
  def Iy(self):
    raise "Abstract method, please override"
    return 0.0    
  def Iz(self):
    raise "Abstract method, please override"
    return 0.0    

  def J(self):
    raise "Abstract method, please override"
    return 0.0    

  def G(self):
    return self.E/(2*(1+self.nu))
  def Wyel(self):
    raise "Abstract method, please override"
    return 0.0    

  def Wzel(self):
    raise "Abstract method, please override"
    return 0.0    

  def defSeccElastica3d(self,preprocessor):
    materiales= preprocessor.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticSection3d(preprocessor,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.Iy(),self.J())
      return retval;
  def defSeccShElastica3d(self,preprocessor):
    materiales= preprocessor.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticShearSection3d(preprocessor,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.Iy(),self.J(),self.alphaY())
      return retval;
  def defSeccElastica2d(self,preprocessor):
    # Define una sección elástica para elementos 2d a partir de los datos del registro.
    materiales= preprocessor.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticSection2d(preprocessor,self.nmb,self.A(),self.E,self.Iz())
      return retval;
  def defSeccShElastica2d(self,preprocessor):
    # Define una sección elástica para elementos 2d a partir de los datos del registro.
    materiales= preprocessor.getMaterialLoader
    if(materiales.materialExists(self.nmb)):
      sys.stderr.write("Section: "+self.nmb+" is already defined.")
    else:
      retval= typical_materials.defElasticShearSection2d(preprocessor,self.nmb,self.A(),self.E,self.G(),self.Iz(),self.alphaY())
      return retval;


# Define una sección elástica para elementos 3d a partir de los datos del registro.
def defSeccElastica3d(preprocessor,defSecc):
  print "DEPRECATED; use object's method defSeccElastica3d"
  return defSecc.defSeccElastica3d(preprocessor)

def defSeccShElastica3d(preprocessor,defSecc):
  print "DEPRECATED; use object's method defSeccShElastica3d"
  return defSecc.defSeccShElastica3d(preprocessor)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccElastica2d(preprocessor,defSecc):
  print "DEPRECATED; use object's method defSeccElastica2d"
  return defSecc.defSeccElastica2d(preprocessor)

# Define una sección elástica para elementos 2d a partir de los datos del registro.
def defSeccShElastica2d(preprocessor,defSecc):
  print "DEPRECATED; use object's method defSeccShElastica2d"
  return defSecc.defSeccShElastica2d(preprocessor)

