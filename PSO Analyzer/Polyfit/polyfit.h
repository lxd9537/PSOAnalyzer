#ifndef POLYFIT_H
#define POLYFIT_H

#include <vector>

void polyfit(const double *x, const double *y, unsigned int length, unsigned int poly_n, double *factors);
void gauss_solve(unsigned int n, std::vector<double>& AA, std::vector<double>& xx, std::vector<double>& bb);

#endif
