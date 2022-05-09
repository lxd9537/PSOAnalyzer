#include"polyfit.h"
#include"qglobal.h"
using namespace std;

void polyfit(const double *x, const double *y, unsigned int length, unsigned int poly_n, double *factors)
{
    unsigned int i,j;
	std::vector<double> factor;

	if (poly_n > length || poly_n < 1 || length < 1)
        return;

    factor.resize(poly_n + 1, 0);

	//double *tempx,*tempy,*sumxx,*sumxy,*ata;
	std::vector<double> tempx(length, 1.0);
	std::vector<double> tempy(y, y + length);
	std::vector<double> sumxx(poly_n * 2 + 1);
	std::vector<double> sumxy(poly_n + 1);
	std::vector<double> ata((poly_n + 1)*(poly_n + 1));

    for (i = 0; i<2 * poly_n + 1; i++)
	{
        for (sumxx[i] = 0, j = 0; j<length; j++)
		{
			sumxx[i] += tempx[j];
			tempx[j] *= x[j];
		}
	}
    for (i = 0; i<poly_n + 1; i++)
	{
		for (sumxy[i] = 0, j = 0; j<length; j++)
		{
			sumxy[i] += tempy[j];
			tempy[j] *= x[j];
		}
	}
    for (i = 0; i<poly_n + 1; i++)
    {
        for (j = 0; j<poly_n + 1; j++)
        {
            ata[i*(poly_n + 1) + j] = sumxx[i + j];
        }
    }

	gauss_solve(poly_n + 1, ata, factor, sumxy);

    for (i = 0; i < poly_n + 1; i++)
		*(factors + i) = factor.at(i);

}

void gauss_solve(unsigned int n, std::vector<double>& AA, std::vector<double>& xx, std::vector<double>& bb)
{
	double* A;
	double* x;
	double* b;
    unsigned int i, j, k, r;
    int t,q,m;
	double max;

    A = &AA[0];
	x = &xx[0];
	b = &bb[0];

	for (k = 0; k<n - 1; k++)
	{
        max = qAbs(A[k*n + k]); /*find maxmum*/
		r = k;
		for (i = k + 1; i<n - 1; i++)
		{
            if (max<qAbs(A[i*n + i]))
			{
                max = qAbs(A[i*n + i]);
				r = i;
			}
		}
		if (r != k)
		{
			for (i = 0; i<n; i++)         /*change array:A[k]&A[r] */
			{
				max = A[k*n + i];
				A[k*n + i] = A[r*n + i];
				A[r*n + i] = max;
            }
		}
		max = b[k];                    /*change array:b[k]&b[r]     */
		b[k] = b[r];
		b[r] = max;
		for (i = k + 1; i<n; i++)
		{
			for (j = k + 1; j<n; j++)
				A[i*n + j] -= A[i*n + k] * A[k*n + j] / A[k*n + k];
			b[i] -= A[i*n + k] * b[k] / A[k*n + k];
		}
	}

    m = static_cast<int>(n);

    for (t = m - 1; t >= 0; x[t] /= A[t*m + t], t--)
        for (q = t + 1, x[t] = b[t]; q<m; q++)
            x[t] -= A[t*m + q] * x[q];
}
