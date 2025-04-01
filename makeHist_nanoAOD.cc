
void makeHist_nanoAOD() {
  // -- define histograms to be filled
  // -- all muons
  TH1D* h_pt = new TH1D("h_pt", "Muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_eta = new TH1D("h_eta", "Muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_phi = new TH1D("h_phi", "Muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  // -- good muons (passing tight ID and isolation)
  TH1D* h_pt_good = new TH1D("h_pt_good", "Muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_eta_good = new TH1D("h_eta_good", "Muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_phi_good = new TH1D("h_phi_good", "Muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  TChain *chain = new TChain("Events"); // -- "Events": name of the tree in the nanoAOD file

  // -- add the nanoAOD files to the chain
  // chain->Add("Example_SingleMuon_Run2016H_NanoAOD.root");
  // -- you can add multiple files to the chain, saved in the cluster (you can use *)
  // chain->Add("/data2/kplee/Lecture/CMSOpenData/Data/SingleMuon/Run2016H/*.root");

  // -- TTreeReader: a useful class that reads the tree
  TTreeReader reader(chain); // -- put the chain into the reader

  // -- define the branches you want to read
  // -- muon variables: defined as "array" because there are multiple muons in the event
  TTreeReaderArray<Float_t> Muon_pt(reader, "Muon_pt");
  TTreeReaderArray<Float_t> Muon_eta(reader, "Muon_eta");
  TTreeReaderArray<Float_t> Muon_phi(reader, "Muon_phi");

  TTreeReaderArray<Bool_t> Muon_tightId(reader, "Muon_tightId");
  TTreeReaderArray<Float_t> Muon_pfRelIso04_all(reader, "Muon_pfRelIso04_all");

  // -- print the number of entries in the chain
  std::cout << "Number of entries in the chain: " << chain->GetEntries() << std::endl;

  Int_t nEvent_tot = reader.GetEntries(kTRUE);

  for (Int_t i_ev = 0; i_ev < nEvent_tot; ++i_ev) {
    reader.SetEntry(i_ev);

    // -- progress bar
    if (i_ev % 1000 == 0) {
      std::cout << "Processing event " << i_ev << " / " << nEvent_tot << " (" << (i_ev / (double)nEvent_tot) * 100 << "%)" << std::endl;
    }

    for (Int_t i_lep = 0; i_lep < Muon_pt.GetSize(); ++i_lep) {
      Float_t pt = Muon_pt[i_lep];
      Float_t eta = Muon_eta[i_lep];
      Float_t phi = Muon_phi[i_lep];

      h_pt->Fill(pt); 
      h_eta->Fill(eta);
      h_phi->Fill(phi);

      if (Muon_tightId[i_lep] == 1 && Muon_pfRelIso04_all[i_lep] < 0.15) {
        h_pt_good->Fill(pt);
        h_eta_good->Fill(eta);
        h_phi_good->Fill(phi);
      }
    }
  } // -- end of event loop

  // -- save the histograms
  TFile* f_out = new TFile("hist_nanoAOD.root", "RECREATE");
  f_out->cd();
  h_pt->Write();
  h_eta->Write();
  h_phi->Write();
  h_pt_good->Write();
  h_eta_good->Write();
  h_phi_good->Write();
  f_out->Close();
}