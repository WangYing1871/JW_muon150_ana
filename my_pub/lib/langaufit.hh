#ifndef langaufit_H
#define langaufit_H

#include "TMath.h"
#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"


using namespace std;

Double_t langaufun(Double_t *x, Double_t *par)
{
    //Fit parameters:
    //par[0]=Width (scale) parameter of Landau density
    //par[1]=Most Probable (MP, location) parameter of Landau density
    //par[2]=Total area (integral -inf to inf, normalization constant)
    //par[3]=Width (sigma) of convoluted Gaussian function
    //
    //In the Landau distribution (represented by the CERNLIB approximation),
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.

    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;  // (2 pi)^(-1/2)
    Double_t mpshift = -0.22278298;       // Landau maximum location

    // Control constants
    Double_t np = 100.0;  // number of convolution steps
    Double_t sc = 5.0;    // convolution extends to +-sc Gaussian sigmas

    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow, xupp;
    Double_t step;
    Double_t i;

    // MP shift correction
    mpc = par[1] - mpshift * par[0];

    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];

    step = (xupp - xlow) / np;

    // Convolution integral of Landau and Gaussian by sum
    for (i = 1.0; i <= np / 2; i++)
    {
        xx = xlow + (i - .5) * step;
        fland = TMath::Landau(xx, mpc, par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0], xx, par[3]);

        xx = xupp - (i - .5) * step;
        fland = TMath::Landau(xx, mpc, par[0]) / par[0];
        sum += fland * TMath::Gaus(x[0], xx, par[3]);
    }

    return (par[2] * step * sum * invsq2pi / par[3]);
}

TF1 *langaufit(TH1F *his, Double_t *fitrange, Double_t *startvalues, Double_t *parlimitslo,
                                Double_t *parlimitshi, Double_t *fitparams, Double_t *fiterrors, Double_t *ChiSqr,
                                Int_t *NDF)
{
    // Once again, here are the Landau * Gaussian parameters:
    //   par[0]=Width (scale) parameter of Landau density
    //   par[1]=Most Probable (MP, location) parameter of Landau density
    //   par[2]=Total area (integral -inf to inf, normalization constant)
    //   par[3]=Width (sigma) of convoluted Gaussian function
    //
    // Variables for langaufit call:
    //   his             histogram to fit
    //   fitrange[2]     lo and hi boundaries of fit range
    //   startvalues[4]  reasonable start values for the fit
    //   parlimitslo[4]  lower parameter limits
    //   parlimitshi[4]  upper parameter limits
    //   fitparams[4]    returns the final fit parameters
    //   fiterrors[4]    returns the final fit errors
    //   ChiSqr          returns the chi square
    //   NDF             returns ndf

    Int_t i;
    Char_t FunName[100];

    sprintf(FunName, "Fitfcn_%s", his->GetName());

    TF1 *ffitold = (TF1 *)gROOT->GetListOfFunctions()->FindObject(FunName);
    if (ffitold)
        delete ffitold;

    TF1 *ffit = new TF1(FunName, langaufun, fitrange[0], fitrange[1], 4);
    ffit->SetParameters(startvalues);
    ffit->SetParNames("Width", "MP", "Area", "GSigma");

    for (i = 0; i < 4; i++)
    {
        ffit->SetParLimits(i, parlimitslo[i], parlimitshi[i]);
    }

    his->Fit(FunName, "RB0Q");  // fit within specified range, use ParLimits, do not plot

    ffit->GetParameters(fitparams);  // obtain fit parameters
    for (i = 0; i < 4; i++)
    {
        fiterrors[i] = ffit->GetParError(i);  // obtain fit parameter errors
    }
    ChiSqr[0] = ffit->GetChisquare();  // obtain chi^2
    NDF[0] = ffit->GetNDF();           // obtain ndf

    return (ffit);  // return fit function
};

#endif
