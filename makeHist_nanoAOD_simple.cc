void makeHist_nanoAOD_simple() {
  // -- define histograms to be filled
  // -- single muons (leading, sub-leading)
  TH1D* h_muLead_pt = new TH1D("h_muLead_pt", "Leading muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_muLead_eta = new TH1D("h_muLead_eta", "Leading muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_muLead_phi = new TH1D("h_muLead_phi", "Leading muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  TH1D* h_muSub_pt = new TH1D("h_muSub_pt", "Sub-leading muon p_{T} (GeV)", 300, 0, 300);
  TH1D* h_muSub_eta = new TH1D("h_muSub_eta", "Sub-leading muon #eta", 60, -3, 3); // -- bin size: 0.1
  TH1D* h_muSub_phi = new TH1D("h_muSub_phi", "Sub-leading muon #phi", 24, -TMath::Pi(), TMath::Pi()); // -- bin size: pi/12 (15 degree)

  // -- dimuons
  TH1D* h_dimu_mass = new TH1D("h_dimu_mass", "Dimuon mass", 60, 60, 120);
  TH1D* h_dimu_pt   = new TH1D("h_dimu_pt", "Dimuon p_{T}", 300, 0, 300);

  TChain *chain = new TChain("Events"); // -- "Events": name of the tree in the nanoAOD file

  // -- add the nanoAOD files to the chain
  chain->Add("Example_SingleMuon_Run2016H_NanoAOD.root");
  // -- you can add multiple files to the chain, saved in the cluster (you can use *)
  // chain->Add("/data2/kplee/Lecture/CMSOpenData/Data/SingleMuon/Run2016H/*.root");

  // -- TTreeReader: a useful class that reads the tree
  TTreeReader reader(chain); // -- put the chain into the reader

  // -- define the branches you want to read
  // -- muon variables: defined as "array" because there are multiple muons in the event
  TTreeReaderValue<Bool_t> HLT_IsoMu24(reader, "HLT_IsoMu24");
  TTreeReaderValue<Bool_t> HLT_IsoTkMu24(reader, "HLT_IsoTkMu24");

  TTreeReaderArray<Float_t> Muon_pt(reader, "Muon_pt");
  TTreeReaderArray<Float_t> Muon_eta(reader, "Muon_eta");
  TTreeReaderArray<Float_t> Muon_phi(reader, "Muon_phi");
  TTreeReaderArray<Int_t> Muon_charge(reader, "Muon_charge");

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

    // -- trigger condition
    if( !(*HLT_IsoMu24 || *HLT_IsoTkMu24) ) continue;

    vector<TLorentzVector> vec_muP4;
    vector<Int_t> vec_muCharge;
    for(Int_t i_mu = 0; i_mu < Muon_pt.GetSize(); ++i_mu) {
      Float_t pt = Muon_pt[i_mu];
      Float_t eta = Muon_eta[i_mu];
      Float_t phi = Muon_phi[i_mu];

      // -- select good muons
      if( pt > 26 && std::abs(eta) < 2.4 &&
        Muon_tightId[i_mu] == 1 && Muon_pfRelIso04_all[i_mu] < 0.15 ) {

        TLorentzVector p4_mu;
        p4_mu.SetPtEtaPhiM(pt, eta, phi, 0.105); // -- m_mu: ~ 0.105 GeV
        vec_muP4.push_back(p4_mu);
        vec_muCharge.push_back(Muon_charge[i_mu]);
      }
    } // -- end of muon loop

    if( vec_muP4.size() == 2 &&
        vec_muCharge[0] * vec_muCharge[1] < 0 ) {

      TLorentzVector p4_mu1 = vec_muP4[0];
      TLorentzVector p4_mu2 = vec_muP4[1];
      TLorentzVector p4_dimu = p4_mu1 + p4_mu2;

      if( p4_dimu.M() < 60 || p4_dimu.M() > 120 ) continue;

      // -- fill histograms
      // -- check which one is leading
      TLorentzVector p4_muLead, p4_muSub;
      if( p4_mu1.Pt() > p4_mu2.Pt() ) {
        p4_muLead = p4_mu1;
        p4_muSub = p4_mu2;
      }
      else {
        p4_muLead = p4_mu2;
        p4_muSub = p4_mu1;
      }

      h_muLead_pt->Fill(p4_muLead.Pt());
      h_muLead_eta->Fill(p4_muLead.Eta());
      h_muLead_phi->Fill(p4_muLead.Phi());
      h_muSub_pt->Fill(p4_muSub.Pt());
      h_muSub_eta->Fill(p4_muSub.Eta());
      h_muSub_phi->Fill(p4_muSub.Phi());

      // -- dimuon histograms
      h_dimu_mass->Fill(p4_dimu.M());
      h_dimu_pt->Fill(p4_dimu.Pt());
    }
      

  } // -- end of event loop

  // -- save the histograms
  TFile* f_out = new TFile("hist_dimuon_nanoAOD_simple.root", "RECREATE");
  f_out->cd();
  h_muLead_pt->Write();
  h_muLead_eta->Write();
  h_muLead_phi->Write();
  h_muSub_pt->Write();
  h_muSub_eta->Write();
  h_muSub_phi->Write();
  h_dimu_mass->Write();
  h_dimu_pt->Write();
  f_out->Close();
}