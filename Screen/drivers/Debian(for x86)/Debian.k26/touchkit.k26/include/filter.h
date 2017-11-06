
#ifndef _FILTER_H_
#define _FILTER_H_

#ifndef VOID
	#define VOID	void
#endif

#ifndef BOOL
	#define BOOL	int
#endif

#define ZTNum		4

typedef struct tagFilterCoeff
{	long	lDefZTranCoeff[ZTNum];
	long	lUpperBound;
	long	lLowerBound;
	long	lThreshold;
	BOOL	bFilterOnOff;
}	FILTERCOEFF, *PFILTERCOEFF;
					

#define F_COFF0			4
#define F_COFF1			4
#define F_COFF2			4
#define F_COFF3			12

#define F_MaxCOFF		50
#define F_MinCOFF		0

#define F_C3UBound		24
#define	F_C3LBound		1

#define F_TholdNUM		1L
#define F_TholdDEN		250L

#define GetTholdFromRange(r)	((r)*F_TholdNUM/F_TholdDEN)

#define InitFilter(fp, C0, C1, C2, C3, MC3, mC3)		\
	(	(fp)->lDefZTranCoeff[0] = (C0),					\
		(fp)->lDefZTranCoeff[1] = (C1),					\
		(fp)->lDefZTranCoeff[2] = (C2),					\
		(fp)->lDefZTranCoeff[3] = (C3),					\
		(fp)->lUpperBound = (MC3),						\
		(fp)->lLowerBound = (mC3),						\
		(fp)->lThreshold = 10,							\
		(fp)->bFilterOnOff = 1							\
	)


#define InitFilterDefault(fp)							\
	InitFilter(fp, F_COFF0, F_COFF1, F_COFF2, F_COFF3,	\
				F_C3UBound, F_C3LBound)

#define SetFilterThreshold(fp, t)		((fp)->lThreshold = (t))


#endif

