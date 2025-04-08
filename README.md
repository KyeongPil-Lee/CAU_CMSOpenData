# CMS open data tutorial

## Setup
```bash
# -- @ CAU cluster
git clone https://github.com/KyeongPil-Lee/CAU_CMSOpenData.git # -- https version
# git clone git@github.com:KyeongPil-Lee/CAU_CMSOpenData.git # -- ssh, if you have registered ssh key in your github account
```

## Read the data (nanoAOD)
```bash
root -l -b -q read_nanoAOD.cc
```

## Make simple histograms
```bash
root -l -b -q makeHist_nanoAOD.cc
```

## Make dimuon histograms
```bash
root -l -b -q makeHist_dimuon_nanoAOD.cc
```

## CMS Open data
### Data
* SingleMuon, Run2016H, nanoAOD
  * https://opendata.cern.ch/record/30563
  * With the validated events only: https://opendata.cern.ch/record/14220