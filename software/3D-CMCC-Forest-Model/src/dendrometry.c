/* dendrometry.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

void Get_dendrometry (SPECIES *const s, HEIGHT *const h, int count)
{
	float oldavDBH;
	float oldTreeHeight;

	Log("\nGET_DENDROMETRY_ROUTINE\n");

	Log("\n**Average DBH 3PG**\n");
	Log("**Tree Height from CC function**\n");

	/*compute Tree AVDBH*/

	oldavDBH = s->value[AVDBH];

	//Log("OLD Average DBH from 3PG CLASSIC from previous year = %g cm\n", oldavDBH);

	oldTreeHeight = h->value;

	//Log("OLD Tree Height from Chapman-Richards function from previous year = %g m\n", oldTreeHeight);

	if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
	{
		//use generic stemconst stempower values

		if (oldavDBH < 9)
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS] / s->value[STEMCONST]), (1.0 / STEMPOWER_A));
		}
		else if (oldavDBH > 9 && oldavDBH < 15)
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS] / s->value[STEMCONST]), (1.0 / STEMPOWER_B));
		}
		else
		{
			s->value[AVDBH] = pow((s->value[AV_STEM_MASS] / s->value[STEMCONST]), (1.0 / STEMPOWER_C));
		}
	}
	else
	{
		//use site specific stemconst stempower values
		Log("Using site related stemconst stempower\n");
		s->value[AVDBH] = pow(s->value[AV_STEM_MASS] / s->value[STEMCONST_P], ( 1.0 / s->value[STEMPOWER_P]));
		//s->value[AV_STEM_MASS]  = s->value[AV_STEM_MASS] = pow ((s->value[STEMCONST_P] * s->value[AVDBH]), s->value[STEMPOWER_P]);

	}
	Log("-New Average DBH from 3PG CLASSIC = %g cm\n", s->value[AVDBH]);

	/*control*/
	if (oldavDBH > s->value[AVDBH])
	{
		Log("Old AVDBH = %g cm\n", oldavDBH);
		Log("ERROR in Average DBH !!!!!!!\n");
		s->value[AVDBH] = oldavDBH;
	}
	else
	{
		Log("-DBH increment from 3PG CLASSIC = %g cm\n", s->value[AVDBH] - oldavDBH);

	}



	/*compute Tree Height*/
	/*using Chapman_Richards Function*/

	//the terms 1.3 in C-R Function is breast height (1.3 m)
	//CRA, CRB, CRC are species-specific regression coefficients
	//CRA coefficient is the asymptotic maximum height this coefficient represents the theoretic maximum height obtainable
	//for a given stand or plot, this parameter could be used to represent maximum tree height. By modifying equation so that CRA changes
	//as a function of stand age, site index, dominant height, or stand density, equation can be generalized to be more applicable to a wider range of
	//sites and stand ages, or more regionally applied
	//CRB represents exponential decay parameter
	//CRC represents shape parameters
	h->value = 1.3 + s->value[CRA] * pow (1.0 - exp ( - s->value[CRB] *  s->value[AVDBH]) , s->value[CRC]);
	Log("-Tree Height using Chapman-Richard function = %g m\n", h->value);

	if (h->value > s->value[HMAX])
	{
		Log("Chapaman-Richards function for tree height exceeds HMAX\n");
		h->value = s->value[HMAX];
	}
	/*Tree Height using SORTIE */
	if (s->value[AVDBH]> 10)
	{
		//Log("SORTIE FUNC DBH > 10 cm\n");

		s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1.0 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
		Log("-Tree Height using Sortie function > 10 cm = %g m\n", s->value[TREE_HEIGHT_SORTIE]);
	}
	else
	{
		//Log("SORTIE FUNC DBH < 10 cm\n");

		s->value[TREE_HEIGHT_SORTIE] = 0.1 + 30 *( 1.0 - exp ( - 0.03 * s->value[AVDBH] ));
		Log("-Tree Height using Sortie function < 10 cm= %g m\n", s->value[TREE_HEIGHT_SORTIE]);
	}




	/*control*/
	if ( oldTreeHeight > h->value)
	{
		h->value = oldTreeHeight;
		Log("ERROR in TREE HEIGHT!!!\n");
	}

}
