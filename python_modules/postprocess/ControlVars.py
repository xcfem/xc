# -*- coding: utf-8 -*-
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WICH MUST DISSAPEAR.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__cppyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc
from postprocess.reports import common_formats as fmt

class CFNMy(object):
  '''Uniaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName,CF,N,My):
    self.combName= combName #Name of the corresponding load combination
    self.CF= CF # Capacity factor or efficiency
    self.N= N # Axial force.
    self.My= My #Bending moment about y axis.
  def getLaTeXString(self,eTag,factor= 1e-3):
    ''' Returns a string that we can insert in a LaTeX table.
        eTag: element identifier.
        factor: factor for units (default 1e-3 -> kN)'''
    return str(eTag)+" & "+self.combName+" & "+fmt.Esf.format(self.N*factor)+" & "+fmt.Esf.format(self.My*factor)+" & "+fmt.Esf.format(self.CF)+"\\\\\n"
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= []
    retval.append("detab,"+str(eTag)+",CF" +axis+","+str(self.CF)+"\n")
    retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
    retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
    return retval
  def strElementProp(self,eTag,nmbProp,factor= 1e-3):
    '''Write a string that will serve to read the element property from file.'''
    retval= 'preprocessor.getElementLoader.getElement('
    retval+= str(eTag)
    retval+= ").setProp("
    retval+= '"' + nmbProp + '"'
    retval+= ',' + self.__class__.__name__ 
    retval+= '(combName= "' + self.combName 
    retval+= '", CF=' + str(self.CF)
    retval+= ',N= ' + str(self.N*factor) 
    retval+= ',My= ' + str(self.My*factor) 
    retval+= "))\n"
    return retval


class CFNMyMz(CFNMy):
  '''Biaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName,CF,N,My,Mz):
    super(CFNMyMz,self).__init__(combName,CF,N,My)
    self.Mz= Mz #Bending moment about z axis.
  def getLaTeXString(self,eTag,factor= 1e-3):
    ''' Returns a string that we can insert in a LaTeX table.
        eTag: element identifier.
        factor: factor for units (default 1e-3 -> kN)'''
    return str(eTag)+" & "+self.combName+" & "+fmt.Esf.format(self.N*factor)+" & "+fmt.Esf.format(self.My*factor)+" & "+fmt.Esf.format(self.Mz*factor)+" & "+fmt.Esf.format(self.CF)+"\\\\\n"
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CFNMyMz,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
    return retval
  def strElementProp(self,eTag,nmbProp,factor= 1e-3):
    '''Write a string that will serve to read the element property from file.'''
    retval= 'preprocessor.getElementLoader.getElement('
    retval+= str(eTag)
    retval+= ").setProp("
    retval+= '"' + nmbProp + '"'
    retval+= ',' + self.__class__.__name__ 
    retval+= '(combName= "' + self.combName 
    retval+= '", CF=' + str(self.CF)
    retval+= ',N= ' + str(self.N*factor) 
    retval+= ',My= ' + str(self.My*factor) 
    retval+= ',Mz= ' + str(self.Mz*factor)
    retval+= "))\n"
    return retval

