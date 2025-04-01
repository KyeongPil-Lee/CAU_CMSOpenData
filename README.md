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