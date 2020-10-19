import os
def salvaCombinacion(prb,nmbComb, tagComb, pth,db):
  fileName= pth+nmbComb+".db"
  os.system("rm -f "+fileName)
  db= prb.newDatabase("BerkeleyDB",fileName)
  db.save(tagSaveFase0)

class DatabaseHelperSolve:
    previousName= ""
    tagPrevia= -1
    db= None
    def __init__(self,db):
        self.previousName= ""
        self.tagPrevia= .1
        self.db= db
    def helpSolve(self,comb):
        previa= comb.getCombPrevia()
        if(previa!=None):
            self.previousName= previa.getName
            self.tagPrevia= previa.tag
            self.restore()  
    def restore(self):
        if(self.tagPrevia>0):
            self.db.restore(self.tagPrevia*100)
    def solveComb(self,comb,solutionProcedure):
        solutionProcedure.resetLoadCase()
        self.helpSolve(comb)
        ''' 
        print("previousName= ",previousName)
        print("tag= ",comb.tag)
        print("tagPrevia= ",tagPrevia)
        print("descomp previa= ",getDescompCombPrevia)
        print("resto sobre previa= ",getDescompRestoSobrePrevia)
        '''
        comb.addToDomain()
        analOk= solutionProcedure.solve()
        self.db.save(comb.tag*100)
        comb.removeFromDomain()
    
