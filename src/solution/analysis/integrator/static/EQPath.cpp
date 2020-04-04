/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// Written: M. Salehi opensees.net@gmail.com
// Created: 02/19
// Revision: A

//refs
//Structural Engineeringand Mechanics   Volume 48, Number 6, December25 2013, pages 849 - 878
//DOI: https://doi.org/10.12989/sem.2013.48.6.849	
//
//Comprehensive evaluation of structural geometrical nonlinear solution techniques Part I : Formulation and characteristics of the methods
//M.Rezaiee - Pajand, M.Ghalishooyan and M.Salehi - Ahmadabad
//FULLTEXT : https://www.researchgate.net/publication/264146397_Comprehensive_evaluation_of_structural_geometrical_nonlinear_solution_techniques_Part_I_Formulation_and_characteristics_of_the_methods


//Structural Engineeringand Mechanics   Volume 48, Number 6, December25 2013, pages 879 - 914
//DOI: https://doi.org/10.12989/sem.2013.48.6.879	
//
//Comprehensive evaluation of structural geometrical nonlinear solution techniques Part II : Comparing efficiencies of the methods
//M.Rezaiee - Pajand, M.Ghalishooyan and M.Salehi - Ahmadabad
//FULLTEXT : https://www.researchgate.net/publication/263361974_Comprehensive_evaluation_of_structural_geometrical_nonlinear_solution_techniques_Part_II_Comparing_efficiencies_of_the_methods

#include "EQPath.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"

XC::EQPath::EQPath(AnalysisAggregation *owr,double arcLen,int method)
  :StaticIntegrator(owr,INTEGRATOR_TAGS_EQPath),
 arclen(arcLen), dl(0), m(1),
 type(method),changed(0),nitr(0),
 du(nullptr),du0(nullptr), uq(nullptr),uq0(nullptr), uqn(nullptr), ur(nullptr),
 q(nullptr)
 {}

XC::EQPath::~EQPath(void)
{
    // delete any vector object created
    if(uq)
      delete uq;
    if(uq0)
      delete uq0;
    if(uqn)
      delete uqn;
    if(ur)
      delete ur;
    if(du)
      delete du;
    if(du0)
      delete du0;
    if(q)
      delete q;
  }

int XC::EQPath::newStep(void)
  {
    // get pointers to AnalysisModel and LinearSOE
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == nullptr || theLinSOE == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING no AnalysisModel or LinearSOE has been set\n";
	return -1;
    }

    // get the current load factor
    double currentLambda = getCurrentModelTime();


    // determine dUhat
    this->formTangent();
    theLinSOE->setB(*q);
    if(theLinSOE->solve() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed in solver\n";
        return -1;
      }

    // for GDC method we need this
    if(uqn==0 && uq0!=0)
      {
	uqn=new Vector(uq0->Size());
	(*uqn)=(*uq0);
      }
    else if(uq0!=0)
      { (*uqn)=(*uq0); }

    uq0=new Vector(du->Size());
    (*uq0) = theLinSOE->getX();

    // determine delta lambda(1) == dlambda
    
    const double a=(*du)^(*uq0);
 
    if(a>=0)
      sign=1;
    else
      sign=-1;
    du->Zero();

    double dLambda = sign*arclen/uq0->Norm();

    (*du)=dLambda*(*uq0);

    du0=new Vector(du->Size());
    (*du0)=(*du);

    currentLambda += dLambda;
    dl+=dLambda;
    
    // update model with delta lambda and delta U
    theModel->incrDisp(*du);    
    applyLoadModel(currentLambda);    
    if(updateModel() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; model failed to update for new dU\n";
        return -1;
      }
    
    //std::cerr << "new step : current dl " << dLambda << "\n" ;

    nitr=0;
    if(m != 1)
	   changed--;
    if(changed == 0)
	   m = 1;

    return 0;
  }

int XC::EQPath::update(const Vector &dU)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == nullptr || theLinSOE == nullptr)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "WARNING; no AnalysisModel or LinearSOE has been set\n";
	return -1;
      }

    nitr++;

    (*ur) = dU;

    theLinSOE->setB(*q);
    theLinSOE->solve();
    (*uq) = theLinSOE->getX();    

    // determine delta lambda(i)
    double dLambda=0;
	   if(type==1) // minimum residual disp
	   {
		  double a = (*ur)^(*uq);
		  double b = (*uq)^(*uq);
		  if(b == 0)
		    {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; zero denominator\n";
			 return -1;
		  }	  
		  dLambda = -a/b;
	   }
	   else if(type==2) // normal plain
	   {
		  double a = (*du0)^(*ur);
		  double b = (*du0)^(*uq);
		  if(b == 0) {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; zero denominator\n";
			 return -1;
		  }

		  dLambda = -a/b;
	   }
	   else if(type==3) // update normal plain
	   {
		  double a = (*du)^(*ur);
		  double b = (*du)^(*uq);
		  if(b == 0) {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; zero denominator\n";
			 return -1;
		  }

		  dLambda = -a/b;
		 
	   }
	   else if(type==4) // cylindrical arc-length
	   {
		  double A = (*uq)^(*uq);
		  double B = 2 * (((*du) + (*ur)) ^ (*uq));
		  double c1=((*du) + (*ur)) ^ (*du);
		  double c2=((*du) + (*ur)) ^(*ur);
		  double C = c1 + c2 - arclen * arclen;
		  double delta = B * B - 4 * A * C;

		  dLambda = 0;
		  if(delta < 0)
		  {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; negative denominator\n";
			 return -1;
		  }
		  else if(delta == 0)
		  {
			 dLambda = -B / 2 / A;
		  }
		  else
		  {
			 double sl1 = (-B + pow(delta,0.5)) / 2 / A;
			 double sl2 = (-B - pow(delta,0.5)) / 2 / A;
			 double aa1=(*du) ^ (*ur);
			 double aa2=(*du) ^ (*du);
			 double aa3=(*du) ^ (*uq);
			 double costl1 = aa1 + aa2 + sl1 * aa3;
			 double costl2 = aa1 + aa2 + sl2 * aa3;
			 dLambda = sl1;
			 if(costl2 > costl1)
				dLambda = sl2;
		  }

	   }
	   else if(type==5) // modified normal flow
	     {
		  const double a = (*ur)^(*uq);
		  const double b = (*uq)^(*uq);
		  if(b == 0)
		    {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; zero denominator\n";
			 return -1;
		    }

		  dLambda = -a/b;
	   }

	   else if(type==6) // GDC
	   {
		  double a,b;
		  if(uqn==0)
		  {
			 // first increment use MRD
			 a = (*ur)^(*uq);
			 b = (*uq)^(*uq);
		  }
		  else
		  {
			 a = (*ur)^(*uqn);
			 b = (*uq)^(*uqn);
		  }
		  
	   
		  if(b == 0) {
		  std::cerr << getClassName() << "::" << __FUNCTION__
			    << "; zero denominator\n";
		  return -1;
		  }
		  dLambda = -a/b;
		  
	   }
	   else if(type==7) // Modified Update Normal Plane
	   {
		  double p1=(*uq)^(*uq);
		  double p2=(*du)^(*uq);
		  double p3=(*ur)^(*uq);
		  double p4=(*ur)^(*du);
		  double p5=(*ur)^(*ur);
		  double dl0=-p3/p1; //Chan constraint


		  double A = p1;//(*uq)^(*uq);
		  double B = p2+2*p3;//(((*du) + (*ur)+ (*ur)) ^ (*uq));
		  double C=p4+p5;//((*du) + (*ur)) ^ (*ur);
		  double delta = B * B - 4 * A * C;
		  //std::cerr << "negative denominator - alpha = " << 0 <<"\n";
		  dLambda = 0;
		  if(delta < 0)
		  {
			 Vector *v1=new Vector(ur->Size());
			 Vector *v2=new Vector(ur->Size());
			 (*v2)=(*ur);
			 v2->addVector(1,*uq,dl0);
			 (*v1)=(*du);
			 v1->addVector(1,*v2,1);
			 double l1=v1->Norm();
			 double l2=v2->Norm();
			 double alpha=(C-B*B/4/A)/l1/l2;
			 alpha+=0.1*(1-alpha);
			 //std::cerr << "negative denominator - alpha = " << alpha <<"\n";
			 delta = B * B - 4 * A * (C-alpha*l1*l2);
			 //dLambda=dl0;
		  }

		  if(delta == 0)
		  {
			 dLambda = -B / 2 / A;
		  }
		  else
		  {
			 double sl1 = (-B + pow(delta,0.5)) / 2 / A;
			 double sl2 = (-B - pow(delta,0.5)) / 2 / A;
			 double aa1=(*du) ^ (*ur);
			 double aa2=(*du) ^ (*du);
			 double aa3=(*du) ^ (*uq);
			 double costl1 = aa1 + aa2 + sl1 * aa3;
			 double costl2 = aa1 + aa2 + sl2 * aa3;
			 dLambda = sl1;
			 if(costl2 > costl1)
				dLambda = sl2;
		  }

	   }
	   else if(type==8) // normal plain
	   {
		  double p1=(*uq)^(*uq);
		  double p2=(*du)^(*uq);
		  double p3=(*ur)^(*uq);
		  if(p1 == 0) {
			 std::cerr << getClassName() << "::" << __FUNCTION__
				   << "; zero denominator\n";
			 return -1;
		  }

		  dLambda = -(p2+p3)/p1;
	   }
	   else if(type==9) // local minimum residual displacement
	     {
	       Domain *theDomain=theModel->getDomainPtr();
	       Element *elePtr;
	       ElementIter &theElemIter = theDomain->getElements();    
	       while ((elePtr = theElemIter()) != 0)
		 {
		    NodePtrsWithIDs nodes= elePtr->getNodePtrs();
		 }
	     }
	   else
	     {
		std::cerr << getClassName() << "::" << __FUNCTION__
			  << ";WARNING unknown update method has been set\n";
		return -1;
	     }



    Vector *sd=new Vector(ur->Size());
    // determine delta U(i)
    (*sd) = (*ur);
    sd->addVector(1.0, *uq,dLambda);
    

    if(type==5)
    {
	   double fac = -((*sd)^(*uq))/((*uq0)^(*uq0));	  
	   sd->addVector(1.0,*uq,fac);
    }

    (*du) += (*sd);
    dl += dLambda;

    double currentLambda = getCurrentModelTime();
    currentLambda+=dLambda;
    // update the model
    theModel->incrDisp(*sd);    
    applyLoadModel(currentLambda);    

    if(updateModel() < 0)
      {
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; model failed to update for new dU\n";
      return -1;
    }
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(*sd);

    
    return 0;
}

int XC::EQPath::domainChanged(void)
  {
    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == nullptr || theLinSOE == nullptr)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no analysis model or linear"
		  << " system of equations has been set."
		  << std::endl;
	    exit(-1);
      }
    const int size= theModel->getNumEqn(); // ask model in case N+1 space

    if (uq == 0 || uq->Size() != size)
      { // create new Vector
	if (uq != nullptr)
	    delete uq;   // delete the old
	uq = new Vector(size);
	if (uq == 0 || uq->Size() != size)
	  { // check got it
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; FATAL ran out of memory for"
		      << " uq Vector of size "
		      << size << std::endl;
	    exit(-1);
	}
    }
    if(du == nullptr || du->Size() != size)
      { // create new Vector
	if(du != 0)
	    delete du;   // delete the old
	du = new Vector(size);
	if(du == nullptr || du->Size() != size)
	  { // check got it
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL ran out of memory for"
		      << " du Vector of size " << size << std::endl;
	    exit(-1);
	  }
    }

    
    if(ur == nullptr || ur->Size() != size) { // create new Vector
	if(ur != 0)
	    delete ur;   // delete the old
	ur = new Vector(size);
	if(ur == nullptr || ur->Size() != size)
	  { // check got it
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL ran out of memory for"
		      << " deltaU Vector of size " << size << std::endl;
	    exit(-1);
	  }
    }

    if(q == nullptr || q->Size() != size) { 
	if(q != 0)
	    delete q;  
	q = new Vector(size);
	if(q == nullptr || q->Size() != size)
	  { 
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL ran out of memory for"
		      << " q Vector of size " << size << std::endl;
	    exit(-1);
	  }
    }

    // now we have to determine phat
    // do this by incrementing lambda by 1, applying load
    // and getting phat from unbalance.
    double currentLambda = getCurrentModelTime();
    currentLambda += 1.0;
    applyLoadModel(currentLambda);    
    this->formUnbalance(); // NOTE: this assumes unbalance at last was 0
    (*q) = theLinSOE->getB();
    currentLambda -= 1.0;
    setCurrentModelTime(currentLambda);    


    // check there is a reference load
    int haveLoad = 0;
    for(int i=0; i<size; i++)
      if( (*q)(i) != 0.0 )
	{
	  haveLoad = 1;
	  i = size;
        }

    if(haveLoad == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	  	  << "; WARNING zero reference load";
        return -1;
      }
    return 0;
  }

int XC::EQPath::sendData(Communicator &comm)
  {
    int res= StaticIntegrator::sendData(comm);
    res+= comm.sendDoubles(arclen,dl,sign,getDbTagData(),CommMetaData(1));
    return res;
  }


int XC::EQPath::recvData(const Communicator &comm)
  {
    int res= StaticIntegrator::recvData(comm);
    res+= comm.receiveDoubles(arclen,dl,sign,getDbTagData(),CommMetaData(1));
    return res;
  }

int XC::EQPath::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }


int XC::EQPath::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

void XC::EQPath::Print(std::ostream &s, int flag) const
  {
    const AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theModel)
      {
	const double cLambda = getCurrentModelTime();
	s << "\t EQPath - currentLambda: " << cLambda <<"\n";
	s << "\t EQPath - arcLength: " << arclen <<"\n";
	s << "\t EQPath - sign: " << sign <<"\n";
      }
    else 
	s << "\t EQPath - no associated AnalysisModel\n";
  }
