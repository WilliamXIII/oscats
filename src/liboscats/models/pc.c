/* OSCATS: Open-Source Computerized Adaptive Testing System
 * Partial Credit IRT Model
 * Copyright 2011 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:pc
 * @title:OscatsModelPc
 * @short_description: Generalized Partial Credit Model
 */

#include <math.h>
#include <gsl/gsl_vector.h>
#include "models/pc.h"

G_DEFINE_TYPE(OscatsModelPc, oscats_model_pc, OSCATS_TYPE_MODEL);

/* There are Ncat+1 response categories, 0, 1, ..., Ncat.
 * z_0 = 0, so there are Ncat location parameters, numbered 1, ..., Ncat.
 */
#define PARAM_B(k) ((k)-1)
#define PARAM_D(j) (Ncat+(j))

enum
{
  PROP_0,
  PROP_NUM_CAT,
};

static void model_constructed (GObject *object);
static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec);
static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec);
static OscatsResponse get_max (const OscatsModel *model);
static gdouble P(const OscatsModel *model, OscatsResponse resp, const OscatsPoint *theta, const OscatsCovariates *covariates);
//static gdouble distance(const OscatsModel *model, const OscatsPoint *theta, const OscatsCovariates *covariates);
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf);
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes);

static void oscats_model_pc_class_init (OscatsModelPcClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  OscatsModelClass *model_class = OSCATS_MODEL_CLASS(klass);
  GParamSpec *pspec;
  
  gobject_class->constructed = model_constructed;
  gobject_class->set_property = model_set_property;
  gobject_class->get_property = model_get_property;

  model_class->get_max = get_max;
  model_class->P = P;
//  model_class->distance = distance;
  model_class->logLik_dtheta = logLik_dtheta;
  model_class->logLik_dparam = logLik_dparam;
  
/**
 * OscatsModelPc:Ncat:
 *
 * The number of response categories in the IRT model,
 * numered 0, 1, ..., Ncat-1.  The default number of categories is 3,
 * which is also the minimum.
 */
  pspec = g_param_spec_uint("Ncat", "Number of Categories", 
                            "Number of response categories in the IRT model",
                            3, G_MAXUINT, 3,
                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                            G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                            G_PARAM_STATIC_BLURB);
  g_object_class_install_property(gobject_class, PROP_NUM_CAT, pspec);

}

static void oscats_model_pc_init (OscatsModelPc *self)
{
}

static void model_constructed(GObject *object)
{
  GString *str;
  guint i, k, Ncat;
  OscatsModel *model = OSCATS_MODEL(object);
  G_OBJECT_CLASS(oscats_model_pc_parent_class)->constructed(object);
  Ncat = OSCATS_MODEL_PC(model)->Ncat;
  
  // Set up parameters
  model->Np = Ncat + model->Ncov;
  model->params = g_new0(gdouble, model->Np);

  // Set up parameter names
  str = g_string_sized_new(10);
  model->names = g_new(GQuark, model->Np);
  for (k=1; k <= Ncat; k++)
  {
    g_string_printf(str, "Diff.%d", k);
    model->names[PARAM_B(k)] = g_quark_from_string(str->str);
  }
  g_string_free(str, TRUE);
  for (k=PARAM_D(0), i=0; i < model->Ncov; k++, i++)
    model->names[k] = model->covariates[i];

  // Check subspace type
  if (model->dimType != OSCATS_DIM_CONT)
  {
    model->Ndims = 0;
    g_critical("OscatsModelPc requires a continuous latent space.");
  }

}

static void model_set_property(GObject *object, guint prop_id,
                               const GValue *value, GParamSpec *pspec)
{
  OscatsModelPc *self = OSCATS_MODEL_PC(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:			// construction only
      self->Ncat = g_value_get_uint(value)-1;
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void model_get_property(GObject *object, guint prop_id,
                               GValue *value, GParamSpec *pspec)
{
  OscatsModelPc *self = OSCATS_MODEL_PC(object);
  switch (prop_id)
  {
    case PROP_NUM_CAT:
      g_value_set_uint(value, self->Ncat+1);
      break;
    
    default:
      // Unknown property
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static OscatsResponse get_max (const OscatsModel *model)
{
  return ((OscatsModelPc*)model)->Ncat;
}

static gdouble P(const OscatsModel *model, OscatsResponse resp,
                 const OscatsPoint *theta, const OscatsCovariates *covariates)
{
  guint *dims = model->shortDims;
  guint dim1, dim2;
  guint Ncat = ((OscatsModelPc*)model)->Ncat;
  guint i, I, k;
  gdouble z[Ncat], denom=1, cov=0;
  g_return_val_if_fail(resp <= Ncat, 0);
  switch (model->Ndims)
  {
    case 2:
      dim1 = dims[0];
      dim2 = dims[1];
      for (k=0; k < Ncat; k++)
        z[k] = theta->cont[dim1] + theta->cont[dim2]
             - model->params[PARAM_B(k+1)];
      break;
    case 1:
      dim1 = dims[0];
      for (k=0; k < Ncat; k++)
        z[k] = theta->cont[dim1] - model->params[PARAM_B(k+1)];
      break;
    
    default:
      for (k=0; k < Ncat; k++)
      {
        z[k] = -model->params[PARAM_B(k+1)];
        for (i=0; i < model->Ndims; i++)
          z[k] += theta->cont[dims[i]];
      }
  }
  for (i=0, I=PARAM_D(0); i < model->Np; i++, I++)
    cov += oscats_covariates_get(covariates, model->covariates[i]) * model->params[I];
  for (k=0; k < Ncat; k++)
    denom += exp(z[k]+cov);
  return (resp == 0 ? 1 : exp(z[resp-1]+cov)) / denom;
}

/* z_k = k sum_i theta_i - sum_h^k b_h + sum_l d_l cov_l, z_0 = 0
 * P_k = exp(z_k) / [ sum_h^Ncat exp(z_h) ]
 * d log P_k / dA = dz_k/dA - sum_x P_x dz_x/dA
 * d log P / dAdB = sum_x P_x dz_x/dA [ sum_y P_y dz_y/dB - dz_x/dB ]
 *
 * dz_k/dtheta_i = k a_i
 *
 * Let E = sum_x x P_x, V = sum_x x^2 P_x.
 * d log P_k / dtheta_i = (k - E)
 * d log P_k / dtheta_i dtheta_j = (E^2 - V)
 */
static void logLik_dtheta(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes, gboolean Inf)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint Ndims = model->Ndims;
  guint Ncat = ((OscatsModelPc*)model)->Ncat;
  guint i, j, I, J;
  guint hes_stride = (hes ? hes_v->tda : 0);
  gdouble grad_val=0, hes_val=0;
  g_return_if_fail(resp <= Ncat);

  for (i=1; i <= Ncat; i++)
  {
    gdouble p = P(model, i, theta, covariates);
    grad_val += i*p;		// E = sum_x x P_x
    hes_val += i*i*p;		// V = sum_x x^2 P_x
  }
  hes_val = grad_val*grad_val - hes_val;	// (E^2 - V)
  grad_val = resp - grad_val;			// (k - E)
  if (Inf)
    hes_val *= -P(model, resp, theta, covariates);

  switch (Ndims)
  {
    case 2:
      J = model->shortDims[1];
      I = model->shortDims[0];
      if (grad) grad_v->data[J] += grad_val;
      if (hes)
      {
        hes_v->data[J*hes_stride+J] += hes_val;
        hes_v->data[I*hes_stride+J] += hes_val;
        hes_v->data[J*hes_stride+I] += hes_val;
      }
    case 1:
      I = model->shortDims[0];
      if (grad) grad_v->data[I] += grad_val;
      if (hes)
        hes_v->data[I*hes_stride+I] += hes_val;
      break;
    
    default:
      for (i=0; i < Ndims; i++)
      {
        I = model->shortDims[i];
        if (grad) grad_v->data[I] += grad_val;
        if (hes)
        {
          hes_v->data[I*hes_stride+I] += hes_val;
          for (j=i+1; j < Ndims; j++)
          {
            J = model->shortDims[j];
            hes_v->data[I*hes_stride+J] += hes_val;
            hes_v->data[J*hes_stride+I] += hes_val;
          }
        }
      }
  }
}

/* z_k = k sum_i theta_i - sum_h^k b_h + sum_l d_l cov_l, z_0 = 0
 * P_k = exp(z_k) / [ sum_h^Ncat exp(z_h) ]
 * d log P_k / dA = dz_k/dA - sum_x P_x dz_x/dA
 * d log P_k / dAdB = sum_x P_x dz_x/dA [ sum_y P_y dz_y/dB - dz_x/dB ]
 *
 * dz_k/db_i = - I_{i <= k}
 * dz_k/dd_i = cov_i
 *
 * D[log P_k, b_i] = sum_{x=i}^Ncat P_x - I_{i <= k}
 * D[log P_k, d_i] = cov_i (P0 - I_{k=0})
 *
 * D[log P_k, b_i, b_j] = (sum_{x=i}^Ncat P_x)(sum_{x=j}^Ncat P_x)
 *                        - sum_{x=max(i,j)}^Ncat P_x
 * D[log P_k, b_i, d_j] = cov_j P_0 (sum_{x=i}^Ncat P_x)
 * D[log P_k, d_i, d_j] = cov_i cov_j P_0 (P_0 - 1)
 */
static void logLik_dparam(const OscatsModel *model, OscatsResponse resp,
                          const OscatsPoint *theta, const OscatsCovariates *covariates,
                          GGslVector *grad, GGslMatrix *hes)
{
  gsl_vector *grad_v = (grad ? grad->v : NULL);
  gsl_matrix *hes_v = (hes ? hes->v : NULL);
  guint Ncov = model->Ncov;
  guint Ncat = ((OscatsModelPc*)model)->Ncat;
  gdouble p[Ncat+1], cumSum[Ncat+1], cov_i, tmp;
  guint i, j, I, J;
  guint hes_stride = (hes ? hes_v->tda : 0);
  g_return_if_fail(resp <= Ncat);

  p[0] = P(model, 0, theta, covariates);
  for (i=1; i <= Ncat; i++)
  {
    p[i] = P(model, i, theta, covariates);
    cumSum[i] = p[i];
    for (j=i-1; j > 0; j--)
      cumSum[j] += p[i];
  }

#define HES(x,y) (hes_v->data[(x)*hes_stride+(y)])

  // b_i
  for (i=1, I=PARAM_B(1); i <= Ncat; i++, I++)
  {
    if (grad) grad_v->data[I] += (resp >= i ? cumSum[i] - 1 : cumSum[i]);
    if (hes)
    {
      HES(I,I) += cumSum[i] * (cumSum[i] - 1);
      // b_i, b_j
      for (j=i+1, J=I+1; j <= Ncat; j++, J++)
      {
        tmp = cumSum[j] * (cumSum[i] - 1);
        HES(I,J) += tmp;
        HES(J,I) += tmp;
      }
      // b_i, d_j
      for (j=0, J=PARAM_D(0); j < Ncov; j++, J++)
      {
        tmp = p[0] * cumSum[i] * 
              oscats_covariates_get(covariates, model->covariates[j]);
        HES(I,J) += tmp;
        HES(J,I) += tmp;
      }
    }
  }
  
  // d_i
  for (i=0, I=PARAM_D(0); i < Ncov; i++, I++)
  {
    cov_i = oscats_covariates_get(covariates, model->covariates[i]);
    if (grad) grad_v->data[i] += cov_i * (resp == 0 ? p[0]-1 : p[0]);
    if (hes)
    {
      HES(I,I) += cov_i * cov_i * p[0] * (p[0] - 1);
      // d_i, d_j
      for (j=i+1, J=I+1; j < Ncov; j++, J++)
      {
        tmp = cov_i * p[0] * (p[0] - 1) *
              oscats_covariates_get(covariates, model->covariates[j]);
        HES(I,J) += tmp;
        HES(J,I) += tmp;
      }
    }
  }

}
