Bool_t IsZtoMMorTT(TTreeReaderArray<Int_t>* LHEPart_pdgId, TString mcType) {
  Bool_t isFound = kFALSE;
  Int_t pdgID_lepton = 0;
  if( mcType == "DY" )        pdgID_lepton = 13;
  else if( mcType == "DYTT" ) pdgID_lepton = 15; // -- tau: 15
  else
    throw std::invalid_argument("Invalid mcType");

  Int_t nLHELepton = 0;
  for( Int_t i_lep = 0; i_lep < LHEPart_pdgId->GetSize(); ++i_lep ) {
    if( std::abs(LHEPart_pdgId->At(i_lep)) == pdgID_lepton ) {
      nLHELepton++;
    }
  }
  if( nLHELepton == 2 ) isFound = kTRUE;

  return isFound;
}

Double_t Get_SF(TH2D* h2D, Double_t pt, Double_t eta) {
  if( pt >= 120.0) pt = 119.9; // -- avoid overflow
  Int_t bin_eta = h2D->GetXaxis()->FindBin(TMath::Abs(eta));
  Int_t bin_pt = h2D->GetYaxis()->FindBin(pt); // -- absolute eta
  Double_t SF = h2D->GetBinContent(bin_eta, bin_pt);
  // if( SF == 0 ) {
  //   cout << "SF == 0 ... pt = " << pt << ", eta = " << eta << ", bin_pt = " << bin_pt << ", bin_eta = " << bin_eta << endl;
  // }
  return SF;
}

Double_t Get_TrigSF(TH2D* h2D_data, TH2D* h2D_mc, Double_t pt1, Double_t eta1, Double_t pt2, Double_t eta2) {
  if( pt1 >= 120.0) pt1 = 119.9; // -- avoid overflow
  if( pt2 >= 120.0) pt2 = 119.9; // -- avoid overflow

  Int_t bin_eta1 = h2D_data->GetXaxis()->FindBin(TMath::Abs(eta1));
  Int_t bin_pt1 = h2D_data->GetYaxis()->FindBin(pt1); // -- absolute eta
  Int_t bin_eta2 = h2D_data->GetXaxis()->FindBin(TMath::Abs(eta2));
  Int_t bin_pt2 = h2D_data->GetYaxis()->FindBin(pt2); // -- absolute eta

  Double_t eff_mu1_data = h2D_data->GetBinContent(bin_eta1, bin_pt1);
  Double_t eff_mu2_data = h2D_data->GetBinContent(bin_eta2, bin_pt2);
  Double_t eff_mu1_mc = h2D_mc->GetBinContent(bin_eta1, bin_pt1);
  Double_t eff_mu2_mc = h2D_mc->GetBinContent(bin_eta2, bin_pt2);

  Double_t eff_data = 1.0 - (1.0 - eff_mu1_data)*(1.0 - eff_mu2_data);
  Double_t eff_mc = 1.0 - (1.0 - eff_mu1_mc)*(1.0 - eff_mu2_mc);

  return eff_data / eff_mc;
}

