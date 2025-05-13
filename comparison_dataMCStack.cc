
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
}