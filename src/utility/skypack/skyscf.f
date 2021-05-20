      SUBROUTINE SKYSCF (N,DGPNT,BLOCK,NBLOCK,FANAME,FAUNIT,FFNAME,
     &                   FFUNIT,FBNAME,FBUNIT,MBTYPE,A,B,
     &                   D,SIGMA,INRTIA,INFO)
C
C***********************************************************************
C*                                                                     *
C*  - PURPOSE :                                                        *
C*    SKYSCF performs the factorization [(A)+sigma*(B)]=(L)*(D)*(L')   *
C*    in the standard fashion (Level 1 BLAS), being (A) stored in a    *
C*    skyline way and being (B) diagonal or having the same pattern    *
C*    of (A)                                                           *
C*                                                                     *
C*  - ARGUMENTS :                                                      *
C*    N      (sii) : dimension of the matrix (A)                       *
C*    DGPNT  (aii) : addresses of the diagonal coefficients of (A)     *
C*    BLOCK  (aii) : blocking information                              *
C*    NBLOCK (sii) : number of blocks                                  *
C*    FANAME (ssi) : file name associated with (A)                     *
C*    FAUNIT (sii) : file unit associated with (A), which is ignored   *
C*                   if FANAME is equal to "INCORE" or "incore"        *
C*    FFNAME (ssi) : file name associated with (D) and (L')            *
C*    FFUNIT (sii) : file unit associated with (D) and (L'), which     *
C*                   is ignored if FFNAME is equal to "INCORE"         *
C*                   or "incore"                                       *
C*    FBNAME (ssi) : file name associated with (B)                     *
C*    FBUNIT (sii) : file unit associated with (B), which is ignored   *
C*                   if FBNAME is equal to "INCORE" or "incore"        *
C*    MBTYPE (sii) : type of (B)                                       *
C*                   = 1, (B) is diagonal                              *
C*                   = 2, (B) is skyline (upper triangle)              *
C*    A      (arb) : the coefficients of the matrix (A), which are     *
C*                   overwritten by (L) and the reciprocals of (D)     *
C*                   and possibly partitioned into NBLOCK blocks       *
C*                   stored in the binary file specified by FANAME     *
C*    B      (arw) : the coefficients of the matrix (B)                *
C*                 - if MTYPE=1 (B) can be read from the binary file   *
C*                   specified by FBNAME, one record with N words      *
C*                 - if MTYPE=2 (B) can be read from the binary file   *
C*                   specified by FBNAME, NBLOCK records with the      *
C*                   number of words defined by DGPNT and BLOCK        *
C*                 - if NBLOCK>1 (B) is also used in SKYSF1 to store   *
C*                   blocks (one at a time) previous factorized        *
C*    D      (arb) : reciprocal of the coefficients of (D)             *
C*    SIGMA  (sri) : the scalar that multiplies (B)                    *
C*    INFO   (sio) : information on the execution of SKYSCF            *
C*                   = 0, normal exit                                  *
C*                   = 1, IO error                                     *
C*                   = 2, the file specified by FANAME does not exist  *
C*                   = 3, the file specified by FBNAME does not exist  *
C*                                                                     *
C*  - SUBPROGRAMS :                                                    *
C*    SKYMEF,SKYMRD,SKYMWR,SKYSDC                                      *
C*                                                                     *
C***********************************************************************
C
C==== arguments ========================================================
C
      INTEGER          BLOCK(3,*),DGPNT(*),FAUNIT,FBUNIT,FFUNIT,
     &                 INFO,INRTIA(3),MBTYPE,N,NBLOCK
      DOUBLE PRECISION A(*),B(*),D(*),SIGMA
      CHARACTER*(*)    FANAME,FBNAME,FFNAME
C
C==== local variables ==================================================
C
      INTEGER          I,JMAX,JMIN,LBLOCK
      LOGICAL          ALIVE,JOINED
C
C**** executable statements ********************************************
C
      INRTIA(1) = 0
      INRTIA(2) = 0
      INRTIA(3) = 0
C
C.... if FANAME is given check the status of the file ..................
C
      IF ( FANAME.NE.'INCORE' .AND. FANAME.NE.'incore' ) THEN
C
         INQUIRE (FILE=FANAME,EXIST=ALIVE,OPENED=JOINED,ERR=1) 
C
         IF      ( .NOT.ALIVE  ) THEN
                 INFO = 2
                 RETURN
         ELSE IF ( .NOT.JOINED ) THEN
                 OPEN (UNIT=FAUNIT,FILE=FANAME,ACCESS='SEQUENTIAL',
     &                 FORM='UNFORMATTED',STATUS='OLD',ERR=1)
         END IF
C
         REWIND (UNIT=FAUNIT,ERR=1)
C
      END IF
C
C.... if FFNAME is given check the status of the file ..................
C
      IF ( FFNAME.NE.'INCORE' .AND. FFNAME.NE.'incore' ) THEN
C
         INQUIRE (FILE=FFNAME,OPENED=JOINED,ERR=1) 
C
         IF ( .NOT.JOINED ) THEN
              OPEN (UNIT=FFUNIT,FILE=FFNAME,ACCESS='SEQUENTIAL',
     &              FORM='UNFORMATTED',STATUS='UNKNOWN',ERR=1)
         END IF
C
         REWIND (UNIT=FFUNIT,ERR=1)
C
      END IF
C
C.... loop on all blocks of (A) ........................................
C
      DO 010 LBLOCK = 1,NBLOCK
C
         JMIN = BLOCK(1,LBLOCK)
         JMAX = BLOCK(2,LBLOCK)
C
C....... read the LBLOCK-th block ......................................
C
         IF ( FANAME.NE.'INCORE' .AND. FANAME.NE.'incore' ) 
     &   READ (UNIT=FAUNIT,ERR=1) (A(I),I=1,DGPNT(JMAX))
C
C....... compute the effective matrix (A)=(A)+SIGMA*(B) ................
C
         CALL SKYMEF (DGPNT,BLOCK,LBLOCK,FBNAME,FBUNIT,
     &                MBTYPE,N,A,B,SIGMA,INFO)
C
         IF ( INFO .NE. 0 ) RETURN
C
C....... influence of previous blocks into the current block ...........
C
         CALL SKYSF1 (A,B,DGPNT,BLOCK,NBLOCK,LBLOCK,FFUNIT,INFO)
C
         IF ( INFO .NE. 0 ) RETURN
C
C....... factorization of the current block ............................
C
         CALL SKYSF2 (A,D,DGPNT,JMIN,JMAX)
C
C....... store the LBLOCK-th block .....................................
C
         IF ( FFNAME.NE.'INCORE' .AND. FFNAME.NE.'incore' )
     &   WRITE (UNIT=FFUNIT,ERR=1) (A(I),I=1,DGPNT(JMAX))
C
  010 CONTINUE
C
C.... diagonal counter (inertia) .......................................
C
      CALL SKYSDC (INRTIA,N,D)
C
      RETURN
C
C.... an IO error has occurred .........................................
C
    1 INFO = 1
C
      RETURN
C
C**** end of SKYSCF ****************************************************
C
      END
