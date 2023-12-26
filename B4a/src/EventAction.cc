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
//
/// \file EventAction.cc
/// \brief Implementation of the B4a::EventAction class

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include <numeric>

namespace B4a
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
  // initialisation per event
  fEnergyGap = 0.;
  fTrackLAbs = 0.;
  fTrackLGap = 0.;
  fTrackLSen = 0.;
  for (int i = 0 ; i < NofCells*NofCells; i++) {
    fEnergyAbs[i] = 0.;
    fEnergySen[i] = 0.;
  }
  G4cout<<"EventStart:"<<G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  // Accumulate statistics
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetNtupleMerging(true);
  // fill histograms
  // analysisManager->FillH1(0, fEnergyAbs);
  // analysisManager->FillH1(1, fEnergyGap);
  // analysisManager->FillH1(2, fTrackLAbs);
  // analysisManager->FillH1(3, fTrackLGap);
  // analysisManager->FillH1(4, fTrackLSen);
  // analysisManager->FillH1(5, fEnergySen);

  // fill ntuple
  analysisManager->FillNtupleDColumn(0, fEnergyGap);
  analysisManager->FillNtupleDColumn(1, fTrackLAbs);
  analysisManager->FillNtupleDColumn(2, fTrackLGap);
  analysisManager->FillNtupleDColumn(3, fTrackLSen);
  for (int i = 0; i < NofCells * NofCells; i++) {
    analysisManager->FillNtupleDColumn(i + 4, fEnergySen[i]);
  }
  for (int i = 0; i < NofCells * NofCells; i++) {
    analysisManager->FillNtupleDColumn(i + NofCells * NofCells + 4,
                                       fEnergyAbs[i]);
  }
  analysisManager->AddNtupleRow();

  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    G4cout << "---> End of event: " << eventID << G4endl;

    G4cout
       << "   Absorber: total energy: " << std::setw(7)
                                        << G4BestUnit(std::accumulate(fEnergyAbs.begin(), fEnergyAbs.end(), 0),"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(fTrackLAbs,"Length")
       << G4endl
       << "        Gap: total energy: " << std::setw(7)
                                        << G4BestUnit(fEnergyGap,"Energy")
       << "       total track length: " << std::setw(7)
                                        << G4BestUnit(fTrackLGap,"Length")
       << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
