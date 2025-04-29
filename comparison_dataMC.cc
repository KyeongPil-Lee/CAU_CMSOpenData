
void comparison_dataMC() {
  TFile* f_data = TFile::Open("hist_dimuon_nanoAOD.root");
  TFile* f_MC   = TFile::Open("hist_dimuon_nanoAOD_MC.root");

  TH1D* h_sMu_pt_data = (TH1D*)f_data->Get("h_sMu_pt");
  TH1D* h_sMu_pt_MC   = (TH1D*)f_MC->Get("h_sMu_pt");

  // -- pt comparison
  TCanvas *c_sMu_pt = new TCanvas("c_sMu_pt", "c_sMu_pt", 800, 600);
  c_sMu_pt->cd();

  h_sMu_pt_data->SetLineColor(kBlack);
  h_sMu_pt_MC->SetLineColor(kBlue);

  h_sMu_pt_MC->Draw();
  h_sMu_pt_data->Draw("SAME");

  c_sMu_pt->SaveAs(".pdf");

  // -- dimuon mass comparison
  TCanvas *c_dimu_mass = new TCanvas("c_dimu_mass", "c_dimu_mass", 800, 600);
  c_dimu_mass->cd();

  TH1D* h_dimu_mass_data = (TH1D*)f_data->Get("h_dimu_mass");
  TH1D* h_dimu_mass_MC   = (TH1D*)f_MC->Get("h_dimu_mass");

  h_dimu_mass_data->SetLineColor(kBlack);
  h_dimu_mass_MC->SetLineColor(kBlue);

  h_dimu_mass_MC->Draw();
  h_dimu_mass_data->Draw("SAME");

  c_dimu_mass->SaveAs(".pdf");

  // -- dimuon mass, shape only
  TCanvas *c_dimu_mass_shape = new TCanvas("c_dimu_mass_shape", "c_dimu_mass_shape", 800, 600);
  c_dimu_mass_shape->cd();

  TH1D* h_dimu_mass_data_shape = (TH1D*)h_dimu_mass_data->Clone("h_dimu_mass_data_shape");
  h_dimu_mass_data_shape->Scale(1./h_dimu_mass_data->Integral());

  TH1D* h_dimu_mass_MC_shape = (TH1D*)h_dimu_mass_MC->Clone("h_dimu_mass_MC_shape");
  h_dimu_mass_MC_shape->Scale(1./h_dimu_mass_MC->Integral());

  h_dimu_mass_data_shape->Draw("HIST");
  h_dimu_mass_MC_shape->Draw("HIST SAME");

  c_dimu_mass_shape->SaveAs(".pdf");
}