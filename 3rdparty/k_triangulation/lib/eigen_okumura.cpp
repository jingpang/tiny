/* ************************************************************** qr.cpp *** *
 * QRˡ�ˤ���ͭ�͡���ͭ�٥��ȥ�η׻��ؿ�(C����ˤ��ǿ����르�ꥺ�༭ŵ)
 *
 * Copyright (C) 2006 Yasuyuki SUGAYA <sugaya@iim.ics.tut.ac.jp>
 *
 *                                    Time-stamp: <07/09/12 10:19:31 sugaya>
 * ************************************************************************* */
#include <cstdlib>
#include <cstring>
#include "matrix.h"
#include "matutil.h"      /* �����Ѿ�ƻ�� */

#define EPS         1E-6  /* ���г���ʬ�ε��Ƹ��� */
#define MAX_ITER    100   /* ����η��֤��� */

double house(int n, vector x)  /* Householder�Ѵ� */
{
    int i;
    double s, t;

    s = sqrt(innerproduct(n, x, x));  /* ���Ѥ�ʿ����, ���ʤ���礭�� */
    if (s != 0) {
        if (x[0] < 0) s = -s;
        x[0] += s;  t = 1 / sqrt(x[0] * s);
        for (i = 0; i < n; i++) x[i] *= t;
    }
    return -s;
}

void tridiagonalize(int n, matrix a, vector d, vector e) /* 3���гѲ� */
{
    int i, j, k;
    double s, t, p, q;
    vector v, w;

    for (k = 0; k < n - 2; k++) {
        v = a[k];  d[k] = v[k];
        e[k] = house(n - k - 1, &v[k + 1]);
        if (e[k] == 0) continue;
        for (i = k + 1; i < n; i++) {
            s = 0;
            for (j = k + 1; j < i; j++) s += a[j][i] * v[j];
            for (j = i;     j < n; j++) s += a[i][j] * v[j];
            d[i] = s;
        }
        t = innerproduct(n-k-1, &v[k+1], &d[k+1]) / 2;
        for (i = n - 1; i > k; i--) {
            p = v[i];  q = d[i] - t * p;  d[i] = q;
            for (j = i; j < n; j++)
                a[i][j] -= p * d[j] + q * v[j];
        }
    }
    if (n >= 2) {  d[n - 2] = a[n - 2][n - 2];
                   e[n - 2] = a[n - 2][n - 1];  }
    if (n >= 1)    d[n - 1] = a[n - 1][n - 1];
    for (k = n - 1; k >= 0; k--) {
        v = a[k];
        if (k < n - 2) {
            for (i = k + 1; i < n; i++) {
                w = a[i];
                t = innerproduct(n-k-1, &v[k+1], &w[k+1]);
                for (j = k + 1; j < n; j++)
                    w[j] -= t * v[j];
            }
        }
        for (i = 0; i < n; i++) v[i] = 0;
        v[k] = 1;
    }
}

int eigen(int n, matrix a, vector d, vector e)
{
    int i, j, k, h, iter;
    double c, s, t, w, x, y;
    vector v;

    tridiagonalize(n, a, d, &e[1]);  /* 3���гѲ� */
    e[0] = 0;  /* �ֿ� */
    for (h = n - 1; h > 0; h--) {  /* ����Υ������򾮤������Ƥ��� */
        j = h;
        while (fabs(e[j]) > EPS * (fabs(d[j - 1]) + fabs(d[j])))
            j--;  /* $\mbox{\tt e[$j$]} \ne 0$ �Υ֥��å��λ����򸫤Ĥ��� */
        if (j == h) continue;
        iter = 0;
        do {
            if (++iter > MAX_ITER) return EXIT_FAILURE;
            w = (d[h - 1] - d[h]) / 2;
            t = e[h] * e[h];
            s = sqrt(w * w + t);  if (w < 0) s = -s;
            x = d[j] - d[h] + t / (w + s);  y = e[j + 1];
            for (k = j; k < h; k++) {
                if (fabs(x) >= fabs(y)) {
                    t = -y / x;  c = 1 / sqrt(t * t + 1);
                    s = t * c;
                } else {
                    t = -x / y;  s = 1 / sqrt(t * t + 1);
                    c = t * s;
                }
                w = d[k] - d[k + 1];
                t = (w * s + 2 * c * e[k + 1]) * s;
                d[k] -= t;  d[k + 1] += t;
                if (k > j) e[k] = c * e[k] - s * y;
                e[k + 1] += s * (c * w - 2 * s * e[k + 1]);
                /* ����5�Ԥϸ�ͭ�٥��ȥ����ʤ��ʤ����� */
                for (i = 0; i < n; i++) {
                    x = a[k][i];  y = a[k + 1][i];
                    a[k    ][i] = c * x - s * y;
                    a[k + 1][i] = s * x + c * y;
                }
                if (k < h - 1) {
                    x = e[k + 1];  y = -s * e[k + 2];
                    e[k + 2] *= c;
                }
            }
        } while (fabs(e[h]) >
                EPS * (fabs(d[h - 1]) + fabs(d[h])));
    }
/*
  �ʲ��ϸ�ͭ�ͤι߽�����󤷤Ƥ������. ɬ�פʤ���оʤ�.
  ��ͭ�٥��ȥ����ʤ��ʤ��ͭ�٥��ȥ������Ϥ����������.
  �ʤ�, {\tt matutil.c} ��ǹ���γƹԤ�٥��ȥ�ؤΥݥ���
  �Ȥ���������Ƥ���Τ�, �Ը򴹤ϥݥ��󥿤Τ����ؤ������ǺѤ�.
*/
    for (k = 0; k < n - 1; k++) {
        h = k;  t = d[h];
        for (i = k + 1; i < n; i++)
            if (d[i] > t) {  h = i;  t = d[h];  }
        d[h] = d[k];  d[k] = t;
        v = a[h];  a[h] = a[k];  a[k] = v;
    }
    return EXIT_SUCCESS;
}

/* ************************************************************************* */
bool
Matrix::Eigen (Vector	&eval,
	       Matrix	&evec,
	       double	eps) const {
  matrix	A;
  vector	d, e;
  int		i, j;

  eval.Initialize (row);
  evec.Initialize (row, row);
  
  A = new_matrix (row, col);
  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      A[i][j] = data_ptr[i][j];
    }
  }
  d = new_vector (row);
  e = new_vector (row);

  eigen (row, A, d, e);

  for (i = 0; i < row; i++) {
    for (j = 0; j < col; j++) {
      evec[i][j] = A[j][i];
    }
    eval[i] = d[i];
  }
  free_vector (e);
  free_vector (d);
  free_matrix (A);
  
  return true;
}