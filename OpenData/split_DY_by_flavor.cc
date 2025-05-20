#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TTreeReaderValue.h"
#include "TString.h"
#include "TSystem.h"
#include <iostream>
#include <vector>

void split_DY_by_flavor() {
    // 입력 파일 목록 가져오기
    TChain *chain = new TChain("Events");
    chain->Add("/data2/kplee/Lecture/CMSOpenData/MC2016/DY_M50_aMCNLO/*.root");
    
    // 입력 파일 목록 가져오기
    TObjArray *fileList = chain->GetListOfFiles();
    Int_t nFiles = fileList->GetEntries();
    
    // 출력 디렉토리 생성
    TString baseDir = "/data2/kplee/Lecture/CMSOpenData/MC2016/DY_M50_aMCNLO";
    gSystem->mkdir(baseDir + "/ee", kTRUE);
    gSystem->mkdir(baseDir + "/mm", kTRUE);
    gSystem->mkdir(baseDir + "/tt", kTRUE);
    
    // 각 파일에 대해 처리
    for(Int_t iFile = 0; iFile < nFiles; iFile++) {
        TString inputFileName = fileList->At(iFile)->GetTitle();
        TString baseFileName = gSystem->BaseName(inputFileName);
        baseFileName.ReplaceAll(".root", "");
        
        std::cout << "Processing file " << iFile + 1 << "/" << nFiles 
                  << ": " << baseFileName << std::endl;
        
        // 현재 파일에 대한 TChain 생성
        TChain *currentChain = new TChain("Events");
        currentChain->Add(inputFileName);
        
        TTreeReader reader(currentChain);
        TTreeReaderArray<Int_t> LHEPart_pdgId(reader, "LHEPart_pdgId");
        
        // 출력 파일 생성
        TFile *fout_ee = new TFile(baseDir + "/ee/" + baseFileName + "_ee.root", "RECREATE");
        TFile *fout_mm = new TFile(baseDir + "/mm/" + baseFileName + "_mm.root", "RECREATE");
        TFile *fout_tt = new TFile(baseDir + "/tt/" + baseFileName + "_tt.root", "RECREATE");
        
        // 새로운 트리 생성
        TTree *tree_ee = currentChain->CloneTree(0);
        TTree *tree_mm = currentChain->CloneTree(0);
        TTree *tree_tt = currentChain->CloneTree(0);
        
        // 이벤트 카운터
        Int_t nEvents_ee = 0;
        Int_t nEvents_mm = 0;
        Int_t nEvents_tt = 0;
        
        // 이벤트 루프
        Long64_t nEntries = reader.GetEntries(kTRUE);
        for(Long64_t i = 0; i < nEntries; i++) {
            reader.SetEntry(i);
            
            // 진행상황 출력
            if(i % 100000 == 0) {
                std::cout << "  Processing event " << i << " / " << nEntries 
                          << " (" << (i * 100.0 / nEntries) << "%)" << std::endl;
            }
            
            // LHE 레벨에서 lepton flavor 확인
            Int_t nElectrons = 0;
            Int_t nMuons = 0;
            Int_t nTaus = 0;
            
            for(Int_t j = 0; j < LHEPart_pdgId.GetSize(); j++) {
                Int_t pdgId = std::abs(LHEPart_pdgId[j]);
                if(pdgId == 11) nElectrons++;
                else if(pdgId == 13) nMuons++;
                else if(pdgId == 15) nTaus++;
            }
            
            // 각 flavor에 따라 이벤트 분류
            if(nElectrons == 2 && nMuons == 0 && nTaus == 0) {
                tree_ee->Fill();
                nEvents_ee++;
            }
            else if(nElectrons == 0 && nMuons == 2 && nTaus == 0) {
                tree_mm->Fill();
                nEvents_mm++;
            }
            else if(nElectrons == 0 && nMuons == 0 && nTaus == 2) {
                tree_tt->Fill();
                nEvents_tt++;
            }
        }
        
        // 결과 저장
        fout_ee->cd();
        tree_ee->Write();
        fout_ee->Close();
        
        fout_mm->cd();
        tree_mm->Write();
        fout_mm->Close();
        
        fout_tt->cd();
        tree_tt->Write();
        fout_tt->Close();
        
        // 현재 파일의 결과 출력
        std::cout << "  File " << baseFileName << " 분리 결과:" << std::endl;
        std::cout << "    ee 채널: " << nEvents_ee << " 이벤트" << std::endl;
        std::cout << "    mm 채널: " << nEvents_mm << " 이벤트" << std::endl;
        std::cout << "    tt 채널: " << nEvents_tt << " 이벤트" << std::endl;
        std::cout << "    Total: " << nEvents_ee + nEvents_mm + nEvents_tt << " 이벤트" << std::endl;
        
        delete currentChain;
    }
    
    delete chain;
} 