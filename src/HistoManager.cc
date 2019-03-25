//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file analysis/AnaEx01/src/HistoManager.cc
/// \brief Implementation of the HistoManager class
//
//
// $Id: HistoManager.cc 105494 2017-07-28 09:02:56Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager(const G4bool enable)
  : fFactoryOn(true),
    fEnabled(enable)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
  if(!fEnabled)
    return;
  // Create or get analysis manager
  // The choice of analysis technology is done via selection of a namespace
  // in HistoManager.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
      
  // Create directories 
  analysisManager->SetHistoDirectoryName("histo");
  analysisManager->SetNtupleDirectoryName("ntuple");
    
  // Open an output file
  //
  G4bool fileOpen = analysisManager->OpenFile("CloudChamber");
  if (! fileOpen) {
    G4cerr << "\n---> HistoManager::Book(): cannot open " 
           << analysisManager->GetFileName() << G4endl;
    return;
  }
  
  // Create histograms.
  // Histogram ids are generated automatically starting from 0.
  // The start value can be changed by:
  // analysisManager->SetFirstHistoId(1);  
  
  // id = 0
//  analysisManager->CreateH1("EAbs","Edep in absorber (MeV)", 100, 0., 800*MeV);
  // id = 1
//  analysisManager->CreateH1("EGap","Edep in gap (MeV)", 100, 0., 100*MeV);
  analysisManager->CreateH1("EGas","Edep in chamber (MeV)", 50, 0., 1*MeV);
  // id = 2
//  analysisManager->CreateH1("LAbs","trackL in absorber (mm)", 100, 0., 1*m);
  // id = 3
//  analysisManager->CreateH1("LGap","trackL in gap (mm)", 100, 0., 50*cm);
  analysisManager->CreateH1("LGas","trackL in chamber (mm)", 50, 0., 1*m);

  analysisManager->CreateH1("EPrimary","Energy of Primaries (MeV)", 50, 0., 80*GeV);

  // Create ntuples.
  // Ntuples ids are generated automatically starting from 0.
  // The start value can be changed by:
  // analysisManager->SetFirstMtupleId(1);  
  
  // Create 1st ntuple (id = 0)
  analysisManager->CreateNtuple("NtupleE", "Edep");
  analysisManager->CreateNtupleDColumn("Egas"); // column Id = 0
//  analysisManager->CreateNtupleDColumn("Egap"); // column Id = 1
  analysisManager->CreateNtupleDColumn("Lgas"); // column Id = 1
  analysisManager->FinishNtuple();

  // Create 2nd ntuple (id = 1)
  analysisManager->CreateNtuple("PrimaryE", "PrimaryE");
  analysisManager->CreateNtupleDColumn("PrimE"); // column Id = 0
  analysisManager->FinishNtuple();

  
  fFactoryOn = true;       

  G4cout << "\n----> Output file is open in " 
         << analysisManager->GetFileName() << "." 
         << analysisManager->GetFileType() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Save()
{
  if(!fEnabled)
    return;
  if (! fFactoryOn) return;
  
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();    
  analysisManager->Write();
  analysisManager->CloseFile(); 
   
  G4cout << "\n----> Histograms and ntuples are saved\n" << G4endl;
      
  delete G4AnalysisManager::Instance();
  fFactoryOn = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::FillHisto(G4int ih, G4double xbin, G4double weight)
{
  if(!fEnabled)
    return;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  analysisManager->FillH1(ih, xbin, weight);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Normalize(G4int ih, G4double fac)
{
  if(!fEnabled)
    return;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance(); 
  G4H1* h1 = analysisManager->GetH1(ih);
  if (h1) h1->scale(fac);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//void HistoManager::FillNtuple(G4double energyAbs, G4double energyGap,
//                              G4double trackLAbs, G4double trackLGap)
//{
//  if(!fEnabled)
//    return;
//  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
//  // Fill 1st ntuple ( id = 0)
//  analysisManager->FillNtupleDColumn(0, 0, energyAbs);
//  analysisManager->FillNtupleDColumn(0, 1, energyGap);
//  analysisManager->AddNtupleRow(0);  
//  // Fill 2nd ntuple ( id = 1)
//  analysisManager->FillNtupleDColumn(1, 0, trackLAbs);
//  analysisManager->FillNtupleDColumn(1, 1, trackLGap);
//  analysisManager->AddNtupleRow(1);  
//}  
void HistoManager::FillNtuple(G4double energydep, G4double tracklen, G4double primaryenergy)
{
  if(!fEnabled)
    return;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  // Fill 1st ntuple ( id = 0)
  analysisManager->FillNtupleDColumn(0, 0, energydep);
  analysisManager->FillNtupleDColumn(0, 1, tracklen);
  analysisManager->AddNtupleRow(0);  
  // Fill 2nd ntuple ( id = 1)
  analysisManager->FillNtupleDColumn(1, 0, primaryenergy);
  analysisManager->AddNtupleRow(1);  
}

void HistoManager::FillNtupleEachStep(G4int evtid, G4double xposition, G4double yposition, G4double zposition, G4double energyStep, G4double stepsize)
{
  if(!fEnabled)
    return;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  // Fill ntuples  ( id = evtID + 2 )
  analysisManager->FillNtupleDColumn(evtid+2, 0, xposition);
  analysisManager->FillNtupleDColumn(evtid+2, 1, yposition);
  analysisManager->FillNtupleDColumn(evtid+2, 2, zposition);
  analysisManager->FillNtupleDColumn(evtid+2, 3, energyStep);
  analysisManager->FillNtupleDColumn(evtid+2, 4, stepsize);
  analysisManager->AddNtupleRow(evtid+2);
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::PrintStatistic()
{
  if(!fEnabled)
    return;
  if (! fFactoryOn) return;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    
  G4cout << "\n ----> print histograms statistic \n" << G4endl;
  for ( G4int i=0; i<analysisManager->GetNofH1s(); ++i ) {
    G4String name = analysisManager->GetH1Name(i);
    G4H1* h1 = analysisManager->GetH1(i);
    
    G4String unitCategory;
    if (name[0U] == 'E' ) unitCategory = "Energy"; 
    if (name[0U] == 'L' ) unitCategory = "Length";
         // we use an explicit unsigned int type for operator [] argument
         // to avoid problems with windows compiler

    G4cout << name
           << ": mean = " << G4BestUnit(h1->mean(), unitCategory) 
           << " rms = " << G4BestUnit(h1->rms(), unitCategory ) 
           << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
