/*
 * photosynthesis2.c
 *
 *  Created on: 11 set 2017
 *      Author: alessio
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "photosynthesis.h"
#include "canopy_evapotranspiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

//extern logger_t* g_debug_log;
extern settings_t* g_settings;


void total_photosynthesis_biome (cell_t *const c, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual)
{

	double cond_corr;        /* (umol/m2/s/Pa) leaf conductance corrected for CO2 vs. water vapor */
	double leafN;            /* (gNleaf/m2) leaf N per unit leaf area */
	double par_abs;          /* (umol/m2 covered/sec) absorbed par */
	double leaf_day_mresp;   /* (umol/m2/s) day leaf m. resp, proj. area basis */
	double psn;              /* photosynthesis (gC/m2/day) */
	int sun_shade;           /* 0 for sun 1 for shaded */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* This function is a wrapper and replacement for the photosynthesis code which used to be in the central bgc.c code */
	/* note: the basis of this photosynthesis code is the DePury and Farquhar two-leaf model of photosynthesis
	 * (Farquhar, Caemmerer et al. 1980; De Pury and Farquhar 1997).*/

	/************************************************************************************************************************************/

	if ( s->value[LEAF_SUN_N] > 0. )
	{
		/* SUNLIT canopy fraction photosynthesis per unit area */
		sun_shade = 0;

		/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor
		(see for correction also Nobel 1991; Jones 1992 and Landsberg and Sands book pg 54) */
		cond_corr                    = s->value[LEAF_SUN_CONDUCTANCE] * 1e6 / ( 1.6 * Rgas * ( meteo_daily->tday + TempAbs ) );

		/* convert Leaf Nitrogen from tN/cell --> to gN m-2 one-sided leaf area */
		leafN                        = ( s->value[LEAF_SUN_N] * 1e6 / g_settings->sizeCell ) / s->value[LAI_SUN_PROJ];

		/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
		//note: since absorbed radiation is scaled to the 24 hours also leaf day resp should be scaled to 24 hours
		leaf_day_mresp               = ( s->value[DAILY_LEAF_SUN_MAINT_RESP] / ( 86400. * GC_MOL * 1e-6 ) ) / s->value[LAI_SUN_PROJ];

		/* convert absorbed par per projected LAI molPAR/m2/day --> umol/m-2 one-sided leaf area/sec */
		par_abs                      = ( s->value[APAR_SUN] * 1e6 / 86400. ) / s->value[LAI_SUN_PROJ];

		/* call Farquhar for sun leaves leaves photosynthesis */
		psn = Farquhar (c, s, meteo_daily, meteo_annual, cond_corr, leafN, par_abs, leaf_day_mresp, sun_shade);

		/* Canopy gross assimilation and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		/* note (from Biome-BGC): "for the final flux assignment, the assimilation output needs to have the maintenance respiration rate added..." */
		s->value[GROSS_ASSIMILATION_SUN]   = ( psn + leaf_day_mresp ) * s->value[LAI_SUN_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

		/* net assimilation and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		/* note (in Biome-BGC) assimilation (assim_sun variable) doesn't take into account leaf resp */
		s->value[NET_ASSIMILATION_SUN]     = psn * s->value[LAI_SUN_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

	}

	/************************************************************************************************************************************/

	if ( s->value[LEAF_SHADE_N] > 0. )
	{
		/* SHADED canopy fraction photosynthesis per unit area */
		sun_shade = 1;

		/* convert conductance from m/s --> umol/m2/s/Pa, and correct for CO2 vs. water vapor
		(see for correction also Nobel 1991; Jones 1992 and Landsberg and Sands book pg 54) */
		cond_corr                    = s->value[LEAF_SHADE_CONDUCTANCE] * 1e6 / ( 1.6 * Rgas * ( meteo_daily->tday + TempAbs ) );

		/* convert Leaf Nitrogen from tN/cell --> to gN m-2 one-sided leaf area */
		leafN                        = ( s->value[LEAF_SHADE_N] * 1e6 / g_settings->sizeCell ) / s->value[LAI_SHADE_PROJ];

		/* convert from mass to molar units, and from a daily rate to a rate per second (umol/m2/s) */
		//note: since absorbed radiation is scaled to the 24 hours also leaf day resp should be scaled to 24 hours
		leaf_day_mresp               = ( s->value[DAILY_LEAF_SHADE_MAINT_RESP] / ( 86400. * GC_MOL * 1e-6 ) ) / s->value[LAI_SHADE_PROJ];

		/* convert absorbed par per projected LAI molPAR/m2/day --> umol/m-2 one-sided leaf area/sec */
		par_abs                      = ( s->value[APAR_SHADE] * 1e6 / 86400. ) / s->value[LAI_SHADE_PROJ];

		/* call Farquhar for shade leaves photosynthesis */
		psn = Farquhar (c, s, meteo_daily, meteo_annual, cond_corr, leafN, par_abs, leaf_day_mresp, sun_shade );

		/* Canopy gross assimilation (photosynthesis) and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		/* note (from Biome-BGC): "for the final flux assignment, the assimilation output needs to have the maintenance respiration rate added..." */
		s->value[GROSS_ASSIMILATION_SHADE] = ( psn + leaf_day_mresp ) * s->value[LAI_SHADE_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

		/* net assimilation (photosynthesis) and converting from umol/m2 leaf/sec gC/m2/day and to LAI for canopy computation */
		/* note (in Biome-BGC) assimilation (assim_shade variable) doesn't take into account leaf resp */
		s->value[NET_ASSIMILATION_SHADE]   = psn * s->value[LAI_SHADE_PROJ] * meteo_daily->daylength_sec * GC_MOL * 1e-6;

	}

	/************************************************************************************************************************************/

	/* as In Wohlfahrt & Gu 2015 Plant Cell and Environment */
	/* "GPP is intended as an integration of apparent photosynthesis (true photosynthesis minus photorespiration), NOT
	gross (true) photosynthesis" */

	/* total gross assimilation */
	s->value[GROSS_ASSIMILATION] = s->value[GROSS_ASSIMILATION_SUN] + s->value[GROSS_ASSIMILATION_SHADE];

	/* total net assimilation */
	s->value[NET_ASSIMILATION]   = s->value[NET_ASSIMILATION_SUN]   + s->value[NET_ASSIMILATION_SHADE];

	/************************************************************************************************************************************/

	s->value[MONTHLY_GROSS_ASSIMILATION]       += s->value[GROSS_ASSIMILATION];
	s->value[MONTHLY_GROSS_ASSIMILATION_SUN]   += s->value[GROSS_ASSIMILATION_SUN];
	s->value[MONTHLY_GROSS_ASSIMILATION_SHADE] += s->value[GROSS_ASSIMILATION_SHADE];

	s->value[YEARLY_GROSS_ASSIMILATION]        += s->value[GROSS_ASSIMILATION];
	s->value[YEARLY_GROSS_ASSIMILATION_SUN]    += s->value[GROSS_ASSIMILATION_SUN];
	s->value[YEARLY_GROSS_ASSIMILATION_SHADE]  += s->value[GROSS_ASSIMILATION_SHADE];

	s->value[MONTHLY_NET_ASSIMILATION]         += s->value[NET_ASSIMILATION];
	s->value[MONTHLY_NET_ASSIMILATION_SUN]     += s->value[NET_ASSIMILATION_SUN];
	s->value[MONTHLY_NET_ASSIMILATION_SHADE]   += s->value[NET_ASSIMILATION_SHADE];

	s->value[YEARLY_NET_ASSIMILATION]          += s->value[NET_ASSIMILATION];
	s->value[YEARLY_NET_ASSIMILATION_SUN]      += s->value[NET_ASSIMILATION_SUN];
	s->value[YEARLY_NET_ASSIMILATION_SHADE]    += s->value[NET_ASSIMILATION_SHADE];

	/************************************************************************************************************************************/
#if 0
	/* gpp */
	s->value[GPP_SUN]                     = s->value[NET_ASSIMILATION_SUN];
	s->value[GPP_SHADE]                   = s->value[NET_ASSIMILATION_SHADE];
	s->value[GPP]                         = s->value[GPP_SUN] + s->value[GPP_SHADE];
#else
	/* gpp */
	s->value[GPP_SUN]                     = s->value[GROSS_ASSIMILATION_SUN];
	s->value[GPP_SHADE]                   = s->value[GROSS_ASSIMILATION_SHADE];
	s->value[GPP]                         = s->value[GPP_SUN] + s->value[GPP_SHADE];

#endif

	/* gC/m2/day --> tC/cell/day */
	s->value[GPP_tC]                      = s->value[GPP] / 1e6 * g_settings->sizeCell ;

	s->value[MONTHLY_GPP]                += s->value[GPP];
	s->value[MONTHLY_GPP_SUN]            += s->value[GPP_SUN];
	s->value[MONTHLY_GPP_SHADE]          += s->value[GPP_SHADE];

	s->value[YEARLY_GPP]                 += s->value[GPP];
	s->value[YEARLY_GPP_SUN]             += s->value[GPP_SUN];
	s->value[YEARLY_GPP_SHADE]           += s->value[GPP_SHADE];

	/* cell level */
	c->daily_gpp                         += s->value[GPP];
	c->monthly_gpp                       += s->value[GPP];
	c->annual_gpp                        += s->value[GPP];

	c->daily_gpp_tC                      += s->value[GPP_tC];
	c->monthly_gpp_tC                    += s->value[GPP_tC];
	c->annual_gpp_tC                     += s->value[GPP_tC];

	/* yearly veg days counter */
	if ( s->value[GPP] > 0. )
	{
		++s->counter[YEARLY_VEG_DAYS];
	}

	/************************************************************************************************************************************/

	/* test: compute actual quantum canopy efficiency (molC/molphotons PAR) */

	if ( s->value[NET_ASSIMILATION]       > 0. ) s->value[ALPHA_EFF]          = ( s->value[NET_ASSIMILATION]       / GC_MOL ) / s->value[PAR];
	else                                         s->value[ALPHA_EFF]          = 0.;
	if ( s->value[NET_ASSIMILATION_SUN]   > 0. ) s->value[ALPHA_EFF_SUN]      = ( s->value[NET_ASSIMILATION_SUN]   / GC_MOL ) / s->value[PAR_SUN];
	else                                         s->value[ALPHA_EFF_SUN]      = 0.;
	if ( s->value[NET_ASSIMILATION_SHADE] > 0. ) s->value[ALPHA_EFF_SHADE]    = ( s->value[NET_ASSIMILATION_SHADE] / GC_MOL ) / s->value[PAR_SHADE];
	else                                         s->value[ALPHA_EFF_SHADE]    = 0.;

	/************************************************************************************************************************************/

}

double Farquhar (cell_t *const c, species_t *const s,const meteo_daily_t *const meteo_daily, const meteo_annual_t *const meteo_annual,
		const double cond_corr, const double leafN, const double par_abs, const double leaf_day_mresp, const int sun_shade )
{
	/* Farquhar, von Caemmerer and Berry (1980) Planta. 149: 78-90. */

	/* the weight proportion of Rubisco to its nitrogen content, fnr, is calculated from the relative proportions of the basic amino acids
		that make up the enzyme, as listed in the Handbook of Biochemistry, Proteins, Vol III, p. 510, which references: Kuehn and McFadden, Biochemistry, 8:2403, 1969 */

	/* the following enzyme kinetic constants are from:
		Woodrow, I.E., and J.A. Berry, 1980. Enzymatic regulation of photosynthetic
		CO2 fixation in C3 plants. Ann. Rev. Plant Physiol. Plant Mol. Biol.,
		39:533-594.
		Note that these values are given in the units used in the paper, and that
		they are converted to units appropriate to the rest of this function before
		they are used. */
	/* I've changed the values for Kc and Ko from the Woodrow and Berry
		reference, and am now using the values from De Pury and Farquhar,
		1997. Simple scaling of photosynthesis from leaves to canopies
		without the errors of big-leaf models. Plant, Cell and Env. 20: 537-557.
		All other parameters, including the q10's for Kc and Ko are the same
		as in Woodrow and Berry. */

	static double Kc25          = 404;    /* (ubar) michaelis-menten const carboxylase, 25 deg C  Badger and Collatz value*/
	static double q10Kc         = 2.1;    /* (DIM) Q_10 for Kc Badger and Collatz and Collatz et al., (2001) */
	static double Ko25          = 248;    /* (mbar) michaelis-menten const oxygenase, 25 deg C Badger and Collatz value */
	static double q10Ko         = 1.2;    /* (DIM) inhibition constant for O2 Collatz et al., (1991) */
	static double act25         = 3.6;    /* (umol/mgRubisco/min) Rubisco activity at 25 C Badger and Collatz value */
	static double q10act        = 2.4;    /* (DIM) Q_10 for Rubisco activity Badger and Collatz value Collatz et al., (1991) */
	static double pabsII_frac   = 0.85;   /* (DIM) fraction of PAR effectively absorbed by photosytem II (leaf absorptance); 0.8 for Bonan et al., 2011 */
	static double fnr           = 7.16;   /* (DIM) g Rubisco/gN Rubisco weight proportion of rubisco relative to its N content Kuehn and McFadden (1969) */
	static double theta         = 0.7;    /* (DIM) curvature of the light-response curve of electron transport (DePury and Farquhar, 1997, Bonan et al., 2011) */

	/* local variables */
	double Kc;                     /* (Pa) michaelis-menten constant for carboxylase reaction */
	double Ko;                     /* (Pa) michaelis-menten constant for oxygenase reaction */
	double act;                    /* (umol CO2/kgRubisco/s) Rubisco activity scaled by temperature and [O2] and [CO2] */
	double Jmax;                   /* (umol/m2/s) max rate electron transport */
	double ppe;                    /* (mol e- /mol photons) photons absorbed by PSII per e- transported (quantum yield of electron transport) */
	double Vcmax;                  /* (umol/m2/s) Leaf-scale maximum carboxylation rate, 25°C */
	double pabsII;                 /* (molPAR/m2/sec) PAR effectively absorbed by the phosystemII */
	double J;                      /* (umol/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration */
	double gamma;                  /* (Pa) CO2 compensation point without dark respiration */
	double Ca;                     /* (Pa) atmospheric [CO2] pressure */
	double O2;                     /* (Pa) intercellular O2 partial pressure, taken to be 0·21 (mol mol-1) see Medlyn et al., 1999 */
	double Av;                     /* (umol/m2/s) carboxylation rate for limited assimilation (synonym of Vc) */
	double Aj;                     /* (umol/m2/s) RuBP (ribulose-1,5-bisphosphate) regeneration limited assimilation */
	double A;                      /* (umol/m2/s) final assimilation rate */
	double Ci;                     /* (Pa) intercellular [CO2] */
	double Rd;                     /* (umol/m2/s) (umol/m2/s) day leaf m. resp, proj. area basis */
	double var_a, var_b, var_c, det;

	//todo todo todo todo todo move in species.txt (this should be the only variable for all photosynthesis)
	// double beta = 1.67; /* Jmax:Vcmax note: in Medlyn et al., 2002*/
	double beta       = 2.1; /* ratio between Vcmax and Jmax see dePury and Farquhar 1997; for fagus see Liozon et al., (2000) and Castanea */
	double test_Vcmax = 55 ; /* (umol/m2/sec) Vcmax for fagus see Deckmyn et al., 2004 GCB */
	double test_Jmax  = 100; /* (umol/m2/sec) Jmax for fagus see Deckmyn et al., 2004 GCB */
	/*
	 * some parameter values (to be included in species.txt):
	 * Vcmax = 55 (umol/m2/sec) for fagus see Deckmyn et al., 2004 GCB
	 * Vcmax = 36.21 (umol/m2/sec) for fagus see Scartazza et al., 2015
	 * Vcmax = 26.66 (umolCO2/m2/sec) for p. sylvestris see Sampson et al., (2006)
	 * Vcmax = 33.7 (umolCO2/m2/sec) for q. robur see Samspon et al. (2006)
	 * Vcmax = 72 (mmol/m2/sec) for p. sylvestris see Sampson et al., 2001
	 * Jmax  = 100 (umol/m2/sec) for fagus see Deckmyn et al., 2004 GCB
	 * Jmax  = 104 (umol/m2/sec) for fagus see Scartazza et al., 2015
	 * beta  = 2.48 for p. sylvestris see Samspon et al., (2006)
	 * beta  = 2.42 for Q. robur see Samspon et al., (2006)
	 * beta  = 2.8 for F. sylvatica see Scartazza et al., 2015
	*/

	/* begin by assigning local variables */

	/* (umol/m2/s) day leaf m. resp, proj. area basis */
	//note: BIOME-BGC assumes leaf main respiration during daylight as dark respiration
	//note: 3D-CMCC following Dufrene et al. considers light inhibition phenomenon in the
	//      daylight respiration routine
	Rd  = leaf_day_mresp;

	/* convert atmospheric CO2 from ppmV --> Pa */
	Ca  = meteo_annual->co2Conc * meteo_daily->air_pressure / 1e6;

	/* set quantum yield for electron transport for C3 as in DePury and Farquhar 1997 */
	ppe = 2.6; /* is equivalent to 0.38 (mol electrons mol-1 photons) */

	/* calculate atmospheric O2 in Pa, assumes 20.9% O2 by volume */
	O2  = (O2CONC / 100. ) * meteo_daily->air_pressure;

	/*******************************************************************************/
	/* the enzyme kinetics built into this model are based on Woodrow and Berry (1988) and Collatz et al., (1991) */

	/* correct kinetic constants for temperature, and do unit conversions */
	Ko  = Ko25 * pow ( q10Ko , ( meteo_daily->tday - 25. ) / 10. );
	Ko *= 100.;   /* mbar --> Pa */

	/* Michaelis Menten approach */
	if ( meteo_daily->tday > 15. )
	{
		Kc  = Kc25  * pow ( q10Kc , ( meteo_daily->tday - 25. ) / 10. );
		act = act25 * pow ( q10act, ( meteo_daily->tday - 25. ) / 10. );
	}
	else
	{
		Kc  = Kc25  * pow ( 1.8 * q10Kc,  ( meteo_daily->tday - 15. ) / 10.) / q10Kc;
		act = act25 * pow ( 1.8 * q10act, ( meteo_daily->tday - 15. ) / 10.) / q10act;
	}

	/* convert Kc ubar --> Pa */
	Kc *= 0.1;

	/* convert rubisco activity units from umol/mgRubisco/min -> umol/gRubisco/s */
	act = act  * 1e3 / 60.;

	/******************************************************************************************************************************/

	/* calculate gamma (Pa) CO2 compensation point due to photorespiration, in the absence of maint (or dark?) respiration */
#if 1

	/* note: BIOME-BGC method */
	/* it assumes Vomax/Vcmax = 0.21; Badger & Andrews (1974) */
	/* 0.5 because with 1 mol of oxygenations assumed to release 0.5 molCO2 by glycine decarboxilation (Farquhar and Busch 2017) */
	gamma = 0.5 * 0.21 * Kc * O2 / Ko;

#else

	/* note: dePury and Farquhar 1997 method */
	gamma = 36.9 + 1.88 * ( meteo_daily->tday - 25. ) + 0.036 * ( pow ( ( meteo_daily->tday - 25. ) , 2. ) );

	//fixme why?
	gamma *= (meteo_daily->air_pressure / 1e6);
#endif


	/******************************************************************************************************************************/

	/* calculate Vmax from leaf nitrogen data and Rubisco activity */

	/* kg Nleaf   kg NRub    kg Rub      umol            umol
	   -------- X -------  X ------- X ---------   =   --------
	      m2      kg Nleaf   kg NRub   kg RUB * s       m2 * s

	     (lnc)  X  (flnr)  X  (fnr)  X   (act)     =    (Vmax)
	*/

	/* calculate Vcmax (umol CO2/m2/s) max rate of carboxylation from leaf nitrogen data and Rubisco activity */
	/* see:
	 * Woodrow and Berry (1988)
	 * Field (1983)
	 * Harley et al., (1992)
	 * Medlyn et al., (1999)
	 * */

#if 0 //not currently used
	//note: modified version of the BIOME-BGC original code
	if ( /*s->value[VCMAX]*/ test_Vcmax != NO_DATA )
	{
		Vcmax = leafN * s->value[N_RUBISCO] * fnr * act;

		if ( Vcmax > /*s->value[VCMAX]*/ test_Vcmax )
		{
			Vcmax = /*s->value[VCMAX]*/ test_Vcmax;
		}
	}
	else
	{
		Vcmax = leafN * s->value[N_RUBISCO] * fnr * act;
	}
#else
	/* "V cmax is more realistically formulated as a dynamic quantity that depends on the leaf area–based concentration of Rubisco and the enzyme activity"
	 * references:
	 * Niinemets and Tenhunen 1997
	 * Thornton and Zimmermann 2007
	 * */
	Vcmax = leafN * s->value[N_RUBISCO] * fnr * act;
#endif

	/******************************************************************************************************************************/

	/* calculate Jmax = f(Vmax), reference:	Wullschleger, S.D., 1993.  Biochemical limitations to carbon assimilation in C3 plants -
	 * A retrospective analysis of the A/Ci curves from	109 species. Journal of Experimental Botany, 44:907-920. */
	/* compute (umol electrons/m2/s) max rate electron transport */

#if 0
	//not currently used
	//note: modified version of the BIOME-BGC original code
	if ( /* s->value[JMAX] */ test_Jmax != NO_DATA )
	{
		Jmax = beta * Vcmax;

		if (Jmax > /*s->value[JMAX]*/ test_Jmax)
		{
			Jmax = /*s->value[JMAX]*/test_Jmax;
		}
	}
	else
	{
		Jmax = beta * Vcmax;
	}
#else
	//note: original version of the BIOME-BGC code
	/* a simplifying assumption that empirically relates the maximum rate of electron transport to maximum carboxylation velocity
	 * see:
	 * Wullschleger (1993)
	 * Field (1983)
	 * Harley et al., (1992)
	 * Watanabe et al., (1994)
	 * DePury and Farquhar (1997)
	 * Medlyn et al., (1999)
	 * Peterson et al., (1999)
	 * Liozon et al., (2000)
	 * Leuning et al., (2002)
	 * Bonan et al., (2011)*/

	Jmax = beta * Vcmax;
#endif

	/******************************************************************************************************************************/

	/* irradiance dependence of electron transport (the "non-rectangular hyperbola") */
	/* from the equation of de Pury and Farquhar (1997) Plant Cell and Env.*/
	/*
	 *
	   theta J^2 - (pabsII + Jmax) J + pabsII Jmax = 0

	*/

	/* compute PAR effectively absorbed by photosystem II */
	pabsII = par_abs * pabsII_frac / ppe;

	/* calculate J = f(Jmax, ppfd) */
	/* smaller root of the quadratic solution to the following equation */
	var_a  = theta;
	var_b  = -Jmax - pabsII;
	var_c  =  Jmax * pabsII;

	/* compute (umol RuBP/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration */
	J      = ( -var_b - sqrt ( var_b * var_b - 4. * var_a * var_c ) ) / ( 2. * var_a );

	/******************************************************************************************************************************/

	/* solve for Av and Aj using the quadratic equation, substitution for Ci
		from A = g(Ca-Ci) into the equations from Farquhar and von Caemmerer:

		       Vmax (Ci - gamma)
		Av =  -------------------   -   Rd
		      Ci + Kc (1 + O2/Ko)


		         J (Ci - gamma)
		Aj  =  -------------------  -   Rd
	           4.5 Ci + 10.5 gamma
	 */

	/* quadratic solution for Av */
	var_a =  -1. / cond_corr;
	var_b = Ca + ( Vcmax - Rd ) / cond_corr + Kc * ( 1. + O2 / Ko );
	var_c = Vcmax * ( gamma - Ca ) + Rd * ( Ca + Kc * ( 1. + O2 / Ko ) );
	det   = var_b * var_b - 4. * var_a * var_c;

	/* check condition */
	CHECK_CONDITION( det , <, 0.0);

	/* compute photosynthesis when Av (or Vc) (umol CO2/m2/s) carboxylation rate for limited assimilation
	 * (net photosynthesis rate when Rubisco activity is limiting) */
	Av    = ( -var_b + sqrt( det ) ) / ( 2. * var_a );

	/* quadratic solution for Aj */
	var_a = -4.5 / cond_corr;
	var_b = 4.5 * Ca + 10.5 * gamma + J / cond_corr - 4.5 * Rd / cond_corr;
	var_c = J * ( gamma - Ca ) + Rd * ( 4.5 * Ca + 10.5 * gamma );

	det   = var_b * var_b - 4. * var_a * var_c;
	/* check condition */
	CHECK_CONDITION( det , <, 0.0);

	/* compute photosynthesis when (umol CO2/m2/s) RuBP (ribulose-1,5-bisphosphate) regeneration limited assimilation
	 * (net photosynthesis rate when RuBP (ribulose-1,5-bisphosphate)-regeneration is limiting) */
	Aj = ( -var_b + sqrt( det ) ) / ( 2. * var_a );

	/* compute (umol CO2/m2/s) final assimilation rate */
	/* estimate A as the minimum of (Av,Aj) */
	A = MIN ( Av , Aj );

	/* compute (Pa) intercellular [CO2] */
	Ci = Ca - ( A / cond_corr );

	/* test */
	if ( sun_shade == 0 )
	{
		/* sun leaves */
		s->value[A_SUN]                  = A;
		if ( Av < Aj )s->value[Av_SUN]   = Av;
		else          s->value[Aj_SUN]   = Aj;

		s->value[YEARLY_A_SUN]    += A;
		s->value[YEARLY_Av_SUN]   += s->value[Av_SUN];
		s->value[YEARLY_Aj_SUN]   += s->value[Aj_SUN];
		CHECK_CONDITION ( fabs ( s->value[YEARLY_A_SUN] - ( s->value[YEARLY_Av_SUN] + s->value[YEARLY_Aj_SUN] ) ) , > , eps );
	}
	else
	{
		/* shaded leaves */
		s->value[A_SHADE]                  = A;
		if ( Av < Aj )s->value[Av_SHADE]   = Av;
		else          s->value[Aj_SHADE]   = Aj;

		s->value[YEARLY_A_SHADE]    += A;
		s->value[YEARLY_Av_SHADE]   += s->value[Av_SHADE];
		s->value[YEARLY_Aj_SHADE]   += s->value[Aj_SHADE];
		CHECK_CONDITION ( fabs ( s->value[YEARLY_A_SHADE] - ( s->value[YEARLY_Av_SHADE] + s->value[YEARLY_Aj_SHADE] ) ) , > , eps );
	}

	/* total leaves */
	s->value[A_TOT]  = s->value[A_SUN]  + s->value[A_SHADE];
	s->value[Av_TOT] = s->value[Av_SUN] + s->value[Av_SHADE];
	s->value[Aj_TOT] = s->value[Aj_SUN] + s->value[Aj_SHADE];

	s->value[YEARLY_A_TOT]  += s->value[A_SUN]  + s->value[A_SHADE];
	s->value[YEARLY_Av_TOT] += s->value[Av_SUN] + s->value[Av_SHADE];
	s->value[YEARLY_Aj_TOT] += s->value[Aj_SUN] + s->value[Aj_SHADE];
	CHECK_CONDITION ( fabs ( s->value[YEARLY_A_TOT] - ( s->value[YEARLY_Av_TOT] + s->value[YEARLY_Aj_TOT] ) ) , > , eps );


	return A;


}

