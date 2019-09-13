/*
 * This file automatically produced by /opt/Mathematica/SystemFiles/Links/MathLink/DeveloperKit/Linux-x86-64/CompilerAdditions/mprep from:
 *	main.tm
 * mprep Revision 19 Copyright (c) Wolfram Research, Inc. 1990-2019
 */

#define MPREP_REVISION 19

#include "mathlink.h"

int MLAbort = 0;
int MLDone  = 0;
long MLSpecialCharacter = '\0';

MLINK stdlink = 0;
MLEnvironment stdenv = 0;
MLYieldFunctionObject stdyielder = (MLYieldFunctionObject)0;
MLMessageHandlerObject stdhandler = (MLMessageHandlerObject)0;

/********************************* end header *********************************/


void create_circular_ca P(( int _tp1, int _tp2, double _tp3, double _tp4, int _tp5));

#if MLPROTOTYPES
static int _tr0( MLINK mlp)
#else
static int _tr0(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	int _tp1;
	int _tp2;
	double _tp3;
	double _tp4;
	int _tp5;
	if ( ! MLGetInteger( mlp, &_tp1) ) goto L0;
	if ( ! MLGetInteger( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLNewPacket(mlp) ) goto L5;

	create_circular_ca(_tp1, _tp2, _tp3, _tp4, _tp5);

	res = 1;
L5: L4: L3: L2: L1: 
L0:	return res;
} /* _tr0 */


void create_open_ca P(( int _tp1, int _tp2, double _tp3, double _tp4, int _tp5, double _tp6, int _tp7));

#if MLPROTOTYPES
static int _tr1( MLINK mlp)
#else
static int _tr1(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	int _tp1;
	int _tp2;
	double _tp3;
	double _tp4;
	int _tp5;
	double _tp6;
	int _tp7;
	if ( ! MLGetInteger( mlp, &_tp1) ) goto L0;
	if ( ! MLGetInteger( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLGetReal( mlp, &_tp6) ) goto L5;
	if ( ! MLGetInteger( mlp, &_tp7) ) goto L6;
	if ( ! MLNewPacket(mlp) ) goto L7;

	create_open_ca(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6, _tp7);

	res = 1;
L7: L6: L5: L4: L3: L2: L1: 
L0:	return res;
} /* _tr1 */


void create_autonomous_circular_ca P(( int _tp1, int _tp2, double _tp3, double _tp4, int _tp5, double _tp6));

#if MLPROTOTYPES
static int _tr2( MLINK mlp)
#else
static int _tr2(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	int _tp1;
	int _tp2;
	double _tp3;
	double _tp4;
	int _tp5;
	double _tp6;
	if ( ! MLGetInteger( mlp, &_tp1) ) goto L0;
	if ( ! MLGetInteger( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLGetReal( mlp, &_tp6) ) goto L5;
	if ( ! MLNewPacket(mlp) ) goto L6;

	create_autonomous_circular_ca(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6);

	res = 1;
L6: L5: L4: L3: L2: L1: 
L0:	return res;
} /* _tr2 */


void create_autonomous_open_ca P(( int _tp1, int _tp2, double _tp3, double _tp4, int _tp5, double _tp6, double _tp7, int _tp8));

#if MLPROTOTYPES
static int _tr3( MLINK mlp)
#else
static int _tr3(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	int _tp1;
	int _tp2;
	double _tp3;
	double _tp4;
	int _tp5;
	double _tp6;
	double _tp7;
	int _tp8;
	if ( ! MLGetInteger( mlp, &_tp1) ) goto L0;
	if ( ! MLGetInteger( mlp, &_tp2) ) goto L1;
	if ( ! MLGetReal( mlp, &_tp3) ) goto L2;
	if ( ! MLGetReal( mlp, &_tp4) ) goto L3;
	if ( ! MLGetInteger( mlp, &_tp5) ) goto L4;
	if ( ! MLGetReal( mlp, &_tp6) ) goto L5;
	if ( ! MLGetReal( mlp, &_tp7) ) goto L6;
	if ( ! MLGetInteger( mlp, &_tp8) ) goto L7;
	if ( ! MLNewPacket(mlp) ) goto L8;

	create_autonomous_open_ca(_tp1, _tp2, _tp3, _tp4, _tp5, _tp6, _tp7, _tp8);

	res = 1;
L8: L7: L6: L5: L4: L3: L2: L1: 
L0:	return res;
} /* _tr3 */


void delete_ca P(( void));

#if MLPROTOTYPES
static int _tr4( MLINK mlp)
#else
static int _tr4(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	delete_ca();

	res = 1;

	return res;
} /* _tr4 */


void ca_step P(( void));

#if MLPROTOTYPES
static int _tr5( MLINK mlp)
#else
static int _tr5(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	ca_step();

	res = 1;

	return res;
} /* _tr5 */


void ca_evolve P(( int _tp1));

#if MLPROTOTYPES
static int _tr6( MLINK mlp)
#else
static int _tr6(mlp) MLINK mlp;
#endif
{
	int	res = 0;
	int _tp1;
	if ( ! MLGetInteger( mlp, &_tp1) ) goto L0;
	if ( ! MLNewPacket(mlp) ) goto L1;

	ca_evolve(_tp1);

	res = 1;
L1: 
L0:	return res;
} /* _tr6 */


int ca_count_cars P(( void));

#if MLPROTOTYPES
static int _tr7( MLINK mlp)
#else
static int _tr7(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		int _rp0;

	_rp0 = ca_count_cars();

	res = MLAbort ?
		MLPutFunction( mlp, "Abort", 0) : MLPutInteger( mlp, _rp0);

	return res;
} /* _tr7 */


void ca_calculate_ocupancy P(( void));

#if MLPROTOTYPES
static int _tr8( MLINK mlp)
#else
static int _tr8(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	ca_calculate_ocupancy();

	res = 1;

	return res;
} /* _tr8 */


void ca_calculate_flow P(( void));

#if MLPROTOTYPES
static int _tr9( MLINK mlp)
#else
static int _tr9(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	ca_calculate_flow();

	res = 1;

	return res;
} /* _tr9 */


void ca_get_current P(( void));

#if MLPROTOTYPES
static int _tr10( MLINK mlp)
#else
static int _tr10(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	ca_get_current();

	res = 1;

	return res;
} /* _tr10 */


void ca_get_history P(( void));

#if MLPROTOTYPES
static int _tr11( MLINK mlp)
#else
static int _tr11(mlp) MLINK mlp;
#endif
{
	int	res = 0;
		if( !mlp) return res; /* avoid unused parameter warning */

	ca_get_history();

	res = 1;

	return res;
} /* _tr11 */


static struct func {
	int   f_nargs;
	int   manual;
	int   (*f_func)P((MLINK));
	const char  *f_name;
	} _tramps[12] = {
		{ 5, 0, _tr0, "create_circular_ca" },
		{ 7, 0, _tr1, "create_open_ca" },
		{ 6, 0, _tr2, "create_autonomous_circular_ca" },
		{ 8, 0, _tr3, "create_autonomous_open_ca" },
		{-1, 1, _tr4, "delete_ca" },
		{-1, 1, _tr5, "ca_step" },
		{ 1, 0, _tr6, "ca_evolve" },
		{-1, 1, _tr7, "ca_count_cars" },
		{-1, 1, _tr8, "ca_calculate_ocupancy" },
		{-1, 1, _tr9, "ca_calculate_flow" },
		{-1, 1, _tr10, "ca_get_current" },
		{-1, 1, _tr11, "ca_get_history" }
		};

#define CARDOF_EVALSTRS 0

static int _definepattern P(( MLINK, char*, char*, int));

int  _MLDoCallPacket P(( MLINK, struct func[], int));


#if MLPROTOTYPES
int MLInstall( MLINK mlp)
#else
int MLInstall(mlp) MLINK mlp;
#endif
{
	int _res;
	_res = MLConnect(mlp);
	if (_res) _res = _definepattern(mlp, (char *)"CreateCircularCA[size_Integer, vmax_Integer, density_Real, randp_Real, initVel_Integer]", (char *)"{ size, vmax, density, randp, initVel }", 0);
	if (_res) _res = _definepattern(mlp, (char *)"CreateOpenCA[size_Integer, vmax_Integer, density_Real, randp_Real, initVel_Integer, newCarProb_Real, newCarSpeed_Integer]", (char *)"{ size, vmax, density, randp, initVel, newCarProb, newCarSpeed }", 1);
	if (_res) _res = _definepattern(mlp, (char *)"CreateAutonomousCircularCA[size_Integer, vmax_Integer, density_Real, randp_Real, initVel_Integer, autDensity_Real]", (char *)"{ size, vmax, density, randp, initVel, autDensity }", 2);
	if (_res) _res = _definepattern(mlp, (char *)"CreateAutonomousOpenCA[size_Integer, vmax_Integer, density_Real, randp_Real, initVel_Integer, autDensity_Real, newCarProb_Real, newCarSpeed_Integer]", (char *)"{ size, vmax, density, randp, initVel, autDensity, newCarProb, newCarSpeed }", 3);
	if (_res) _res = _definepattern(mlp, (char *)"DeleteCA[]", (char *)"Manual", 4);
	if (_res) _res = _definepattern(mlp, (char *)"CAStep[]", (char *)"Manual", 5);
	if (_res) _res = _definepattern(mlp, (char *)"CAEvolve[iterations_Integer]", (char *)"{ iterations }", 6);
	if (_res) _res = _definepattern(mlp, (char *)"CACountCars[]", (char *)"Manual", 7);
	if (_res) _res = _definepattern(mlp, (char *)"CACalculateOcupancy[]", (char *)"Manual", 8);
	if (_res) _res = _definepattern(mlp, (char *)"CACalculateFlow[]", (char *)"Manual", 9);
	if (_res) _res = _definepattern(mlp, (char *)"CAGetCurrent[]", (char *)"Manual", 10);
	if (_res) _res = _definepattern(mlp, (char *)"CAGetHistory[]", (char *)"Manual", 11);
	if (_res) _res = MLPutSymbol( mlp, "End");
	if (_res) _res = MLFlush( mlp);
	return _res;
} /* MLInstall */


#if MLPROTOTYPES
int MLDoCallPacket( MLINK mlp)
#else
int MLDoCallPacket( mlp) MLINK mlp;
#endif
{
	return _MLDoCallPacket( mlp, _tramps, 12);
} /* MLDoCallPacket */

/******************************* begin trailer ********************************/

#ifndef EVALSTRS_AS_BYTESTRINGS
#	define EVALSTRS_AS_BYTESTRINGS 1
#endif


#if CARDOF_EVALSTRS
#if MLPROTOTYPES
static int  _doevalstr( MLINK mlp, int n)
#else
static int  _doevalstr( mlp, n)
	 MLINK mlp; int n;
#endif
{
	long bytesleft, charsleft, bytesnow;
#if !EVALSTRS_AS_BYTESTRINGS
	long charsnow;
#endif
	char **s, **p;
	char *t;

	s = (char **)evalstrs;
	while( n-- > 0){
		if( *s == 0) break;
		while( *s++ != 0){}
	}
	if( *s == 0) return 0;
	bytesleft = 0;
	charsleft = 0;
	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = t - *p;
		bytesleft += bytesnow;
		charsleft += bytesnow;
#if !EVALSTRS_AS_BYTESTRINGS
		t = *p;
		charsleft -= MLCharacterOffset( &t, t + bytesnow, bytesnow);
		/* assert( t == *p + bytesnow); */
#endif
		++p;
	}


	MLPutNext( mlp, MLTKSTR);
#if EVALSTRS_AS_BYTESTRINGS
	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = t - *p;
		bytesleft -= bytesnow;
		MLPut8BitCharacters( mlp, bytesleft, (unsigned char*)*p, bytesnow);
		++p;
	}
#else
	MLPut7BitCount( mlp, charsleft, bytesleft);
	p = s;
	while( *p){
		t = *p; while( *t) ++t;
		bytesnow = t - *p;
		bytesleft -= bytesnow;
		t = *p;
		charsnow = bytesnow - MLCharacterOffset( &t, t + bytesnow, bytesnow);
		/* assert( t == *p + bytesnow); */
		charsleft -= charsnow;
		MLPut7BitCharacters(  mlp, charsleft, *p, bytesnow, charsnow);
		++p;
	}
#endif
	return MLError( mlp) == MLEOK;
}
#endif /* CARDOF_EVALSTRS */


#if MLPROTOTYPES
static int  _definepattern( MLINK mlp, char *patt, char *args, int func_n)
#else
static int  _definepattern( mlp, patt, args, func_n)
	MLINK  mlp;
	char  *patt, *args;
	int    func_n;
#endif
{
	MLPutFunction( mlp, "DefineExternal", (long)3);
	  MLPutString( mlp, patt);
	  MLPutString( mlp, args);
	  MLPutInteger( mlp, func_n);
	return !MLError(mlp);
} /* _definepattern */


#if MLPROTOTYPES
int _MLDoCallPacket( MLINK mlp, struct func functable[], int nfuncs)
#else
int _MLDoCallPacket( mlp, functable, nfuncs)
	MLINK mlp;
	struct func functable[];
	int nfuncs;
#endif
{
#if MLINTERFACE >= 4
	int len;
#else
	long len;
#endif
	int n, res = 0;
	struct func* funcp;

	if( ! MLGetInteger( mlp, &n) ||  n < 0 ||  n >= nfuncs) goto L0;
	funcp = &functable[n];

	if( funcp->f_nargs >= 0
#if MLINTERFACE >= 4
	&& ( ! MLTestHead(mlp, "List", &len)
#else
	&& ( ! MLCheckFunction(mlp, "List", &len)
#endif
	     || ( !funcp->manual && (len != funcp->f_nargs))
	     || (  funcp->manual && (len <  funcp->f_nargs))
	   )
	) goto L0;

	stdlink = mlp;
	res = (*funcp->f_func)( mlp);

L0:	if( res == 0)
		res = MLClearError( mlp) && MLPutSymbol( mlp, "$Failed");
	return res && MLEndPacket( mlp) && MLNewPacket( mlp);
} /* _MLDoCallPacket */


#if MLPROTOTYPES
mlapi_packet MLAnswer( MLINK mlp)
#else
mlapi_packet MLAnswer( mlp)
	MLINK mlp;
#endif
{
	mlapi_packet pkt = 0;
#if MLINTERFACE >= 4
	int waitResult;

	while( ! MLDone && ! MLError(mlp)
		&& (waitResult = MLWaitForLinkActivity(mlp),waitResult) &&
		waitResult == MLWAITSUCCESS && (pkt = MLNextPacket(mlp), pkt) &&
		pkt == CALLPKT)
	{
		MLAbort = 0;
		if(! MLDoCallPacket(mlp))
			pkt = 0;
	}
#else
	while( !MLDone && !MLError(mlp) && (pkt = MLNextPacket(mlp), pkt) && pkt == CALLPKT){
		MLAbort = 0;
		if( !MLDoCallPacket(mlp)) pkt = 0;
	}
#endif
	MLAbort = 0;
	return pkt;
} /* MLAnswer */



/*
	Module[ { me = $ParentLink},
		$ParentLink = contents of RESUMEPKT;
		Message[ MessageName[$ParentLink, "notfe"], me];
		me]
*/

#if MLPROTOTYPES
static int refuse_to_be_a_frontend( MLINK mlp)
#else
static int refuse_to_be_a_frontend( mlp)
	MLINK mlp;
#endif
{
	int pkt;

	MLPutFunction( mlp, "EvaluatePacket", 1);
	  MLPutFunction( mlp, "Module", 2);
	    MLPutFunction( mlp, "List", 1);
		  MLPutFunction( mlp, "Set", 2);
		    MLPutSymbol( mlp, "me");
	        MLPutSymbol( mlp, "$ParentLink");
	  MLPutFunction( mlp, "CompoundExpression", 3);
	    MLPutFunction( mlp, "Set", 2);
	      MLPutSymbol( mlp, "$ParentLink");
	      MLTransferExpression( mlp, mlp);
	    MLPutFunction( mlp, "Message", 2);
	      MLPutFunction( mlp, "MessageName", 2);
	        MLPutSymbol( mlp, "$ParentLink");
	        MLPutString( mlp, "notfe");
	      MLPutSymbol( mlp, "me");
	    MLPutSymbol( mlp, "me");
	MLEndPacket( mlp);

	while( (pkt = MLNextPacket( mlp), pkt) && pkt != SUSPENDPKT)
		MLNewPacket( mlp);
	MLNewPacket( mlp);
	return MLError( mlp) == MLEOK;
}


#if MLPROTOTYPES
int MLEvaluate( MLINK mlp, char *s)
#else
int MLEvaluate( mlp, s)
	MLINK mlp;
	char *s;
#endif
{
	if( MLAbort) return 0;
	return MLPutFunction( mlp, "EvaluatePacket", 1L)
		&& MLPutFunction( mlp, "ToExpression", 1L)
		&& MLPutString( mlp, s)
		&& MLEndPacket( mlp);
} /* MLEvaluate */


#if MLPROTOTYPES
int MLEvaluateString( MLINK mlp, char *s)
#else
int MLEvaluateString( mlp, s)
	MLINK mlp;
	char *s;
#endif
{
	int pkt;
	if( MLAbort) return 0;
	if( MLEvaluate( mlp, s)){
		while( (pkt = MLAnswer( mlp), pkt) && pkt != RETURNPKT)
			MLNewPacket( mlp);
		MLNewPacket( mlp);
	}
	return MLError( mlp) == MLEOK;
} /* MLEvaluateString */


#if MLPROTOTYPES
void MLDefaultHandler( MLINK mlp, int message, int n)
#else
void MLDefaultHandler( mlp, message, n)
	MLINK mlp;
	int message, n;
#endif
{
	switch (message){
	case MLTerminateMessage:
		MLDone = 1;
	case MLInterruptMessage:
	case MLAbortMessage:
		MLAbort = 1;
	default:
		return;
	}
}

#if MLPROTOTYPES
static int _MLMain( char **argv, char **argv_end, char *commandline)
#else
static int _MLMain( argv, argv_end, commandline)
  char **argv, argv_end;
  char *commandline;
#endif
{
	MLINK mlp;
	int err;

#if MLINTERFACE >= 4
	if( !stdenv)
		stdenv = MLInitialize( (MLEnvironmentParameter)0);
#else
	if( !stdenv)
		stdenv = MLInitialize( (MLParametersPointer)0);
#endif

	if( stdenv == (MLEnvironment)0) goto R0;

	if( !stdhandler)
		stdhandler = (MLMessageHandlerObject)MLDefaultHandler;


	mlp = commandline
		? MLOpenString( stdenv, commandline, &err)
		: MLOpenArgcArgv( stdenv, (int)(argv_end - argv), argv, &err);
	if( mlp == (MLINK)0){
		MLAlert( stdenv, MLErrorString( stdenv, err));
		goto R1;
	}

	if( stdyielder) MLSetYieldFunction( mlp, stdyielder);
	if( stdhandler) MLSetMessageHandler( mlp, stdhandler);

	if( MLInstall( mlp))
		while( MLAnswer( mlp) == RESUMEPKT){
			if( ! refuse_to_be_a_frontend( mlp)) break;
		}

	MLClose( mlp);
R1:	MLDeinitialize( stdenv);
	stdenv = (MLEnvironment)0;
R0:	return !MLDone;
} /* _MLMain */


#if MLPROTOTYPES
int MLMainString( char *commandline)
#else
int MLMainString( commandline)  char *commandline;
#endif
{
	return _MLMain( (charpp_ct)0, (charpp_ct)0, commandline);
}

#if MLPROTOTYPES
int MLMainArgv( char** argv, char** argv_end) /* note not FAR pointers */
#else
int MLMainArgv( argv, argv_end)  char **argv, **argv_end;
#endif
{   
	static char FAR * far_argv[128];
	int count = 0;
	
	while(argv < argv_end)
		far_argv[count++] = *argv++;
		 
	return _MLMain( far_argv, far_argv + count, (charp_ct)0);

}

#if MLPROTOTYPES
int MLMain( int argc, char **argv)
#else
int MLMain( argc, argv) int argc; char **argv;
#endif
{
 	return _MLMain( argv, argv + argc, (char *)0);
}
 
