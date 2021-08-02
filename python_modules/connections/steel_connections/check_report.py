# -*- coding: utf-8 -*-

from __future__ import print_function
from __future__ import division

import os
import xc_base
import geom
import xc
import math
from postprocess import output_handler
from solution import predefined_solutions

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2020, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es l.pereztato@gmail.com"

# Functions to check bolts and welds according to AISC-16
def aisc_check_bolts_welds(modelSpace,ULSs,boltSets2Check=[],welds2Check=[],baseMetal=None,meanShearProc=True,resFile=None, solutionProcedureType= predefined_solutions.SimpleStaticLinearUMF,warningsFile=None,Phi=0.75):
    '''Verification of bolts and welds according to AISC-16
    Checking of bolts uses the capacity factor # formula proposed by Rugarli
    https://www.steelchecks.com/CONNECTIONS-GUIDE/index.html?check_welds.htm
    Parasitic moments in the bolt shafts are neglected.

    :param ULSs: list of names of existing load cases to be analyzed
    :param boltSets2Check: list of pairs (bolt set, bolt type) with the
          set of bolts and bolt material (instance of class
          astm.BoltFastener) to be analyzed. (Defaults to [])
    :param welds2Check: welds (instances of classes FilletWeld or multiFilletWeld) to check
    :param baseMetal: steel of the plate (only used when welds are checked). Defaults to None
    :paran meanShearProc: if True, each bolt is verified using the mean shear of 
                          the set of bolts (defaults to True)
    :param resFile: file to which dump the results (path and name without extension)
                    (if None-> print to terminal)
    :param solutionProcedureType: solution procedure type (defaults to SimpleStaticLinearUMF).
    :param warningsFile: name of the file of warnings (defaults to None)
    :param Phi: resistance factor (defaults to 0.75)
    '''
    #Initialize properties
    init_prop_checking_bolts(boltSets2Check)
    singlWelds=init_prop_checking_welds(welds2Check)
    # Calculation and checking
    modelSpace.solutionProcedureType=  solutionProcedureType
    for ULS in ULSs:
        ULS=str(ULS)
        modelSpace.removeAllLoadPatternsFromDomain()
        modelSpace.revertToStart()
        modelSpace.addLoadCaseToDomain(ULS)
        result= modelSpace.analyze(calculateNodalReactions= True, reactionCheckTolerance= 1e-4)
        oh= output_handler.OutputHandler(modelSpace)
        #bolts checking
        set_bolt_check_resprop_current_LC(ULS,boltSets2Check,meanShearProc)
        #welds checking
        set_welds_check_resprop_current_LC(ULS,singlWelds,baseMetal,Phi)
    #print results of bolt checking
    if len(boltSets2Check)>0:
        Lres=print_bolt_results(boltSets2Check)
        if resFile:
            f=open(resFile+'_bolts.tex','w')
            f.writelines(Lres)
            f.close()
        else:
            for l in Lres: print(l)
    #print results of weld checking
    if len(welds2Check)>0:
        Lres=print_weld_results(singlWelds)
        if resFile:
            f=open(resFile+'_welds.tex','w')
            f.writelines(Lres)
            f.close()
        else:
           for l in Lres: print(l)
    if warningsFile:
        write_check_warnings(warningsFile,boltSets2Check,singlWelds)

def print_bolt_results(boltSets2Check):
    '''return a list with the results

    :param singlWelds: list of welds and results generated during 
          weld checking
    '''
    retval=list()
    for checkIt in  boltSets2Check:
        bset=checkIt[0]
        btype=checkIt[1] 
        CFmax=0
        for e in bset.elements:
            CF=e.getProp('CF')
            if CF>CFmax: CFmax=CF
        retval.append('Bolt set: ' + bset.description + ' diameter=' + str(round(btype.diameter,3)) + ' CF=' + str(round(CFmax,2))+'\n')
    return retval

def print_weld_results(singlWelds):
    '''return a list with the results

    :param singlWelds: list of welds and results generated during 
          weld checking
    '''
    retval=list()
    for w in  singlWelds:
        weld=w[0]
        par=w[1]
        retval.append('Weld: ' + weld.descr+ ' minSz=' + str(round(weld.minSz*1e3,1)) + ' maxSz=' + str(round(weld.maxSz*1e3,1))  + ' size='+ str(round(weld.weldSz*1e3,1)) + ' CF=' + str(round(par[1],2)) + '\n')
    return retval
        
def write_check_warnings(warningsFile,boltSets2Check,singlWelds):
    f=open(warningsFile,'w')
    for checkIt in  boltSets2Check:
        bset=checkIt[0]
        CFmax=0
        for e in bset.elements:
            CF=e.getProp('CF')
            if CF>CFmax: CFmax=CF
        if CFmax >1:
            btype=checkIt[1] 
            f.write('Bolt set: '  + bset.description + ' with diameter=' + str(round(btype.diameter,3)) + ' has a CF=' + str(round(CFmax,2))  + ' > 1 \n \n') 
    for sw in singlWelds:
        w=sw[0]
        par=sw[1]
        txtW='Weld: '+ w.descr + ' between plates with t1= ' + str(w.tWS1) + ' and t2=' + str(w.tWS2) + ' szMin='+ str(w.minSz) + ' szMax=' + str(w.maxSz)
        if w.weldSz < w.minSz:
            f.write(txtW + ' has a size=' + str(w.weldSz) + ' less than minimum=' + str(w.minSz) +'\n \n')
        if w.weldSz > w.maxSz:
            f.write(txtW + ' has a size=' + str(w.weldSz) + ' greater than maximum=' + str(round(w.maxSz,3)) +'\n \n')
        if par[1] > 1:
            f.write(txtW + ' with size=' + str(w.weldSz) + ' has a CF=' +  str(round(par[1],2)) +' > 1 minSZ=' + str(round(w.minSz,3)) +' maxSz=' + str(round(w.maxSz,4)) +'\n \n')
    f.close()
                      
        
    
def init_prop_checking_bolts(boltSets2Check):
    '''Initialize properties of bolt elements for checking''' 
    for checkIt in  boltSets2Check:
        bset=checkIt[0]
        for e in bset.elements:
            e.setProp('CF',0.0);e.setProp('LS','');e.setProp('N',0.0);e.setProp('V',0.0)
    
def init_prop_checking_welds(welds2Check):
    '''Initialize properties of weld elements for checking
    Return a list of single welds
    ''' 
    singlWelds=list()
    for checkW in welds2Check:
        LS='' ; CF=0
        if not hasattr(checkW,'lstWelds'):
            singlWelds.append([checkW,[CF]])
        else:
            for w in checkW.lstWelds: singlWelds.append([w,[LS,CF]])
    return singlWelds

def set_bolt_check_resprop_current_LC(ULS,boltSets2Check,meanShearProc):
    '''Set, for each bolt,  the capacity factor, and internal forces resulting
    for the current ULS
    '''
    for checkIt in  boltSets2Check:
        bset=checkIt[0]
        btyp=checkIt[1]
        Fdt=btyp.getDesignTensileStrength()
        Fdv=btyp.getDesignShearStrength()
        # mean of shear internal forces in the set of bolts
        if meanShearProc:
            Vmean=0
            for e in bset.elements:
                Vmean+=math.sqrt((e.getVy())**2+(e.getVz())**2)
            Vmean=Vmean/bset.elements.size
        for e in bset.elements:
            N=e.getN()
            V=Vmean if meanShearProc else math.sqrt((e.getVy())**2+(e.getVz())**2)
            eN= max(N/Fdt,0)
            eV= V/Fdv
            eMax= max(abs(eN),abs(eV))
            if(eMax<1e-6):
                CF= eMax
            else:
                CF=math.sqrt((2*eN**4+4*eN**2*eV**2+2*eV**4)/(2*eV**2+2*eN**2))
            if CF>e.getProp('CF'):
                e.setProp('CF',CF);e.setProp('LS',ULS);e.setProp('N',N);e.setProp('V',V)

def set_welds_check_resprop_current_LC(ULS,singlWelds,baseMetal,Phi=0.75):
    for i in range(len(singlWelds)):
        weld=singlWelds[i][0]
        CFinit=singlWelds[i][1][-1]
        CF=weld.getCF_AISCverif(baseMetal,Phi)
        if CF>CFinit:
            LS=ULS
#            Rd=0.75*weld.getWeldDesignStrength(Fpar,Fperp)
            singlWelds[i][1]=[LS,CF]

# Funtions to generate weld and bolt calculation-reports
def gen_report_files(modelSpace,genDescr,specDescr,loadCaseNames,reportPath,rltvResPath,grWidth,texfileNm,boltSets2Check=[],welds2Check=[],baseMetal=None,meanShearProc=True,genGrULSs=True, solutionProcedureType= predefined_solutions.SimpleStaticLinearUMF,warningsFile=None,Phi=0.75):
    '''Generates the graphics corresponding to loads and displacements for each load case,
    together with the tex file to include them in a report.

    :param genDescr: general description
    :param specDescr: specific description
    :param loadCaseNames: list of load case names
    :param reportPath: directory where report is to be generated
    :param rltvResPath: directory where to place the text file relative to reportPath
    :param grWidth: with to insert the graphics
    :param texfileNm: name of the tex file.
    :param boltSets2Check: list of pairs (bolt set, bolt type) with the
          set of bolts and bolt material (instance of class
          astm.BoltFastener) to be analyzed. (Defaults to [])
    :param welds2Check: welds (instances of classes FilletWeld or multiFilletWeld) to check
    :param baseMetal: steel of the plate (only used when welds are checked). Defaults to None
    :param meanShearProc: if True, each bolt is verified using the mean shear of 
                          the set of bolts (defaults to True)
    :param genGrULSs: generate graphics of loads and displacements for all
                      the load cases. (Defaults to True)
    :param solutionProcedureType: solution procedure type (defaults to SimpleStaticLinearUMF)..
    :param warningsFile: name of the file of warnings (defaults to None)
    :param Phi: resistance factor (defaults to 0.75)
    '''
    oh= output_handler.OutputHandler(modelSpace)
    resPath=reportPath+rltvResPath
    if not os.path.isdir(resPath): os.mkdir(resPath)
    grPath=resPath+'graphics/'
    if not os.path.isdir(grPath): os.mkdir(grPath)
    texPath=reportPath+rltvResPath ; grPath=texPath+'graphics/'; rltvGrPath=rltvResPath+'graphics/'
    # Initialize properties of bolts and welds
    init_prop_checking_bolts(boltSets2Check)
    singlWelds=init_prop_checking_welds(welds2Check)
    # load cases
    lcNm=[str(ULS) for ULS in loadCaseNames]
    lcNm.sort()
    if genGrULSs: f=open(texPath+texfileNm+'_load_disp.tex','w')
    cont=0
    modelSpace.solutionProcedureType=  solutionProcedureType
    for ULS in lcNm:
        txtDescr=genDescr+' '+specDescr+' '+ULS + ': '
        modelSpace.removeAllLoadPatternsFromDomain()
        modelSpace.revertToStart()
        modelSpace.addLoadCaseToDomain(ULS)
        result= modelSpace.analyze(calculateNodalReactions= True)
        if genGrULSs:
            #loads
            captFig=specDescr+' '+ ULS + ': loads [kN]'
            graphNm=texfileNm+'_'+ULS+'_loads'
            oh.displayLoads(caption=captFig,fileName=grPath+graphNm+'.jpg')
            captTxt=txtDescr+ 'loads [kN]'
            addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
            #displacements
            graphNm=texfileNm+'_'+ULS+'_uX'
            oh.displayDispRot(itemToDisp='uX',fileName=grPath+graphNm+'.jpg')
            captTxt=txtDescr+ 'displacement in global X direction [mm]'
            addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
            graphNm=texfileNm+'_'+ULS+'_uY'
            oh.displayDispRot(itemToDisp='uY',fileName=grPath+graphNm+'.jpg')
            captTxt=txtDescr+ 'displacement in global Y direction [mm]'
            addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
            graphNm=texfileNm+'_'+ULS+'_uZ'
            oh.displayDispRot(itemToDisp='uZ',fileName=grPath+graphNm+'.jpg')
            captTxt=txtDescr+ 'displacement in global Z direction [mm]'
            addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
            cont+=3
            if cont>20:
                f.write('\\clearpage \n')
                cont=0
        #Bolts checking
        set_bolt_check_resprop_current_LC(ULS,boltSets2Check,meanShearProc)
        #Welds checking
        set_welds_check_resprop_current_LC(ULS,singlWelds,baseMetal,Phi)
    if genGrULSs: f.close()
    #generate graphics of internal forces in welds
    cont=0
    for w in  singlWelds:
        weld=w[0]
        par=w[1]
        ULS=par[0]
        modelSpace.removeAllLoadPatternsFromDomain()
        modelSpace.addLoadCaseToDomain(ULS)
        result= modelSpace.analyze(calculateNodalReactions= True)
        graphNm=texfileNm+'weld'+str(cont)
        w[1].append(rltvGrPath+graphNm)
        oh.displayIntForcDiag('N',weld.weldSet,fileName=grPath+graphNm+'_N.jpg')
        oh.displayIntForcDiag('Vy',weld.weldSet,fileName=grPath+graphNm+'_Vy.jpg')
        oh.displayIntForcDiag('Vz',weld.weldSet,fileName=grPath+graphNm+'_Vz.jpg')
        cont+=1
    #print results of bolt checking
    if len(boltSets2Check)>0:
        Lres=print_bolt_results(boltSets2Check)
        f=open(texPath+texfileNm+'_bolts.tex','w')
        f.writelines(Lres)
        f.close()
    #print results of weld checking
    if len(welds2Check)>0:
        Lres=print_welds_results(singlWelds)
        f=open(texPath+texfileNm+'_welds.tex','w')
        f.writelines(Lres)
        f.close()
    if warningsFile:
        write_check_warnings(warningsFile,boltSets2Check,singlWelds)

def addGraph2Tex(texFile,graph2add,caption,grWidth):
    lhead=['\\begin{figure}[h] \n','\\begin{center} \n']
    texFile.writelines(lhead)
    texFile.write('\\includegraphics[width='+grWidth+']{'+graph2add+'} \n')
    texFile.write('\\caption{'+caption+'} \n')
    lfoot=['\\end{center} \n','\\end{figure} \n','\n']
    texFile.writelines(lfoot)

    
def print_bolt_results(boltSets2Check):
    '''return a list with the results to print in a latex file

    :param boltSets2Check: list of pairs (bolt set, bolt type) with the
          set of bolts and bolt material (instance of class
          astm.BoltFastener) to print results.
    '''
    retval=list()
    retval.append('\\begin{center} \n')
    retval.append('\\begin{longtable}{|c|c|c|c|c|} \n')
    retval.append('\\endfirsthead \n')
    retval.append('\\multicolumn{5}{l}{\\textit{\\ldots(continued from previous page)}} \\\\ \n')
    retval.append('\\hline \n')
    retval.append('\\endhead \n')
    retval.append('\\hline \n')
    retval.append('\\multicolumn{5}{r}{\\textit{(continued on next page)\\ldots }} \\\\ \n')
    retval.append('\\endfoot \n')
    retval.append('\\endlastfoot \n')
    for checkIt in  boltSets2Check:
        bset=checkIt[0]
        btype=checkIt[1]  #bolt type
        retval.append('\\hline \n')
        retval.append('\\multicolumn{5}{|l|}{\\textsc{Bolt set: ' + bset.description +'}} \\\\ \n')
        retval.append('\\multicolumn{1}{|l}{bolt steel = ' + btype.steelType.name + '} & \\multicolumn{2}{l}{bolt diam. = ' + str(btype.diameter*1e3) +' mm} & \\multicolumn{2}{l|}{bolt area = ' + str(round(btype.getArea()*1e4,2)) +  ' $cm^2$}  \\\\  \n')
        retval.append('\\multicolumn{1}{|l}{\\small{nominal tensile and shear strength:}} & \\multicolumn{2}{l}{$F_{nt}$ = ' + str(round(btype.getNominalTensileStrength()*1e-3,2)) + ' kN/bolt} & \\multicolumn{2}{l|}{$F_{nv}$ = ' + str(round(btype.getNominalShearStrength()*1e-3,2)) + ' kN/bolt}  \\\\  \n')
        retval.append('\\multicolumn{1}{|l}{\\small{design tensile and shear strength:}} & \\multicolumn{2}{l}{$\Phi F_{nt}$ = ' + str(round(btype.getDesignTensileStrength()*1e-3,2)) + ' kN/bolt} & \\multicolumn{2}{l|}{$\Phi F_{nv}$ = ' + str(round(btype.getDesignShearStrength()*1e-3,2)) + ' kN/bolt}  \\\\  \n')
        retval.append('\\hline \n')
        retval.append('\\textbf{Bolt coord.} [m] & \\textbf{ULS} & \\textbf{N} [kN] & \\textbf{V} [kN] & \\textbf{CF} \\\\ \n')
        retval.append('\\hline \n')
        CFmax=0
        for e in bset.elements:
            CF=e.getProp('CF')
            if CF>CFmax: CFmax=CF
            eCoord=e.getCooCentroid(True)
            retval.append('(' +  str(round(eCoord[0],3)) +  ',' + str(round(eCoord[1],3)) +  ',' + str(round(eCoord[2],3)) +  ') & ' +  str(e.getProp('LS')) +  ' & ' +  str(round((e.getProp('N')*1e-3),2)) +  ' & ' +  str(round((e.getProp('V')*1e-3),2)) + ' & ' +  str(round(CF,3))  +  '\\\\ \n')
        retval.append('\\hline \n')
        retval.append('\\multicolumn{4}{|r|}{Maximum capacity factor:} & ' +  str(round(CFmax,3))  +  '\\\\ \n')
        retval.append('\\hline \n')
    retval.append('\\end{longtable} \n')
    retval.append('\\end{center} \n')
    return retval

def print_welds_results(singlWelds):
    '''return a list with the results to print in a latex file

    :param singlWelds: list of welds and results generated during 
          weld checking
    '''
    retval=list()
    retval.append('\\begin{center} \n')
    retval.append('\\begin{longtable}{|c|c|c|c|c|c|} \n')
    retval.append('\\hline \n')
    retval.append('\\endfirsthead \n')
    retval.append('\\multicolumn{6}{l}{\\textit{\\ldots(continued from previous page)}} \\\\ \n')
    retval.append('\\hline \n')
    retval.append('\\endhead \n')
    retval.append('\\hline \n')
    retval.append('\\multicolumn{6}{r}{\\textit{(continued on next page)\\ldots }} \\\\ \n')
    retval.append('\\endfoot \n')
    retval.append('\\endlastfoot \n')
    for w in  singlWelds:
        weld=w[0]
        par=w[1]
        retval.append('\\hline \n')
        retval.append('\\multicolumn{6}{|l|}{\\textsc{Weld: ' + weld.descr + '}} \\\\ \n')
        retval.append('\\multicolumn{6}{|l|}{filler metal: ' + weld.weldMetal.name + '}\\\\ \n')
        retval.append('\\multicolumn{1}{|l}{weld size limits:} & \\multicolumn{2}{l}{$t_{plate1}$=' + str(round(weld.tWS1*1e3,1)) + ' mm} &   \\multicolumn{3}{l|}{$t_{plate2}$=' + str(round(weld.tWS2*1e3,1)) + ' mm } \\\\ \n')
        retval.append('\\multicolumn{1}{|l}{} &  \\multicolumn{2}{l}{$w_{min}$=' + str(round(weld.minSz*1e3,1)) + ' mm} &   \\multicolumn{3}{l|}{$w_{max}$=' + str(round(weld.maxSz*1e3,1)) + ' mm}  \\\\ \n')
        retval.append('\\multicolumn{1}{|l}{weld size: \\textbf{w=' + str(round(weld.weldSz*1e3,1)) +  ' mm}} & \\multicolumn{2}{l}{actual throat: a=' + str(round(weld.weldThroat*1e3,1)) + ' mm} & \\multicolumn{3}{l|}{weld length: L=' + str(round(weld.length*1e3,0)) + ' mm} \\\\ \n')  
        retval.append('\\hline \n')
#        retval.append( '\\textbf{Extr. point coord. [m]} & \\textbf{ULS} & \\textbf{$F_{par}$} [kN] &  \\textbf{$F_{perp}$} [kN]&   \\textbf{$\Phi R_n$} [kN]&   \\textbf{CF} \\\\ \n')
#        retval.append('\\hline \n')
#        retval.append( '(' + str(round(weld.weldP1[0],3)) + ',' + str(round(weld.weldP1[1],3)) + ',' + str(round(weld.weldP1[2],3)) + ')  & ' +  par[0]  +  ' & ' +  str(round(par[1]*1e-3,2))  +  ' & ' +  str(round(par[2]*1e-3,2))  +  ' & ' +  str(round(par[3]*1e-3,2))  +  ' & ' + str(round(par[-1],3)) +  '\\\\ \n')
#        retval.append( '(' + str(round(weld.weldP2[0],3)) + ',' + str(round(weld.weldP2[1],3)) + ',' + str(round(weld.weldP2[2],3)) + ')  & & & & & \\\\ \n')
        retval.append('\\multicolumn{6}{|l|}{CF=' + str(round(par[1],2)) + '   in load case:' + par[0]+'} \\\\')
        retval.append('\\hline \n')
        retval.append('\\multicolumn{3}{|c|}{\includegraphics[height=50mm]{'+par[2]+'_N}} & \\multicolumn{3}{|c|}{\includegraphics[height=50mm]{'+ par[2]+'_Vy}} \\\\')
        retval.append('\\hline \n')
    retval.append('\\end{longtable} \n')
    retval.append('\\end{center} \n')
    return retval



def display_weld_results(modelSpace,welds2Check,ULS,set2displ=None):
    '''Calculate limit state ULS and display internal forces
    in welds. Print the total internal forces for each weld seam.
    set2displ is the set to display displacements (defaults to None)
    '''
    singlWelds=init_prop_checking_welds(welds2Check)
    lstWEldSets=[w[0].weldSet for w in singlWelds]
    oh= output_handler.OutputHandler(modelSpace)
    # weldsSet=modelSpace.setSum('weldsSet',lstWEldSets)
    # oh.displayFEMesh(weldsSet)
    modelSpace.removeAllLoadPatternsFromDomain()
    modelSpace.addLoadCaseToDomain(ULS)
    modelSpace.analyze(calculateNodalReactions=True)
    for w in singlWelds:
        descr=w[0].descr
        st=w[0].weldSet
        if set2displ:
            oh.displayDispRot('uX',set2displ)
            oh.displayDispRot('uY',set2displ)
            oh.displayDispRot('uZ',set2displ)
        oh.displayIntForcDiag('N',st)
        oh.displayIntForcDiag('Vy',st)
        oh.displayIntForcDiag('Vz',st)
        N=0
        Vper=0
        Vpar=0
        for e in st.elements:
            N+=e.getN()
            Vper+=e.getVy()
            Vpar+=e.getVz()
        print(descr,' N=',round(N*1e-3),' Vper=',round(Vper*1e-3), ' Vpar=',round(Vpar*1e-3))

from postprocess.xcVtk import vtk_graphic_base
from postprocess import limit_state_data as lsd
from postprocess.control_vars import VonMisesControlVars

def gen_vonmises_results(sets2disp,modelSpace,genDescr,specDescr,reportPath,rltvResPath,grWidth,texfileNm,resVMfile='./tmp_results/verifications/verifRsl_VonMisesStressULS.py'):
    '''Generates the graphics corresponding to loads and displacements for each load case,
    together with the tex file to include them in a report.

    :param genDescr: general description
    :param specDescr: specific description
    :param loadCaseNames: list of load case names
    :param reportPath: directory where report is to be generated
    :param rltvResPath: directory where to place the text file relative to reportPath
    :param grWidth: with to insert the graphics
    :param texfileNm: name of the tex file.
    :param resVMfile: file containing results from Von Mises verification (defaults 
                      to './tmp_results/verifications/verifRsl_VonMisesStressULS.py')
    '''
    preprocessor=modelSpace.preprocessor
    limitState= lsd.vonMisesStressResistance
    exec(open(resVMfile).read())
    oh= output_handler.OutputHandler(modelSpace)
    resPath=reportPath+rltvResPath
    if not os.path.isdir(resPath): os.mkdir(resPath)
    grPath=resPath+'graphics/'
    if not os.path.isdir(grPath): os.mkdir(grPath)
    texPath=reportPath+rltvResPath ; grPath=texPath+'graphics/'; rltvGrPath=rltvResPath+'graphics/'
    f=open(texPath+texfileNm+'_vonmises.tex','w')
    camD1=vtk_graphic_base.CameraParameters('+X+Y+Z')  #camera dihedral 1
    camD3=vtk_graphic_base.CameraParameters('-X-Y+Z')   #camera dihedral 3
    txtDescr='ULS Von Mises stress check. ' + genDescr+' '+specDescr + ' '
    for st in sets2disp:
        setdescr=st.name.replace('Set',' ')
        setdescr=setdescr.replace('Gusset',' '+ 'gusset')
        setdescr=setdescr.capitalize()
        oh.setCameraParameters(camD1)
        graphNm=texfileNm+'_vonmisesStress'+st.name
        captTxt=txtDescr+ setdescr + '. Envelope of Von Misses stress (MPa)'
        oh.displayField(limitStateLabel= limitState.label, section= None, argument='vm_stress', component= None, setToDisplay= st, fileName= grPath+graphNm+'.jpg')
        addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
        graphNm=texfileNm+'_vonmisesCF'+st.name
        captTxt=txtDescr+ setdescr + '. Efficiency'
        oh.displayField(limitStateLabel= limitState.label, section= None, argument='CF', component= None, setToDisplay= st, fileName= grPath+graphNm+'.jpg')
        addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
    if 'gusset' in st.name.lower():
        oh.setCameraParameters(camD3)
        graphNm=texfileNm+'_vonmisesStress_view2'+st.name
        captTxt=txtDescr+ setdescr + '. Envelope of Von Misses stress (MPa)'
        oh.displayField(limitStateLabel= limitState.label, section= None, argument='vm_stress', component= None, setToDisplay= st, fileName= grPath+graphNm+'.jpg')
        addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
        graphNm=texfileNm+'_vonmisesCF_view2'+st.name
        captTxt=txtDescr+ setdescr + '. Efficiency'
        oh.displayField(limitStateLabel= limitState.label, section= None, argument='CF', component= None, setToDisplay= st, fileName= grPath+graphNm+'.jpg')
        addGraph2Tex(f,rltvGrPath+graphNm,captTxt,grWidth)
    f.close()
