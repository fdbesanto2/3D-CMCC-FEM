#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mpfit.h"
#include "common.h"
#include "leaf_fall.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_debug_log;


void leaf_fall_deciduous ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double foliage_to_remove;
	static double fine_root_to_remove;
	static double fraction_to_retransl = 0.1; /* fraction of C to re-translocate (see Bossell et al., 2006 and Campioli et al., 2013 */
	double previousLai, currentLai;
	double previousBiomass_lai, newBiomass_lai;


	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL DECIDUOUS **\n");

	logger(g_debug_log, "Leaf fall counter = %d\n", s->counter[LEAF_FALL_COUNTER]);
	logger(g_debug_log, "Leaf Carbon = %f\n", s->value[LEAF_C]);
	logger(g_debug_log, "Fine root Carbon  = %f\n", s->value[FINE_ROOT_C]);

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_debug_log, "First day of Leaf fall\n");
		logger(g_debug_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);

		/* note: assuming that fine roots for deciduous species progressively die together with leaves */
		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */

		/* assign LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI_PROJ] = s->value[LAI_PROJ];

		/* assign senescence doy */
		s->counter[SENESCENCE_DAY_ONE] = c->doy;
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of leaf and fine root Carbon is daily re-translocated as reserve in the reserve pool */
		/* compute amount of leaf and fine root Carbon to re-translocate as reserve */


		s->value[C_LEAF_TO_RESERVE] = (s->value[LEAF_C] * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);


		s->value[C_FINEROOT_TO_RESERVE]= (s->value[FINE_ROOT_C] * fraction_to_retransl) /s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);

		previousLai = s->value[LAI_PROJ];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai = MAX(0,s->value[MAX_LAI_PROJ] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE] -
				c->doy)/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAY_ONE]) -
						log(.11111111111))))));
		logger(g_debug_log, "previousLai = %f\n", previousLai);
		logger(g_debug_log, "currentLai = %f\n", currentLai);

		/* check */
		CHECK_CONDITION(previousLai, <, currentLai);

		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);

		newBiomass_lai = (currentLai * (s->value[CANOPY_COVER_PROJ] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0));


		foliage_to_remove = previousBiomass_lai - newBiomass_lai;
		logger(g_debug_log, "foliage_to_remove = %f\n", foliage_to_remove);


		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FINE_ROOT_C]*foliage_to_remove)/s->value[LEAF_C];
		logger(g_debug_log, "fineroot_to_remove = %f\n", fine_root_to_remove);

		/* update leaf falling */
		s->value[C_TO_LEAF] = -foliage_to_remove ;
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = -fine_root_to_remove;
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", s->value[C_TO_FINEROOT]);
		s->value[C_TO_LITTER] = (foliage_to_remove - s->value[C_LEAF_TO_RESERVE]);
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
		s->value[C_TO_SOIL]= (fine_root_to_remove - s->value[C_FINEROOT_TO_RESERVE]);
		logger(g_debug_log, "C_TO_SOIL = %f\n", s->value[C_TO_SOIL]);
	}
	else
	{
		/** LAST DAY OF LEAF FALL **/
		logger(g_debug_log, "Last day of leaf fall\n");
		s->value[C_TO_LEAF] = -s->value[LEAF_C];
		logger(g_debug_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = - s->value[FINE_ROOT_C];
		logger(g_debug_log, "C_TO_FINEROOT = %f\n", -s->value[C_TO_FINEROOT]);

		/* assuming that the last day of leaf fall all the remaining carbon goes to reserve */
		s->value[C_LEAF_TO_RESERVE] = s->value[LEAF_C];
		logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FINEROOT_TO_RESERVE] = s->value[FINE_ROOT_C];
		logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);

		s->value[C_TO_LITTER] = 0.;
		logger(g_debug_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
		s->value[C_TO_SOIL] = 0.;
		logger(g_debug_log, "C_TO_SOIL = %f\n", s->value[C_TO_SOIL]);
	}

	/* litter and soil compartments */



}

void leaf_fall_evergreen ( cell_t *const c, const int height, const int dbh, const int age, const int species )
{
	static double foliage_to_remove;
	static double fine_root_to_remove;
	double yearly_leaf_fall_falling_C;
	double yearly_fine_root_turnover_C;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_debug_log, "\n**LEAF FALL (turnover) EVERGREEN**\n");

	/************************************************************************************************************/

	if ( c->doy == 1 )
	{
		/* compute annual carbon leaf turnover */
		yearly_leaf_fall_falling_C = s->value[LEAF_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "Annual leaf turnover = %g tC/cell/year\n", yearly_leaf_fall_falling_C);

		/* daily leaf fall */
		foliage_to_remove = yearly_leaf_fall_falling_C / 365;
		logger(g_debug_log, "Daily leaf turnover = %g tC/cell/year\n", foliage_to_remove);

		/* compute carbon fine root turnover */
		yearly_fine_root_turnover_C = s->value[FINE_ROOT_C] * s->value[LEAF_FINEROOT_TURNOVER];
		logger(g_debug_log, "Annual fine root turnover = %g tC/cell/year\n", yearly_fine_root_turnover_C);

		/* daily fine root turnover */
		fine_root_to_remove = yearly_fine_root_turnover_C / 365;
		logger(g_debug_log, "Daily fine root turnover = %g tC/cell/year\n", fine_root_to_remove);
	}

	/*************************************************************************************************************/

	/* update carbon leaf pool */
	s->value[LEAF_C] -= foliage_to_remove;
	logger(g_debug_log, "LEAF_C = %f tC/cell/day\n", s->value[LEAF_C]);

	/* compute daily amount of carbon leaf and fine root to remove */
	s->value[C_LEAF_TO_LITTER] = foliage_to_remove ;
	logger(g_debug_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	/* update carbon fine root pool */
	s->value[FINE_ROOT_C] -= fine_root_to_remove;
	logger(g_debug_log, "FINE_ROOT_C = %g tC/cell/day\n", s->value[FINE_ROOT_C]);

	s->value[C_FINE_ROOT_TO_SOIL] = fine_root_to_remove;
	logger(g_debug_log, "Daily fine root turnover = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_SOIL]);

	logger(g_debug_log, "Daily Carbon turnover to litter before retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_SOIL]);

	/* compute daily amount of carbon to re-translocate before remove leaf and fine root */
	s->value[C_LEAF_TO_RESERVE] = s->value[C_LEAF_TO_LITTER] * fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);

	s->value[C_FINEROOT_TO_RESERVE] = s->value[C_FINE_ROOT_TO_SOIL] * fraction_to_retransl;
	logger(g_debug_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FINEROOT_TO_RESERVE]);

	/* update considering carbon retranslocation */
	s->value[C_LEAF_TO_LITTER] -= s->value[C_LEAF_TO_RESERVE];
	logger(g_debug_log, "Daily leaf turnover after retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	s->value[C_FINE_ROOT_TO_SOIL] -= s->value[C_FINEROOT_TO_RESERVE];
	logger(g_debug_log, "Daily fine root turnover after retranslocation = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_SOIL]);

	/**/
	s->value[C_TO_LEAF] -= s->value[C_LEAF_TO_LITTER];
	s->value[C_TO_FINEROOT] -= s->value[C_FINE_ROOT_TO_SOIL];

	/* considering that both carbon leaf and fine root contribute to the carbon litter and carbon soil pool */
	s->value[C_TO_LITTER] = s->value[C_LEAF_TO_LITTER];
	logger(g_debug_log, "Carbon to litter after retranslocation = %g tC/cell/day\n", s->value[C_TO_LITTER]);
	s->value[C_TO_SOIL] = s->value[C_FINE_ROOT_TO_SOIL];
	logger(g_debug_log, "Carbon to soil after retranslocation = %g tC/cell/day\n", s->value[C_TO_SOIL]);



}




