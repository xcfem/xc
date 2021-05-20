      SUBROUTINE SKYPDC (DGPNT,INRTIA,JMAX,JMIN,A,D)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYPDC evaluates the inertia of the effective matrix             *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (A)     *
C*    INRTIA (aib) : inertia of the effective matrix                   *
C*    JMAX   (sii) : index of the last  column of (A)                  *
C*    JMIN   (sii) : index of the first column of (A)                  *
C*    A      (arb) : block of the effective matrix                     *
C*    D      (arb) : reciprocal of the coefficients of (D)             *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          DGPNT(*),INRTIA(3),JMAX,JMIN
      DOUBLE PRECISION A(*),D(*)
C
C==== local variables ==================================================
C
      INTEGER          J,NNEG,NPLUS,NULL
      DOUBLE PRECISION DIAG 
C
C**** executable statements ********************************************
C
      NNEG  = 0
      NPLUS = 0
      NULL  = 0
C
      DO 010 J = JMIN,JMAX
         DIAG  = A(DGPNT(J)) 
         IF      ( DIAG .LT. 0.0D0 ) THEN
	         NNEG  = NNEG  + 1
                 D(J) = 1.0D0/DIAG
         ELSE IF ( DIAG .EQ. 0.0D0 ) THEN
		 NULL  = NULL  + 1
                 D(J) = 0.0D0
	 ELSE IF ( DIAG .GT. 0.0D0 ) THEN
		 NPLUS = NPLUS + 1
                 D(J) = 1.0D0/DIAG
	 END IF
         A(DGPNT(J)) = D(J)
  010 CONTINUE
C
      INRTIA(1) = INRTIA(1) + NNEG
      INRTIA(2) = INRTIA(2) + NULL
      INRTIA(3) = INRTIA(3) + NPLUS
C
      RETURN
C
C**** end of SKYPDC ****************************************************
C
      END
