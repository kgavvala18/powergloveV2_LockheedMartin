#pragma once

#include <math.h>
#include <stdbool.h>
#include <string.h>

#ifndef _float_t
#define _float_t float
#endif

// Linear alegbra ////////////////////////////////////////////////////////////

/// @private
static void _mulmat(
    const _float_t *a,
    const _float_t *b,
    _float_t *c,
    const int arows,
    const int acols,
    const int bcols)
{
    for (int i = 0; i < arows; ++i)
    {
        for (int j = 0; j < bcols; ++j)
        {
            c[i * bcols + j] = 0;
            for (int k = 0; k < acols; ++k)
            {
                c[i * bcols + j] += a[i * acols + k] * b[k * bcols + j];
            }
        }
    }
}

/// @private
static void _mulvec(
    const _float_t *a,
    const _float_t *x,
    _float_t *y,
    const int m,
    const int n)
{
    for (int i = 0; i < m; ++i)
    {
        y[i] = 0;
        for (int j = 0; j < n; ++j)
            y[i] += x[j] * a[i * n + j];
    }
}

/// @private
static void _transpose(
    const _float_t *a, _float_t *at, const int m, const int n)
{
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
        {
            at[j * m + i] = a[i * n + j];
        }
}

/// @private
static void _addmat(
    const _float_t *a, const _float_t *b, _float_t *c,
    const int m, const int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            c[i * n + j] = a[i * n + j] + b[i * n + j];
        }
    }
}

/// @private
static void _negate(_float_t *a, const int m, const int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            a[i * n + j] = -a[i * n + j];
        }
    }
}

/// @private
static void _addeye(_float_t *a, const int n)
{
    for (int i = 0; i < n; ++i)
    {
        a[i * n + i] += 1;
    }
}

/* Cholesky-decomposition matrix-inversion code, adapated from
http://jean-pierre.moreau.pagesperso-orange.fr/Cplus/_choles_cpp.txt */

/// @private
static int _choldc1(_float_t *a, _float_t *p, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = i; j < n; j++)
        {
            _float_t sum = a[i * n + j];
            for (int k = i - 1; k >= 0; k--)
            {
                sum -= a[i * n + k] * a[j * n + k];
            }
            if (i == j)
            {
                if (sum <= 0)
                {
                    return 1; /* error */
                }
                p[i] = sqrt(sum);
            }
            else
            {
                a[j * n + i] = sum / p[i];
            }
        }
    }

    return 0; // success:w
}

/// @private
static int _choldcsl(const _float_t *A, _float_t *a, _float_t *p, const int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            a[i * n + j] = A[i * n + j];
        }
    }
    if (_choldc1(a, p, n))
    {
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        a[i * n + i] = 1 / p[i];
        for (int j = i + 1; j < n; j++)
        {
            _float_t sum = 0;
            for (int k = i; k < j; k++)
            {
                sum -= a[j * n + k] * a[k * n + i];
            }
            a[j * n + i] = sum / p[j];
        }
    }

    return 0; // success
}

/// @private
static int _cholsl(const _float_t *A, _float_t *a, _float_t *p, const int n)
{
    if (_choldcsl(A, a, p, n))
    {
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            a[i * n + j] = 0.0;
        }
    }
    for (int i = 0; i < n; i++)
    {
        a[i * n + i] *= a[i * n + i];
        for (int k = i + 1; k < n; k++)
        {
            a[i * n + i] += a[k * n + i] * a[k * n + i];
        }
        for (int j = i + 1; j < n; j++)
        {
            for (int k = j; k < n; k++)
            {
                a[i * n + j] += a[k * n + i] * a[k * n + j];
            }
        }
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
            a[i * n + j] = a[j * n + i];
        }
    }

    return 0; // success
}

/// @private
static void _addvec(
    const _float_t *a, const _float_t *b, _float_t *c, const int n)
{
    for (int j = 0; j < n; ++j)
    {
        c[j] = a[j] + b[j];
    }
}

/// @private
static void _sub(
    const _float_t *a, const _float_t *b, _float_t *c, const int n)
{
    for (int j = 0; j < n; ++j)
    {
        c[j] = a[j] - b[j];
    }
}