#define LUMI 8740.1 // -- in /pb

#include "commonTool.h"

void makeHist_dimuon_nanoAOD() {
  TH1::SetDefaultSumw2();
  TH1::AddDirectory(kFALSE);

  Bool_t isData = kFALSE;
  // -- available mcType: DY (DY->mm), DYTT (DY->tautau), TT
  std::string mcType = "TT";
  Bool_t doNorm = kTRUE;
  Bool_t useEffSF = kTRUE;

  std::cout << "isData: " << isData << std::endl;
  std::cout << "mcType: " << mcType << std::endl;
  std::cout << "doNorm: " << doNorm << std::endl;
  std::cout << "useEffSF: " << useEffSF << std::endl;

  // -- load efficiency and scale factors
  TFile *f_SF_ID   = TFile::Open("EffSF/EffSF_Muon_ID.root");
  TFile *f_SF_ISO  = TFile::Open("EffSF/EffSF_Muon_ISO.root");
  TFile *f_SF_TRIG = TFile::Open("EffSF/EffSF_Muon_TRIG.root");

  TH2D* h2D_SF_ID         = (TH2D*)f_SF_ID->Get("NUM_TightID_DEN_TrackerMuons_abseta_pt")->Clone();
  TH2D* h2D_SF_ISO        = (TH2D*)f_SF_ISO->Get("NUM_TightRelIso_DEN_TightIDandIPCut_abseta_pt")->Clone();
  TH2D* h2D_eff_data_TRIG = (TH2D*)f_SF_TRIG->Get("NUM_IsoMu24_or_IsoTkMu24_DEN_CutBasedIdTight_and_PFIsoTight_abseta_pt_efficiencyData")->Clone();
  TH2D* h2D_eff_mc_TRIG   = (TH2D*)f_SF_TRIG->Get("NUM_IsoMu24_or_IsoTkMu24_DEN_CutBasedIdTight_and_PFIsoTight_abseta_pt_efficiencyMC")->Clone();

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
  if( isData ) {
    // chain->Add("Example_SingleMuon_Run2016H_NanoAOD.root");
    chain->Add("/data2/kplee/Lecture/CMSOpenData/Data/SingleMuon/Run2016H/*.root");
  }
  else {
    if( mcType == "DY" )        chain->Add("/data2/kplee/Lecture/CMSOpenData/MC2016/DY_M50_aMCNLO/*.root");
    else if( mcType == "DYTT" ) chain->Add("/data2/kplee/Lecture/CMSOpenData/MC2016/DY_M50_aMCNLO/*.root");
    else if( mcType == "TT" )   chain->Add("/data2/kplee/Lecture/CMSOpenData/MC2016/TTTo2L2Nu_Powheg/*.root");
    else                      throw std::invalid_argument("Invalid MC type: " + mcType);
  }

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

  // -- MC only: should be initialized to nullptr first and assigned to a value only if it is MC
  TTreeReaderValue<Float_t>* genWeight = nullptr;
  TTreeReaderArray<Int_t>* LHEPart_pdgId = nullptr;
  if( !isData ) {
    genWeight = new TTreeReaderValue<Float_t>(reader, "genWeight");
    LHEPart_pdgId = new TTreeReaderArray<Int_t>(reader, "LHEPart_pdgId");
  }

  // -- print the number of entries in the chain
  std::cout << "Number of entries in the chain: " << chain->GetEntries() << std::endl;

  Int_t nEvent_tot = reader.GetEntries(kTRUE);
  // nEvent_tot = 1e7;

  Int_t nEvent_cutoff = 1e7;
  if( nEvent_tot > nEvent_cutoff ) {
    nEvent_tot = nEvent_cutoff;
    std::cout << "nEvent_tot is too large, set to " << nEvent_cutoff << std::endl;
  }

  Int_t sumWeight = 0;
  // -- event loop
  for (Int_t i_ev = 0; i_ev < nEvent_tot; ++i_ev) {
    reader.SetEntry(i_ev);

    // -- progress bar
    if (i_ev % 100000 == 0) {
      std::cout << "Processing event " << i_ev << " / " << nEvent_tot << " (" << (i_ev / (double)nEvent_tot) * 100 << "%)" << std::endl;
    }

    // -- check if the event is Z->mm or TT
    if( mcType == "DY" || mcType == "DYTT" ) {
      if( !IsZtoMMorTT(LHEPart_pdgId, mcType) ) continue;
    }

    Double_t event_weight = 1.0;
    if( !isData ) {
      event_weight = **genWeight < 0 ? -1.0 : 1.0;
      sumWeight += event_weight;
    }

    // -- trigger selection
    if( !(*HLT_IsoMu24 || *HLT_IsoTkMu24) ) continue;

    vector<TLorentzVector> vec_muP4;
    vector<Int_t> vec_muCharge;
    for (Int_t i_lep = 0; i_lep < Muon_pt.GetSize(); ++i_lep) {
      Float_t pt = Muon_pt[i_lep];
      Float_t eta = Muon_eta[i_lep];
      Float_t phi = Muon_phi[i_lep];

      h_pt->Fill(pt, event_weight); 
      h_eta->Fill(eta, event_weight);
      h_phi->Fill(phi, event_weight);

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

      Double_t SF_all = 1.0;
      if( useEffSF ) { 
        Double_t SF_ID_mu1 = Get_SF(h2D_SF_ID, mu1.Pt(), mu1.Eta());
        Double_t SF_ID_mu2 = Get_SF(h2D_SF_ID, mu2.Pt(), mu2.Eta());

        Double_t SF_ISO_mu1 = Get_SF(h2D_SF_ISO, mu1.Pt(), mu1.Eta());
        Double_t SF_ISO_mu2 = Get_SF(h2D_SF_ISO, mu2.Pt(), mu2.Eta());

        Double_t SF_ID = SF_ID_mu1 * SF_ID_mu2;
        Double_t SF_ISO = SF_ISO_mu1 * SF_ISO_mu2;

        Double_t SF_TRIG = Get_TrigSF(h2D_eff_data_TRIG, h2D_eff_mc_TRIG, mu2.Pt(), mu2.Eta(), mu1.Pt(), mu1.Eta());

        SF_all = SF_ID * SF_ISO * SF_TRIG;

        // cout << "SF_ID = " << SF_ID << ", SF_ISO = " << SF_ISO << ", SF_TRIG = " << SF_TRIG << ", SF_all = " << SF_all << endl;
      }
      else {
        SF_all = 1.0;
      }

      h_sMu_pt->Fill(mu1.Pt(), event_weight * SF_all);
      h_sMu_eta->Fill(mu1.Eta(), event_weight * SF_all);
      h_sMu_phi->Fill(mu1.Phi(), event_weight * SF_all);
      h_sMu_pt->Fill(mu2.Pt(), event_weight * SF_all);
      h_sMu_eta->Fill(mu2.Eta(), event_weight * SF_all);
      h_sMu_phi->Fill(mu2.Phi(), event_weight * SF_all);

      h_dimu_mass->Fill(dimu.M(), event_weight * SF_all);
      h_dimu_pt->Fill(dimu.Pt(), event_weight * SF_all);
    }

  } // -- end of event loop

  std::cout << "sumWeight: " << sumWeight << std::endl;
  if( !isData && doNorm ) {
    Double_t xSec = -1.0;
    if( mcType == "DY" )        xSec = 6019.95 / 3.0;
    else if( mcType == "DYTT" ) xSec = 6019.95 / 3.0;
    else if( mcType == "TT" )   xSec = 88.51;
    else throw std::invalid_argument("Invalid MC type: " + mcType);

    Double_t normFactor = (xSec * LUMI) / sumWeight;
    std::cout << "normFactor: " << normFactor << std::endl;

    h_pt->Scale(normFactor);
    h_eta->Scale(normFactor);
    h_phi->Scale(normFactor);

    h_sMu_pt->Scale(normFactor);
    h_sMu_eta->Scale(normFactor);
    h_sMu_phi->Scale(normFactor);

    h_dimu_mass->Scale(normFactor);
    h_dimu_pt->Scale(normFactor);  
  }

  // -- save the histograms
  std::string fileName = "hist_dimuon_nanoAOD.root";
  if( !isData ) fileName = "hist_dimuon_nanoAOD_MC_" + mcType + ".root";

  TFile* f_out = new TFile(fileName.c_str(), "RECREATE");
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