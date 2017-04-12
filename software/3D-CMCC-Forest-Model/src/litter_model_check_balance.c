/*
 * litter_model_check_balance.c
 *
 *  Created on: 03 apr 2017
 *      Author: alessio
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "check_balance.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;

int check_litter_carbon_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK LITTER CARBON BALANCE************\n");

	/* check complete litter level carbon flux balance */

	/* sum of carbon sources */
	in      = c->daily_litrC;

	/* sum of carbon sinks */
	out     = c->daily_soilC;

	//todo include cwd carbon fluxes ?
	/* sum of current carbon storage */
	store   = c->daily_leaf_litr1C +
			c->daily_leaf_litr2C +
			c->daily_leaf_litr3C +
			c->daily_leaf_litr4C +
			c->daily_froot_litr1C +
			c->daily_froot_litr2C +
			c->daily_froot_litr3C +
			c->daily_froot_litr4C +
			c->daily_fruit_litrC  ;

	balance = in - out - store;

	logger(g_debug_log, "\nCELL CARBON FLUX BALANCE\n");

	if ( ( fabs( balance ) > eps ) && ( c->dos > 1) )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_litrC = %g gC/m2/day\n", c->daily_litrC);
		error_log("\nout\n");
		error_log("c->daily_soilC = %g gC/m2/day\n",c->daily_soilC);
		error_log("\nstore\n");
		error_log("c->daily_leaf_litr1C = %g gC/m2/day\n", c->daily_leaf_litr1C);
		error_log("c->daily_leaf_litr2C = %g gC/m2/day\n", c->daily_leaf_litr2C);
		error_log("c->daily_leaf_litr3C = %g gC/m2/day\n", c->daily_leaf_litr3C);
		error_log("c->daily_leaf_litr4C = %g gC/m2/day\n", c->daily_leaf_litr4C);
		error_log("c->daily_froot_litr1C = %g gC/m2/day\n", c->daily_froot_litr1C);
		error_log("c->daily_froot_litr2C = %g gC/m2/day\n", c->daily_froot_litr2C);
		error_log("c->daily_leaf_litrC = %g gC/m2/day\n", c->daily_froot_litr3C);
		error_log("c->daily_froot_litr4C = %g gC/m2/day\n", c->daily_froot_litr4C);
		error_log("c->daily_fruit_litrC = %g gC/m2/day\n", c->daily_fruit_litrC);
		error_log("\ncarbon in = %g gC/m2/day\n", in);
		error_log("carbon out = %g gC/m2/day\n", out);
		error_log("carbon store = %g gC/m2/day\n", store);
		error_log("carbon_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Litter_ model_daily' carbon balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Litter_ model_daily' carbon balance\n");
	}
	/* ok */
	return 1;
}

int check_litter_carbon_mass_balance(cell_t *const c)
{
	/* DAILY CHECK ON LITTER LEVEL CARBON MASS BALANCE */
	/* check complete litter level carbon mass balance */

	/* sum of sources */
	c->litr_carbon_in    = c->daily_litrC;

	//fixme
	/* sum of sinks */
	c->litr_carbon_out   = 0. /* + c->daily_het_resp + c->daily_soilC */;

	//todo include cwd carbon pool
	/* sum of current storage */
	c->litr_carbon_store = c->leaf_litr1C +
			c->leaf_litr2C  +
			c->leaf_litr3C  +
			c->leaf_litr4C  +
			c->froot_litr1C +
			c->froot_litr2C +
			c->froot_litr3C +
			c->froot_litr4C ;

	/* check carbon pool balance */
	c->litr_carbon_balance = c->litr_carbon_in - c->litr_carbon_out - ( c->litr_carbon_store - c->litr_carbon_old_store );

	logger(g_debug_log, "\nLITTER LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( c->litr_carbon_balance ) > eps ) && ( c->dos > 1 ) )
	{
		error_log("DOS = %d\n", c->dos);
		error_log("\nin = %g gC/m2/day\n", c->litr_carbon_in);
		error_log("daily_litrC = %g gC/m2\n", c->daily_litrC);
		error_log("\nout = %g gC/m2/day\n", c->litr_carbon_out);
		error_log("daily_het_resp = 0 gC/m2/day\n");
		error_log("daily_soilC = 0 gC/m2/day\n");
		error_log("\nstore\n");
		error_log("leaf_litr1C = %g gC/m2/day\n", c->leaf_litr1C);
		error_log("leaf_litr2C = %g gC/m2/day\n", c->leaf_litr2C);
		error_log("leaf_litr3C = %g gC/m2/day\n", c->leaf_litr3C);
		error_log("leaf_litr4C = %g gC/m2/day\n", c->leaf_litr4C);
		error_log("froot_litr1C = %g gC/m2/day\n", c->froot_litr1C);
		error_log("froot_litr2C = %g gC/m2/day\n", c->froot_litr2C);
		error_log("froot_litr3C = %g gC/m2/day\n",c->froot_litr3C);
		error_log("froot_litr4C = %g gC/m2/day\n", c->froot_litr4C);
		error_log("\ncarbon in = %g gC/m2/day\n", c->litr_carbon_in);
		error_log("carbon out = %g gC/m2/day\n", c->litr_carbon_out);
		error_log("delta carbon store = %g gC/m2/day\n", c->litr_carbon_store - c->litr_carbon_old_store);
		error_log("carbon_balance = %g gC/m2/day\n",c->litr_carbon_balance);
		error_log("...FATAL ERROR in 'Litter_model_daily' carbon mass balance (gC/m2/day) (exit)\n");
		CHECK_CONDITION(fabs( c->litr_carbon_balance ), > , eps);

		return 0;
	}
	else
	{
		c->litr_carbon_old_store = c->litr_carbon_store;
		logger(g_debug_log, "...ok in 'Cell_model_daily' carbon mass balance (gC/m2/day)\n");
	}
	/* ok */
	return 1;

}

/******************************************************************************************/
/******************************************************************************************/

int check_litter_nitrogen_flux_balance(cell_t *const c)
{
	double in;
	double out;
	double store;
	double balance;

	logger(g_debug_log, "\n*********CHECK LITTER NITROGEN BALANCE************\n");

	/* check complete litter level nitrogen flux balance */

	//todo include cwd nitrogen fluxes

	/* sum of nitrogen sources */
	in      = c->daily_litrN  /* fixme include Ndepostiion id daily */;

	/* sum of nitrogen sinks */
	out     = c->daily_soilN;

	/* sum of current nitrogen storage */
	store   = c->daily_leaf_litr1N +
			c->daily_leaf_litr2N +
			c->daily_leaf_litr3N +
			c->daily_leaf_litr4N +
			c->daily_froot_litr1N +
			c->daily_froot_litr2N +
			c->daily_froot_litr3N +
			c->daily_froot_litr4N +
			c->daily_fruit_litrN  ;

	balance = in - out - store;

	logger(g_debug_log, "\nCELL NITROGEN FLUX BALANCE\n");

	if ( ( fabs( balance ) > eps ) && ( c->dos > 1) )
	{
		error_log("DOY = %d\n", c->doy);
		error_log("\nin\n");
		error_log("c->daily_litrC = %g gC/m2/day\n", c->daily_litrC);
		error_log("\nout\n");
		error_log("c->daily_soilC = %g gC/m2/day\n",c->daily_soilC);
		error_log("\nstore\n");
		error_log("c->daily_leaf_litr1N = %g gC/m2/day\n", c->daily_leaf_litr1N);
		error_log("c->daily_leaf_litr2N = %g gC/m2/day\n", c->daily_leaf_litr2N);
		error_log("c->daily_leaf_litr3N = %g gC/m2/day\n", c->daily_leaf_litr3N);
		error_log("c->daily_leaf_litr4N = %g gC/m2/day\n", c->daily_leaf_litr4N);
		error_log("c->daily_froot_litr1N = %g gC/m2/day\n", c->daily_froot_litr1N);
		error_log("c->daily_froot_litr2N = %g gC/m2/day\n", c->daily_froot_litr2N);
		error_log("c->daily_leaf_litrC = %g gC/m2/day\n", c->daily_froot_litr3N);
		error_log("c->daily_froot_litr4N = %g gC/m2/day\n", c->daily_froot_litr4N);
		error_log("c->daily_fruit_litrC = %g gC/m2/day\n", c->daily_fruit_litrC);
		error_log("\nnitrogen in = %g gC/m2/day\n", in);
		error_log("nitrogen out = %g gC/m2/day\n", out);
		error_log("nitrogen store = %g gC/m2/day\n", store);
		error_log("nitrogen_balance = %g gC/m2/day\n",balance);
		error_log("...FATAL ERROR in 'Litter_ model_daily' nitrogen balance (exit)\n");

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Litter_ model_daily' nitrogen balance\n");
	}
	/* ok */
	return 1;
}
