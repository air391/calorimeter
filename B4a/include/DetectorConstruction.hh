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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef B4DetectorConstruction_h
#define B4DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include <G4Material.hh>
#include "globals.hh"
#include <vector>
#include "CaloValue.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

namespace B4
{

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

struct Pixel {
  G4double x;
  G4double y;
  G4VPhysicalVolume* pv;
};

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

  public:
    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // get methods
    //
    const std::vector<struct Pixel>& GetAbsorberPV() const;
    const std::vector<struct Pixel>& GetSensitivePV() const;
    const G4VPhysicalVolume* GetGapPV() const;

  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    void LayerConstruction(G4double x, G4double y, G4LogicalVolume* layerLV);
    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
                                      // magnetic field messenger
  // Geometry parameters
  G4int nofLayers = NofLayers;
  G4int nofCells = NofCells;
  G4double absoThickness = AbsoThickness;
  G4double sensThickness = SensThickness;
  G4double gapThickness =  GapThickness;
  G4double calorSizeXY  = CalorSizeXY;

  G4double layerThickness = absoThickness +  sensThickness + gapThickness;
  G4double calorThickness = nofLayers * layerThickness;
  G4double calorLength = nofCells * calorSizeXY;
  G4double worldSizeXY = 1.2 * calorLength;
  G4double worldSizeZ  = 1.2 * calorThickness + 5*m;

  // Get materials
  G4Material * defaultMaterial = nullptr;
  G4Material * absorberMaterial = nullptr;
  G4Material * sensMaterial = nullptr;
  G4Material * gapMaterial = nullptr;

  std::vector<struct Pixel> fAbsorberPV = std::vector<struct Pixel>(); // the absorber physical volume
  std::vector<struct Pixel> fSensitivePV = std::vector<struct Pixel>(); // the absorber physical volume
  G4VPhysicalVolume* fGapPV = nullptr;      // the gap physical volume

  G4bool fCheckOverlaps = true; // option to activate checking of volumes overlaps
};

// inline functions

inline const std::vector<struct Pixel>& DetectorConstruction::GetAbsorberPV() const {
  return fAbsorberPV;
}

inline const std::vector<struct Pixel>& DetectorConstruction::GetSensitivePV() const {
  return fSensitivePV;
}

inline const G4VPhysicalVolume* DetectorConstruction::GetGapPV() const  {
  return fGapPV;
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

