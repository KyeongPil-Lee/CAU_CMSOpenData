
void makeHist_dimuon_nanoAOD() {
  // -- define histograms to be filled
  // -- all muons
  TH1D* h_pt = new TH1D("h_pt", "Muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_eta = new TH1D("h_eta", "Muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_phi = new TH1D("h_phi", "Muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  // -- good muons (passing tight ID and isolation)
  // TH1D* h_pt_good = new TH1D("h_pt_good", "Muon p_{T} (GeV)", 300, 0, 300);
  // TH1D* h_eta_good = new TH1D("h_eta_good", "Muon #eta", 60, -3, 3); // -- bin size: 0.1
  // TH1D* h_phi_good = new TH1D("h_phi_good", "Muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  TH1D* h_sMu_pt = new TH1D("h_sMu_pt", "Muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_sMu_eta = new TH1D("h_sMu_eta", "Muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_sMu_phi = new TH1D("h_sMu_phi", "Muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  TH1D* h_dimu_mass = new TH1D("h_dimu_mass", "Dimuon mass", 160, 50, 130);
  TH1D* h_dimu_pt   = new TH1D("h_dimu_pt", "Dimuon p_{T}", 300, 0, 300);

  TChain *chain = new TChain("Events"); // -- "Events": name of the tree in the nanoAOD file

  // -- add the nanoAOD files to the chain
  // chain->Add("Example_SingleMuon_Run2016H_NanoAOD.root");
  // -- you can add multiple files to the chain, saved in the cluster (you can use *)
  chain->Add("/data2/kplee/Lecture/CMSOpenData/Data/SingleMuon/Run2016H/*.root");

  // -- TTreeReader: a useful class that reads the tree
  TTreeReader reader(chain); // -- put the chain into the reader

  // -- define the branches you want to read
  // -- use triggers
  TTreeReaderValue<Bool_t> HLT_IsoMu24(reader, "HLT_IsoMu24");
  TTreeReaderValue<Bool_t> HLT_IsoTkMu24(reader, "HLT_IsoTkMu24");

  // -- muon variables: defined as "array" because there are multiple muons in the event
  TTreeReaderArray<Float_t> Muon_pt(reader, "Muon_pt");
  TTreeReaderArray<Float_t> Muon_eta(reader, "Muon_eta");
  TTreeReaderArray<Float_t> Muon_phi(reader, "Muon_phi");
  TTreeReaderArray<Int_t>   Muon_charge(reader, "Muon_charge");

  TTreeReaderArray<Bool_t> Muon_tightId(reader, "Muon_tightId");
  TTreeReaderArray<Float_t> Muon_pfRelIso04_all(reader, "Muon_pfRelIso04_all");

  // -- print the number of entries in the chain
  std::cout << "Number of entries in the chain: " << chain->GetEntries() << std::endl;

  Int_t nEvent_tot = reader.GetEntries(kTRUE);

  if( nEvent_tot > 1e7 ) {
    nEvent_tot = 1e7;
    std::cout << "nEvent_tot is too large, set to 1e7" << std::endl;
  }

  for (Int_t i_ev = 0; i_ev < nEvent_tot; ++i_ev) {
    reader.SetEntry(i_ev);

    // -- progress bar
    if (i_ev % 1000 == 0) {
      std::cout << "Processing event " << i_ev << " / " << nEvent_tot << " (" << (i_ev / (double)nEvent_tot) * 100 << "%)" << std::endl;
    }

    // -- trigger selection
    if( !(*HLT_IsoMu24 || *HLT_IsoTkMu24) ) continue;

    vector<TLorentzVector> vec_muP4;
    vector<Int_t> vec_muCharge;
    for (Int_t i_lep = 0; i_lep < Muon_pt.GetSize(); ++i_lep) {
      Float_t pt = Muon_pt[i_lep];
      Float_t eta = Muon_eta[i_lep];
      Float_t phi = Muon_phi[i_lep];

      h_pt->Fill(pt); 
      h_eta->Fill(eta);
      h_phi->Fill(phi);

      if (Muon_tightId[i_lep] == 1 && Muon_pfRelIso04_all[i_lep] < 0.15 && 
          Muon_pt[i_lep] > 26.0 && TMath::Abs(Muon_eta[i_lep]) < 2.4) {
          TLorentzVector muP4;
          muP4.SetPtEtaPhiM(pt, eta, phi, 0.105); // -- m_mu: ~ 0.105 GeV
          vec_muP4.push_back(muP4);
          vec_muCharge.push_back(Muon_charge[i_lep]);
      }
    } // -- end of muon loop

    // printf("vec_muP4.size(): %d\n", vec_muP4.size());

    // -- dimuon selection: consider only the events with 2 muons
    if( vec_muP4.size() == 2 &&
        vec_muCharge[0] * vec_muCharge[1] < 0 ) {
      TLorentzVector mu1 = vec_muP4[0];
      TLorentzVector mu2 = vec_muP4[1];
      TLorentzVector dimu = mu1 + mu2;

      if( dimu.M() < 60 || dimu.M() > 120 ) continue;

      h_sMu_pt->Fill(mu1.Pt());
      h_sMu_eta->Fill(mu1.Eta());
      h_sMu_phi->Fill(mu1.Phi());
      h_sMu_pt->Fill(mu2.Pt());
      h_sMu_eta->Fill(mu2.Eta());
      h_sMu_phi->Fill(mu2.Phi());

      h_dimu_mass->Fill(dimu.M());
      h_dimu_pt->Fill(dimu.Pt());
    }

  } // -- end of event loop

  // -- save the histograms
  TFile* f_out = new TFile("hist_dimuon_nanoAOD.root", "RECREATE");
  f_out->cd();
  h_pt->Write();
  h_eta->Write();
  h_phi->Write();
  // h_pt_good->Write();
  // h_eta_good->Write();
  // h_phi_good->Write();
  h_sMu_pt->Write();
  h_sMu_eta->Write();
  h_sMu_phi->Write();
  h_dimu_mass->Write();
  h_dimu_pt->Write();
  f_out->Close();
}