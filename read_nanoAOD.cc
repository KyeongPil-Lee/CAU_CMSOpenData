
void read_nanoAOD() {
  TChain *chain = new TChain("Events"); // -- "Events": name of the tree in the nanoAOD file

  // -- add the nanoAOD files to the chain
  chain->Add("Example_SingleMuon_Run2016H_NanoAOD.root");
  // -- you can add multiple files to the chain
  // chain->Add("nanoAOD_file2.root");
  // chain->Add("nanoAOD_file3.root");

  // -- TTreeReader: a useful class that reads the tree
  TTreeReader reader(chain); // -- put the chain into the reader

  // -- define the branches you want to read
  // -- muon variables: defined as "array" because there are multiple muons in the event
  TTreeReaderArray<Float_t> Muon_pt(reader, "Muon_pt");
  TTreeReaderArray<Float_t> Muon_eta(reader, "Muon_eta");
  TTreeReaderArray<Float_t> Muon_phi(reader, "Muon_phi");

  // -- print the number of entries in the chain
  std::cout << "Number of entries in the chain: " << chain->GetEntries() << std::endl;

  Int_t nEvent_tot = reader.GetEntries(kTRUE);

  for (Int_t i_ev = 0; i_ev < nEvent_tot; ++i_ev) {
    reader.SetEntry(i_ev);

    for (Int_t i_lep = 0; i_lep < Muon_pt.GetSize(); ++i_lep) {
      Float_t pt = Muon_pt[i_lep];
      Float_t eta = Muon_eta[i_lep];
      Float_t phi = Muon_phi[i_lep];

      std::cout << "Muon " << i_lep << " pt: " << pt << " eta: " << eta << " phi: " << phi << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
  } // -- end of event loop
}