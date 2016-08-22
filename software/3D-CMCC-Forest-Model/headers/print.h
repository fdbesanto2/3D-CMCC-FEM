/* print.h */
#ifndef PRINT_H_
#define PRINT_H_

#include "matrix.h"

void Print_parameters(species_t *const s, const int species_count, const int day, const int month, const int years);

void print_cell_data(cell_t* const c);

void print_forest_class_data(cell_t* const c, const int layer, const int height, const int age, const int species);

void print_daily_met_data(cell_t *c, const int day, const int month, const int year);

#endif /* PRINT_H_ */