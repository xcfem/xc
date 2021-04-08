# -*- coding: utf-8 -*-

from solution import predefined_solutions

class SuperLU(predefined_solutions.SolutionProcedure):
    ''' Return a linear static solution algorithm
        with a penalty constraint handler and
        a super_lu solver.
    '''
    def __init__(self, prb, name= None, maxNumIter= 1, convergenceTestTol= 1e-9, printFlag= 0, numSteps= 1):
        ''' Constructor.

        :param prb: XC finite element problem.
        :param name: identifier for the solution procedure.
        :param maxNumIter: maximum number of iterations (defauts to 10)
        :param convergenceTestTol: convergence tolerance (defaults to 1e-9)
        :param printFlag: if not zero print convergence results on each step.
        :param numSteps: number of steps to use in the analysis (useful only when loads are variable in time).
        '''
        # Sparse solver doesn't need renumbering.
        super(SuperLU,self).__init__(name, 'penalty', maxNumIter, convergenceTestTol, printFlag, numSteps, 'simple', soeType= 'sparse_gen_col_lin_soe', solverType= 'super_lu_solver')
        self.feProblem= prb
        self.setPenaltyFactors()
        
    def setup(self):
        ''' Defines the solution procedure in the finite element 
            problem object.
        '''
        super(SuperLU,self).setup()
        self.solutionAlgorithmSetup(solAlgType= 'linear_soln_algo', integratorType= 'load_control_integrator')
        self.sysOfEqnSetup()
        self.analysisSetup('static_analysis')

solProc= SuperLU(feProblem)
result= solProc.solve()
