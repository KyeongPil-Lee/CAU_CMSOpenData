void acc_eff() {
  TChain *chain = new TChain("Events"); // -- "Events": name of the tree in the nanoAOD file
  // -- Z MC
  chain->Add("/data2/kplee/Lecture/CMSOpenData/MC2016/DY_M50_aMCNLO/*.root");

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

  // -- MC-only informations: truth (generator level) information
  TTreeReaderValue<Float_t> genWeight(reader, "genWeight");
  TTreeReaderArray<Float_t> GenPart_pt(reader, "GenPart_pt");
  TTreeReaderArray<Float_t> GenPart_eta(reader, "GenPart_eta");
  TTreeReaderArray<Float_t> GenPart_phi(reader, "GenPart_phi");
  TTreeReaderArray<Int_t> GenPart_pdgId(reader, "GenPart_pdgId");
  TTreeReaderArray<Int_t> GenPart_status(reader, "GenPart_status");
  TTreeReaderArray<Int_t> GenPart_statusFlags(reader, "GenPart_statusFlags");

  // -- LHE information: to separate Z->mm events from Z->ll events
  TTreeReaderArray<Int_t> LHEPart_pdgId(reader, "LHEPart_pdgId");

  // -- print the number of entries in the chain
  std::cout << "Number of entries in the chain: " << chain->GetEntries() << std::endl;

  Int_t nEvent_tot = reader.GetEntries(kTRUE);
  nEvent_tot = 1000000;

  Double_t sumWeight_tot = 0; // -- sum of weights (denominator of the acceptance)
  Double_t sumWeight_acc = 0; // -- sum of weights passing acceptance condition (numerator of the acceptance)
  Double_t sumWeight_eff = 0; // -- sum of weights passing efficiency condition (numerator of the efficiency)
  // -- event loop
  for (Int_t i_ev = 0; i_ev < nEvent_tot; ++i_ev) {
    reader.SetEntry(i_ev);

        // -- progress bar
    if (i_ev % 100000 == 0) {
      std::cout << "Processing event " << i_ev << " / " << nEvent_tot << " (" << (i_ev / (double)nEvent_tot) * 100 << "%)" << std::endl;
    }

    // -- select Z->mm events only
    Int_t nLHEMuon = 0;
    for( Int_t i_lep = 0; i_lep < LHEPart_pdgId.GetSize(); ++i_lep ) {
      if( std::abs(LHEPart_pdgId[i_lep]) == 13 ) {
        nLHEMuon++;
      }
    }
    if( nLHEMuon != 2 ) continue;

    // -- collect gen-muons
    std::vector<TLorentzVector> vec_genMuP4;
    for( Int_t i_lep = 0; i_lep < GenPart_pt.GetSize(); ++i_lep ) {
      if( std::abs(GenPart_pdgId[i_lep]) != 13 ) continue;
      if( std::abs(GenPart_status[i_lep]) != 1 ) continue; // -- is stable
      if( !(std::abs(GenPart_statusFlags[i_lep]) & 256) ) continue; // -- fromHardProcess (from Z)

      TLorentzVector genMuP4;
      genMuP4.SetPtEtaPhiM(GenPart_pt[i_lep], GenPart_eta[i_lep], GenPart_phi[i_lep], 0.105);
      vec_genMuP4.push_back(genMuP4);
    }
    if( vec_genMuP4.size() != 2 ) {
      cout << "vec_genMuP4.size() != 2 ... # mu = " << vec_genMuP4.size() << endl;
      break;
    }
    Double_t m_ll_gen = (vec_genMuP4[0] + vec_genMuP4[1]).M();
    if( m_ll_gen < 60.0 || m_ll_gen > 120.0 ) continue;

    // -- count event weight only for Z->mm events (in 60 < m < 120 GeV)
    Double_t event_weight = *genWeight < 0 ? -1.0 : 1.0;
    sumWeight_tot += event_weight; // -- denominator of the acceptance

    Bool_t pass_acc = kFALSE;
    if( vec_genMuP4[0].Pt() > 26.0 && std::abs(vec_genMuP4[0].Eta()) < 2.4 &&
        vec_genMuP4[1].Pt() > 26.0 && std::abs(vec_genMuP4[1].Eta()) < 2.4 ) {
      pass_acc = kTRUE;
    }

    if( pass_acc ) {
      sumWeight_acc += event_weight; // -- numerator of the acceptance
      
      // -- start to count # reco events passing event selection
      // -- only counts for the events passing acceptance condition

      // -- below: copy & paste from makeHist_dimuon_nanoAOD.cc: it should be exactly same selection criteria -- //

      // -- trigger selection
      if( !(*HLT_IsoMu24 || *HLT_IsoTkMu24) ) continue;

      vector<TLorentzVector> vec_muP4;
      vector<Int_t> vec_muCharge;
      for (Int_t i_lep = 0; i_lep < Muon_pt.GetSize(); ++i_lep) {
        Float_t pt = Muon_pt[i_lep];
        Float_t eta = Muon_eta[i_lep];
        Float_t phi = Muon_phi[i_lep];

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

        sumWeight_eff += event_weight; // -- numerator of the efficiency
      }
    } // -- end of pass_acc
  } // -- end of event loop

  // -- print the results
  std::cout << "Acceptance = " << sumWeight_acc << " / " << sumWeight_tot << " = " << sumWeight_acc / sumWeight_tot << std::endl;
  std::cout << "Efficiency = " << sumWeight_eff << " / " << sumWeight_acc << " = " << sumWeight_eff / sumWeight_acc << std::endl;
}