# -*- coding: utf-8 -*-
'''Classes to store limit state control variables (internal forces, 
   strains, stresses,...) calculated in the analysis.
   THESE CLASSES ARE INTENDED TO REPLACE THE PROPERTIES DEFINED
   IN def_vars_control.py WICH MUST DISSAPEAR.
'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2014 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import scipy
from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc
from postprocess.reports import common_formats as fmt

class ControlVarsBase(object):
  '''Base class for control variabless.'''
  def __init__(self,combName= 'nil'):
    self.combName= combName #Name of the corresponding load combination
  def getCF(self):
    return -1.0
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    return self.combName
  def getLaTeXString(self,eTag,factor= 1e-3):
    ''' Returns a string that we can insert in a LaTeX table.
        eTag: element identifier.
        factor: factor for units (default 1e-3 -> kN)'''
    return str(eTag)+" & "+self.getLaTeXFields(factor)+" & "+fmt.Esf.format(self.getCF())+"\\\\\n"
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= []
    retval.append("detab,"+str(eTag)+",CF" +axis+","+str(self.getCF())+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= 'combName= "' + self.combName 
    retval+= '", CF=' + str(self.getCF())
    return retval
  def getStrConstructor(self,factor= 1e-3):
    retval= self.__class__.__name__ 
    retval+= '(' + self.getStrArguments(factor) + ')'
    return retval
  def strElementProp(self,eTag,nmbProp,factor= 1e-3):
    '''Write a string that will serve to read the element property from file.'''
    retval= 'preprocessor.getElementLoader.getElement('
    retval+= str(eTag)
    retval+= ").setProp("
    retval+= '"' + nmbProp + '"'
    retval+= ',' + self.getStrConstructor(factor)
    retval+= ")\n"
    return retval

class CFNMy(ControlVarsBase):
  '''Uniaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
    super(CFNMy,self).__init__(combName)
    self.CF= CF # Capacity factor or efficiency
    self.N= N # Axial force.
    self.My= My #Bending moment about y axis.
  def getCF(self):
    return self.CF
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CFNMy,self).getLaTeXFields(factor)
    retval+= " & "+fmt.Esf.format(self.N*factor)+" & "+fmt.Esf.format(self.My*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CFNMy,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
    retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(CFNMy,self).getStrArguments(factor)
    retval+= ',N= ' + str(self.N*factor) 
    retval+= ',My= ' + str(self.My*factor)
    return retval

class UniaxialBendingControlVars(CFNMy):
  '''Uniaxial bending. Normal stresses limit state variables.'''
  def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
    super(UniaxialBendingControlVars,self).__init__(combName,CF,N,My)
    self.idSection= idSection #Reinforced concrete section identifier.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= self.idSection+" & "
    retval+= super(UniaxialBendingControlVars,self).getLaTeXFields(factor)
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= 'idSection= "' + self.idSection +'", ' 
    retval+= super(UniaxialBendingControlVars,self).getStrArguments(factor)
    return retval

class CFNMyMz(CFNMy):
  '''Biaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
    super(CFNMyMz,self).__init__(combName,CF,N,My)
    self.Mz= Mz #Bending moment about z axis.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CFNMyMz,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Mz*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CFNMyMz,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(CFNMyMz,self).getStrArguments(factor)
    retval+= ',Mz= ' + str(self.Mz*factor)
    return retval

class BiaxialBendingControlVars(UniaxialBendingControlVars):
  '''Biaxial bending. Normal stresses limit state variables.'''
  def __init__(self,idSection= 'nil',combName= 'nil',CF= -1.0,N= 0.0,My= 0.0,Mz= 0.0):
    super(BiaxialBendingControlVars,self).__init__(idSection,combName,CF,N,My)
    self.Mz= Mz #Bending moment about z axis.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(BiaxialBendingControlVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Mz*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(BiaxialBendingControlVars,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(BiaxialBendingControlVars,self).getStrArguments(factor)
    retval+= ',Mz= ' + str(self.Mz*factor)
    return retval

class RCShearControlVars(BiaxialBendingControlVars):
  '''Control variables for biaxial bending shear limit state
     verification.'''
  def __init__(self,idSection=-1,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, Mu= 0.0, Vy= 0.0, Vz= 0.0, theta= 0.0, Vcu= 0.0, Vsu= 0.0, Vu= 0.0):
    super(RCShearControlVars,self).__init__(idSection,combName,CF,N,My,Mz)
    self.Mu= Mu #Ultimate bending moment.
    self.Vy= Vy #Shear parallel to the y axis.
    self.Vz= Vz #Shear parallel to the z axis.
    self.theta= theta #Strut angle.
    self.Vcu= Vcu #Vcu component of the shear resistance (defined in the codes).
    self.Vsu= Vsu #Vsu component of the shear resistance (defined in the codes).
    self.Vu= Vu # Shear resistance.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(BiaxialBendingControlVars,self).getLaTeXFields(factor)+' & '+fmt.Esf.format(self.Mu*factor)+' & '+fmt.Esf.format(self.Vy*factor)+' & '+fmt.Esf.format(self.Vz*factor)+' & '+fmt.Esf.format(self.Vu*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(RCShearControlVars,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Mu" +axis+","+str(self.Mu*factor)+"\n")
    retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
    retval.append("detab,"+str(eTag)+",Vz" +axis+","+str(self.Vz*factor)+"\n")
    retval.append("detab,"+str(eTag)+",Vcu" +axis+","+str(self.Vcu*factor)+"\n")
    retval.append("detab,"+str(eTag)+",Vsu" +axis+","+str(self.Vsu*factor)+"\n")
    retval.append("detab,"+str(eTag)+",Vu" +axis+","+str(self.Vu*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(RCShearControlVars,self).getStrArguments(factor)
    retval+= ',Mu= ' + str(self.Mu*factor)
    retval+= ',Vy= ' + str(self.Vy*factor)
    retval+= ',Vz= ' + str(self.Vz*factor)
    retval+= ',theta= ' + str(self.theta)
    retval+= ',Vcu= ' + str(self.Vcu*factor)
    retval+= ',Vsu= ' + str(self.Vsu*factor)
    retval+= ',Vu= ' + str(self.Vu*factor)
    return retval

class CrackControlBaseVars(CFNMyMz):
  '''Biaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName= 'nil',CF= -1.0,N= 0.0, My= 0.0, Mz= 0.0, steelStress= 0.0):
    super(CrackControlBaseVars,self).__init__(combName,CF,N,My)
    self.steelStress= steelStress #Bending moment about z axis.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CrackControlBaseVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.steelStress*factor*factor) #Factor for stresses ??
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CrackControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",steelStress" +axis+","+str(self.steelStress*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(CrackControlBaseVars,self).getStrArguments(factor)
    retval+= ',steelStress= ' + str(self.steelStress*factor*factor)
    return retval


class CrackControlVars(ControlVarsBase):
  '''Crack limit state control variables.'''
  def __init__(self,idSection= 'nil',crackControlBaseVarsPos= CrackControlBaseVars(), crackControlBaseVarsNeg= CrackControlBaseVars()):
    self.idSection= idSection #Reinforced concrete section identifier.
    self.crackControlVarsPos= crackControlBaseVarsPos #Cracks in + face.
    self.crackControlVarsNeg= crackControlBaseVarsNeg #Cracks in - face.
  def getCF(self):
    return max(self.crackControlVarsPos.getCF(),self.crackControlVarsNeg.getCF())
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= self.idSection+" & "
    retval+= self.crackControlVarsPos.getLaTeXFields()+" & "
    retval+= self.crackControlVarsNeg.getLaTeXFields()
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= 'idSection= "' + self.idSection + '"' 
    retval+= ', crackControlBaseVarsPos= ' + self.crackControlVarsPos.getStrConstructor(factor)
    retval+= ', crackControlBaseVarsNeg= ' + self.crackControlVarsNeg.getStrConstructor(factor)
    return retval


def writeControlVarsFromElements(controlVarName,preprocessor,outputFileName):
  '''Writes control var values from element into a file for
     doing graphics and into a latex file.

     Parameters:
     controlVarName: name of the control var. 
     preprocessor:    preprocessor from FEA model.
     outputFileName: name of the files to write (.py and .tex)
  '''
  texOutput1= open("/tmp/texOutput1.tmp","w")
  texOutput1.write("Section 1\n")
  texOutput2= open("/tmp/texOutput2.tmp","w")
  texOutput2.write("Section 2\n")
  xcOutput= open(outputFileName+".py","w")
  #printCabeceraListadoFactorCapacidad("texOutput1","1 ("+ sectionName1 +")")
  #printCabeceraListadoFactorCapacidad("texOutput2","2 ("+ sectionName2 +")")
  fcs1= [] #Capacity factors at section 1.
  fcs2= [] #Capacity factors at section 2.
  elementos= preprocessor.getSets["total"].getElements
  for e in elementos:
    eTag= e.getProp("idElem")
    idSection= e.getProp("idSection")
    controlVar= e.getProp(controlVarName)
    outStr= controlVar.getLaTeXString(eTag,1e-3)
    if(e.getProp("dir")==1):
      fcs1.append(controlVar.getCF())
      texOutput1.write(outStr)
      xcOutput.write(controlVar.strElementProp(eTag,controlVarName+'Dir1',1e-3))
    else:
      fcs2.append(controlVar.getCF())
      texOutput2.write(outStr)
      xcOutput.write(controlVar.strElementProp(eTag,controlVarName+'Dir2',1e-3))

  #printCierreListadoFactorCapacidad("texOutput1")
  #printCierreListadoFactorCapacidad("texOutput2")
  texOutput1.close()
  texOutput2.close()
  xcOutput.close()
    
  os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    
  os.system("rm -f "+"/tmp/texOutput1.tmp")
  os.system("rm -f "+"/tmp/texOutput2.tmp")
  retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
  return retval

# Imprime los resultados de la comprobación frente a tensiones normales
def writeControlVarsFromElementsForAnsys(controlVarName,preprocessor,outputFileName, sectionName1, sectionName2):
  '''
  Parameters:
    preprocessor:    preprocessor name
    outputFileName:  name of the output file containing tue results of the 
                     verification 
  '''
  texOutput1= open("/tmp/texOutput1.tmp","w")
  texOutput1.write("Section 1\n")
  texOutput2= open("/tmp/texOutput2.tmp","w")
  texOutput2.write("Section 2\n")
  ansysOutput1= open(outputFileName+".mac","w")
  ansysOutput2= open(outputFileName+"esf.mac","w")
  #printCabeceraListadoFactorCapacidad("texOutput1","1 ("+ sectionName1 +")")
  #printCabeceraListadoFactorCapacidad("texOutput2","2 ("+ sectionName2 +")")
  fcs1= [] #Capacity factors at section 1.
  fcs2= [] #Capacity factors at section 2.
  elementos= preprocessor.getSets["total"].getElements
  for e in elementos:
    eTag= e.getProp("idElem")
    controlVar= e.getProp(controlVarName)
    outStr= controlVar.getLaTeXString(eTag,1e-3)
    if(odd(e.tag)):
      fcs1.append(controlVar.getCF())
      texOutput1.write(outStr)
      ansOut= controlVal.getAnsysStrings(eTag,'1',1e-3)
      for s in ansOut:
        ansysOutput1.write(s)
    else:
      fcs2.append(controlVar.getCF())
      texOutput2.write(outStr)
      ansOut= controlVal.getAnsysStrings(eTag,'2',1e-3)
      for s in ansOut:
        ansysOutput2.write(s)
  
  #printCierreListadoFactorCapacidad("texOutput1")
  #printCierreListadoFactorCapacidad("texOutput2")
  texOutput1.close()
  texOutput2.close()
  ansysOutput1.close()
  ansysOutput2.close()
    
  os.system("cat /tmp/texOutput1.tmp /tmp/texOutput2.tmp > "+outputFileName+".tex")
    
  # os.system("rm -f "+"/tmp/acciones.xci")
  # os.system("rm -f "+"/tmp/cargas.xci")
  # os.system("rm -f "+"/tmp/elementos.xci")
  os.system("rm -f "+"/tmp/texOutput1.tmp")
  os.system("rm -f "+"/tmp/texOutput2.tmp")
  retval= [scipy.mean(fcs1),scipy.mean(fcs2)]
  return retval
