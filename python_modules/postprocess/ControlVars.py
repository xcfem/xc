# -*- coding: utf-8 -*-
from __future__ import division
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
import inspect
from miscUtils import LogMessages as lmsg
import xc_base
import geom
import xc
from postprocess.reports import common_formats as fmt
from postprocess import extrapolate_elem_attr as ext


class ControlVarsBase(object):
  '''Base class for control variabless.'''
  def __init__(self,combName= 'nil'):
    self.combName= combName #Name of the corresponding load combination
  def getCF(self):
    return -1.0
  def __call__(self,arguments):
    retval= None
    obj= self
    args= arguments.split('.')
    for arg in args:
      if(hasattr(obj,arg)):
        obj= getattr(obj,arg)
        if(hasattr(obj,'__call__')):
          obj= obj.__call__()
      else:
        lmsg.error('argument: '+ arg+' not found')
    retval= obj
    return retval
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

class NMy(ControlVarsBase):
  '''Uniaxial bending. Internal forces for a combination.'''
  def __init__(self,combName= 'nil',N= 0.0,My= 0.0):
    super(NMy,self).__init__(combName)
    self.N= N # Axial force.
    self.My= My #Bending moment about y axis.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(NMy,self).getLaTeXFields(factor)
    retval+= " & "+fmt.Esf.format(self.N*factor)+" & "+fmt.Esf.format(self.My*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(NMy,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",N" +axis+","+str(self.N*factor)+"\n")
    retval.append("detab,"+str(eTag)+",My" +axis+","+str(self.My*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(NMy,self).getStrArguments(factor)
    retval+= ',N= ' + str(self.N*factor) 
    retval+= ',My= ' + str(self.My*factor)
    return retval

class NMyMz(NMy):
  '''Uniaxial bending. Internal forces for a combination.'''
  def __init__(self,combName= 'nil',N= 0.0,My= 0.0, Mz= 0.0):
    super(NMyMz,self).__init__(combName,N,My)
    self.Mz= Mz #Bending moment about z axis.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(NMyMz,self).getLaTeXFields(factor)
    retval+= " & "+fmt.Esf.format(self.Mz*factor)
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(NMyMz,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Mz" +axis+","+str(self.Mz*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(NMyMz,self).getStrArguments(factor)
    retval+= ',Mz= ' + str(self.Mz*factor)
    return retval


class CFNMy(NMy):
  '''Uniaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName= 'nil',CF= -1.0,N= 0.0,My= 0.0):
    super(CFNMy,self).__init__(combName,N,My)
    self.CF= CF # Capacity factor or efficiency
  def getCF(self):
    return self.CF

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
    self.steelStress= steelStress #Stress in rebars.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(CrackControlBaseVars,self).getLaTeXFields(factor)+" & "+fmt.Stress.format(self.steelStress*factor*factor) #Factor for stresses ??
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
  def getMaxSteelStress(self):
    '''Maximum value for rebar stresses.'''
    return max(self.crackControlVarsPos.steelStress,self.crackControlVarsNeg.steelStress)
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

class FatigueControlBaseVars(NMyMz):
  '''Biaxial bending. Normal stresses limit state variables.'''
  def __init__(self,combName= 'nil',N= 0.0, My= 0.0, Mz= 0.0, Vy= 0.0, posSteelStress= 0.0, negSteelStress= 0.0, concreteStress= 0.0):
    super(FatigueControlBaseVars,self).__init__(combName,N,My,Mz)
    self.Vy= Vy #Shear.
    self.posSteelStress= posSteelStress #traction stress in rebars.
    self.negSteelStress= negSteelStress #negative stress in rebars.
    self.concreteStress= concreteStress #negative stress in concrete.
  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(FatigueControlBaseVars,self).getLaTeXFields(factor)+" & "+fmt.Esf.format(self.Vy*factor)+" & "+fmt.Stress.format(self.posSteelStress*factor*factor)+" & "+fmt.Stress.format(self.negSteelStress*factor*factor)+" & "+fmt.Stress.format(self.concreteStress*factor*factor) #Factor for stresses == factor*factor ??
    return retval
  def getAnsysStrings(self,eTag,axis, factor= 1e-3):
    ''' Returns a string to represent fields in ANSYS (R).
        eTag: element identifier.
        axis: section 1 or 2
        factor: factor for units (default 1e-3 -> kN)'''
    retval= super(FatigueControlBaseVars,self).getAnsysStrings(eTag,axis,factor)
    retval.append("detab,"+str(eTag)+",Vy" +axis+","+str(self.Vy*factor)+"\n")
    retval.append("detab,"+str(eTag)+",posSteelStress" +axis+","+str(self.posSteelStress*factor*factor)+"\n")
    retval.append("detab,"+str(eTag)+",negSteelStress" +axis+","+str(self.negSteelStress*factor*factor)+"\n")
    retval.append("detab,"+str(eTag)+",concreteStress" +axis+","+str(self.concreteStress*factor*factor)+"\n")
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= super(FatigueControlBaseVars,self).getStrArguments(factor)
    retval+= ',Vy= ' + str(self.Vy*factor)
    retval+= ',negSteelStress= ' + str(self.negSteelStress*factor*factor)
    retval+= ',posSteelStress= ' + str(self.posSteelStress*factor*factor)
    retval+= ',concreteStress= ' + str(self.concreteStress*factor*factor)
    return retval

class FatigueControlVars(ControlVarsBase):
  '''Fatigue limit state control variables.'''
  def __init__(self,idSection= 'nil',state0= FatigueControlBaseVars(), state1= FatigueControlBaseVars()):
    self.idSection= idSection #Reinforced concrete section identifier.
    self.state0= state0 #Under permanent loads.
    self.state1= state1 #Under fatigue loads.
    self.concreteLimitStress= 0.0 #SIA 262(2013)  4.3.8.3.1
    self.concreteBendingCF= -1.0 #Concrete capacity factor.
    self.shearLimit= 0.0 #SIA 262(2013)  4.3.8.3.2
    self.concreteShearCF= -1.0 #Concrete shear factor.
    self.Mu= 0.0 #Ultimate bending moment.
    self.Vu= 0.0 #Ultimate shear.
  def getSteelPosStressIncrement(self):
    '''Returns positive stress increment in rebars.'''
    return self.state1.posSteelStress-self.state0.posSteelStress
  def getSteelNegStressIncrement(self):
    '''Returns negative stress increment in rebars.'''
    return self.state1.negSteelStress-self.state0.negSteelStress
  def getAbsSteelStressIncrement(self):
    '''Returns maximun stress increment in rebars (absolute value).'''
    return max(self.getSteelPosStressIncrement(),-self.getSteelNegStressIncrement())
  def getConcreteStressIncrement(self):
    '''Returns stress increment in concrete.'''
    return self.state1.concreteStress-self.state0.concreteStress
  def getConcreteMaxMinStresses(self):
    '''Used in FatigueController.concreteLimitStress.''' 
    retval= [0.0,0.0]
    retval[0]= min(min(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
    retval[1]= min(max(self.state0.concreteStress,self.state1.concreteStress),0.0) # No positive stresses.
    return retval
  def getConcreteMinStress(self):
    '''Returns minimum (max. compressive) concrete stress between loaded and unloaded states.'''
    sgc0= abs(min(self.state0.concreteStress,0.0))
    sgc1= abs(min(self.state1.concreteStress,0.0))
    return max(sgc0,sgc1)

  def getLaTeXFields(self,factor= 1e-3):
    ''' Returns a string with the intermediate fields of the LaTeX string.
        factor: factor for units (default 1e-3 -> kN)'''
    retval= self.idSection+" & "
    retval+= self.state0.getLaTeXFields()+" & "
    retval+= self.state1.getLaTeXFields()+" & "
    retval+= fmt.Stress.format(self.concreteLimitStress*factor*factor)+" & "
    retval+= fmt.Esf.format(self.concreteBendingCF)+" & "
    retval+= fmt.Esf.format(self.shearLimit*factor)+" & "
    retval+= fmt.Esf.format(self.concreteShearCF)+" & "
    retval+= fmt.Esf.format(self.Mu*factor)+" & "
    retval+= fmt.Esf.format(self.Vu*factor)
    return retval
  def getStrArguments(self,factor):
    '''Returns a string for a 'copy' (kind of) constructor.'''
    retval= 'idSection= "' + self.idSection + '"' 
    retval+= ', controlBaseVars0= ' + self.state0.getStrConstructor(factor)
    retval+= ', controlBaseVars1= ' + self.state1.getStrConstructor(factor)
    retval+= ', concreteLimitStress= ' + str(self.concreteLimitStress*factor*factor)
    retval+= ', concreteBendingCF= ' + str(self.concreteBendingCF)
    retval+= ', shearLimit= ' + str(self.shearLimit*factor)
    retval+= ', concreteShearCF= ' + str(self.concreteShearCF)
    retval+= ', Mu= ' + str(self.Mu*factor)
    retval+= ', Vu= ' + str(self.Vu*factor)
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

def extrapolate_control_var(elemSet,propName,argument,initialValue= 0.0):
  '''Extrapolates element's function values to the nodes.
     elemSet: set of elements.
     propName: name of the property that contains the control variables.
     function: name of the function to call for each element.
     argument: name of the control variable to extrapolate.
     initialValue: initial value for the prop defined at the nodes.
  '''
  print 'propName: ', propName
  print 'argument: ', argument
  nodePropName= propName+'_'+argument
  nodeTags= ext.create_attribute_at_nodes(elemSet,nodePropName,initialValue)
  #Calculate totals.
  for e in elemSet:
    elemNodes= e.getNodes
    sz= len(elemNodes)
    for i in range(0,sz):
      n= elemNodes[i]
      controlVar= e.getProp(propName)
      value= controlVar(argument)
      oldValue= n.getProp(nodePropName)
      n.setProp(nodePropName,oldValue+value)
  #Divide by number of elements in the set that touch the node.
  preprocessor= elemSet.owner.getPreprocessor
  for tag in nodeTags:
    n= preprocessor.getNodeLoader.getNode(tag)
    denom= nodeTags[tag]
    n.setProp(nodePropName,n.getProp(nodePropName)/denom)
  return nodePropName
