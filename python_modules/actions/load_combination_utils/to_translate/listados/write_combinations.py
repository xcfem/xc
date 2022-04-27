# -*- coding: utf-8 -*-

import yaml

def writeULSCombinations(fileName, latexFileName, nmbFormat):
    ''' Write ULS combinations in a LaTeX file.

    :param latexFileName: LaTeX file name.
    '''
    ulsCombDict= dict()
    ulsCombDict['comb_elu_persistentes']= combinations.comb_elu_persistentes.getDict()
    ulsCombDict['comb_elu_accidentales']= combinations.comb_elu_accidentales.getDict()
    
    
    with open('output.yaml', 'w') as def_hip_elu:
        outputs= yaml.dump(ulsCombDict, def_hip_elu)
    def_hip_elu.close()
    
    hip_elu_tex= open(latexFileName,"w")
    printCabeceraListaHipotesis("hip_elu_tex")
    for conta, comb in enumerate(combinations.comb_elu_persistentes):
        hip_elu_tex.write(format(int(conta),nmbFormat)," & ",comb.getName,"\\\\\n")
    for conta, comb in enumerate(combinations.comb_elu_accidentales):
        hip_elu_tex.write(format(int(conta),nmbFormat)," & ",comb.getName,"\\\\\n")
    hip_elu_tex.write("\\end{supertabular}\n")
    hip_elu_tex.write("\\end{center}\n")
    hip_elu_tex.close()


def writeFrequentSLSCombinations(fileName, latexFileName, nmbFormat):
    ''' Write SLS combinations in a LaTeX file.

    :param latexFileName: LaTeX file name.
    '''
    ulsCombDict= dict()
    ulsCombDict['comb_sls_frecuentes']= combinations.comb_sls_frecuentes.getDict()
    
    
    with open('output.yaml', 'w') as def_hip_sls:
        outputs= yaml.dump(ulsCombDict, def_hip_sls)
    def_hip_sls.close()

    hip_sls_tex= open(latexFileName,"w")
    printCabeceraListaHipotesis("hip_sls_tex")
    for conta, comb in enumerate(combinations.comb_sls_frecuentes):
        hip_sls_tex.write(format(int(conta),nmbFormat)," & ",comb.getName,"\\\\\n")
    hip_sls_tex.write("\\end{supertabular}\n")
    hip_sls_tex.write("\\end{center}\n")
    hip_sls_tex.close()
