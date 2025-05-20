Double_t Count_nEvent_ZPeak(TH1D* h_dimu_mass) {
  Double_t nEvent_ZPeak = 0;
  for(Int_t i=0; i<h_dimu_mass->GetNbinsX(); i++) {
    Int_t i_bin = i+1;
    Double_t binCenter = h_dimu_mass->GetBinCenter(i_bin);
    if( binCenter < 60.0 || binCenter > 120.0 ) continue;
    nEvent_ZPeak += h_dimu_mass->GetBinContent(i_bin);
  }
  return nEvent_ZPeak;
}

void comparison_dataMCStack() {
  TFile* f_data = TFile::Open("hist_dimuon_nanoAOD.root");
  TFile* f_MC_DY = TFile::Open("hist_dimuon_nanoAOD_MC_DY.root");
  TFile* f_MC_TT = TFile::Open("hist_dimuon_nanoAOD_MC_TT.root");

  // -- dimuon mass comparison
  TCanvas *c_dimu_mass = new TCanvas("c_dimu_mass_stack", "c_dimu_mass_stack", 800, 600);
  c_dimu_mass->cd();

  TH1D* h_dimu_mass_data = (TH1D*)f_data->Get("h_dimu_mass");
  TH1D* h_dimu_mass_MC_DY = (TH1D*)f_MC_DY->Get("h_dimu_mass");
  TH1D* h_dimu_mass_MC_TT = (TH1D*)f_MC_TT->Get("h_dimu_mass");

  h_dimu_mass_data->SetLineColor(kBlack);
  h_dimu_mass_data->SetMarkerColor(kBlack);
  h_dimu_mass_data->SetMarkerStyle(20);
  h_dimu_mass_data->SetMarkerSize(0.7);

  h_dimu_mass_MC_TT->SetLineColor(kRed-2);
  h_dimu_mass_MC_TT->SetFillColor(kRed-2);
  h_dimu_mass_MC_DY->SetLineColor(kGreen-2);
  h_dimu_mass_MC_DY->SetFillColor(kGreen-2);

  THStack* h_dimu_mass_MCStack = new THStack("h_dimu_mass_MCStack", "h_dimu_mass_MCStack");
  h_dimu_mass_MCStack->Add(h_dimu_mass_MC_TT);
  h_dimu_mass_MCStack->Add(h_dimu_mass_MC_DY);

  h_dimu_mass_data->Draw();
  h_dimu_mass_MCStack->Draw("HIST SAME");
  h_dimu_mass_data->Draw("EPSAME");

  c_dimu_mass->SaveAs(".pdf");

  c_dimu_mass->SetLogy();
  h_dimu_mass_data->GetYaxis()->SetRangeUser(5e1, 1e6);
  c_dimu_mass->SaveAs("c_dimu_mass_stack_logy.pdf");

  // -- ratio
  TCanvas *c_dimu_mass_ratio = new TCanvas("c_dimu_mass_ratio_stack", "c_dimu_mass_ratio_stack", 800, 600);
  c_dimu_mass_ratio->cd();

  TH1D* h_dimu_mass_totMC = (TH1D*)h_dimu_mass_MC_DY->Clone("h_dimu_mass_totMC");
  h_dimu_mass_totMC->Add(h_dimu_mass_MC_TT);

  TH1D* h_dimu_mass_ratio = (TH1D*)h_dimu_mass_data->Clone("h_dimu_mass_ratio");
  h_dimu_mass_ratio->Divide(h_dimu_mass_totMC);
  h_dimu_mass_ratio->Draw();

  h_dimu_mass_ratio->GetYaxis()->SetRangeUser(0.7, 1.3);

  c_dimu_mass_ratio->SaveAs("c_dimu_mass_ratio_stack.pdf");

  // -- calculate Z cross section
  Double_t nEvent_data = Count_nEvent_ZPeak(h_dimu_mass_data);
  Double_t nEvent_MC_TT = Count_nEvent_ZPeak(h_dimu_mass_MC_TT);

  Double_t nEvent_obs = nEvent_data - nEvent_MC_TT;
  // -- values: from acc_eff.cc
  Double_t acc = 0.361926;
  Double_t eff = 0.857341;
  Double_t lumi = 8740.1;

  std::cout << "cross section = " << nEvent_obs << " / ( " << acc << " * " << eff << " * " << lumi << " ) = " << nEvent_obs / (acc * eff * lumi) << " pb" << std::endl;  
}