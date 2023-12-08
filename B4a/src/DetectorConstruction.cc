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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include <G4Types.hh>

namespace B4
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_W");
  nistManager->FindOrBuildMaterial("G4_Si");

  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;
  G4double density;

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  defaultMaterial = G4Material::GetMaterial("Galactic");
  absorberMaterial = G4Material::GetMaterial("G4_W");
  sensMaterial = G4Material::GetMaterial("G4_Si");
  gapMaterial = G4Material::GetMaterial("Galactic");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::LayerConstruction(G4double x, G4double y, G4LogicalVolume* layerLV)
{
  //
  // Absorber
  //
  auto absorberS
    = new G4Box("Abso",            // its name
                 calorSizeXY/2, calorSizeXY/2, absoThickness/2); // its size

  auto absorberLV
    = new G4LogicalVolume(
                 absorberS,        // its solid
                 absorberMaterial, // its material
                 "Abso");          // its name

  fAbsorberPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(x, y, -(sensThickness + gapThickness)/2), // its position
                 absorberLV,       // its logical volume
                 "Abso",           // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps
  //
  // Sensitive
  //
  auto sensitiveS
    = new G4Box("Sens",            // its name
                 calorSizeXY/2, calorSizeXY/2, sensThickness/2); // its size
  
  auto sensitiveLV
    = new G4LogicalVolume(
                 sensitiveS,       // its solid
                 sensMaterial,     // its material
                 "Sens");          // its name

  fSensitivePV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(x, y, (absoThickness - gapThickness)/2), // its position
                 sensitiveLV,      // its logical volume
                 "Sens",           // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps
  //
  // Gap
  //
  auto gapS
    = new G4Box("Gap",             // its name
                 calorSizeXY/2, calorSizeXY/2, gapThickness/2); // its size

  auto gapLV
    = new G4LogicalVolume(
                 gapS,             // its solid
                 gapMaterial,      // its material
                 "Gap");           // its name

  fGapPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(x, y, (absoThickness + sensThickness)/2), // its position
                 gapLV,            // its logical volume
                 "Gap",            // its name
                 layerLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // // Geometry parameters
  // G4int nofLayers = 26;
  // G4int nofCells = 40;
  // G4double absoThickness = 3.5*mm;
  // G4double sensThickness = 0.3*mm;
  // G4double gapThickness =  1.7*mm;
  // G4double calorSizeXY  = 1.01*cm;

  // auto layerThickness = absoThickness +  sensThickness + gapThickness;
  // auto calorThickness = nofLayers * layerThickness;
  // auto calorLength = nofCells * calorSizeXY;
  // auto worldSizeXY = 1.2 * calorLength;
  // auto worldSizeZ  = 1.2 * calorThickness;

  // // Get materials
  // auto defaultMaterial = G4Material::GetMaterial("Galactic");
  // auto absorberMaterial = G4Material::GetMaterial("G4_W");
  // auto sensMaterial = G4Material::GetMaterial("G4_Si");
  // auto gapMaterial = G4Material::GetMaterial("Galactic");

  if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial || ! sensMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined.";
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }

  //
  // World
  //
  auto worldS
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

  auto worldLV
    = new G4LogicalVolume(
                 worldS,           // its solid
                 defaultMaterial,  // its material
                 "World");         // its name

  auto worldPV
    = new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 worldLV,          // its logical volume
                 "World",          // its name
                 0,                // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

  //
  // Calorimeter
  //
  auto calorimeterS
    = new G4Box("Calorimeter",     // its name
                 calorLength/2, calorLength/2, calorThickness/2); // its size

  auto calorLV
    = new G4LogicalVolume(
                 calorimeterS,     // its solid
                 defaultMaterial,  // its material
                 "Calorimeter");   // its name

  new G4PVPlacement(
                 0,                // no rotation
                 G4ThreeVector(),  // at (0,0,0)
                 calorLV,          // its logical volume
                 "Calorimeter",    // its name
                 worldLV,          // its mother  volume
                 false,            // no boolean operation
                 0,                // copy number
                 fCheckOverlaps);  // checking overlaps

  //
  // Layer
  //
  auto layerS
    = new G4Box("Layer",           // its name
                 calorLength/2, calorLength/2, layerThickness/2); // its size

  auto layerLV
    = new G4LogicalVolume(
                 layerS,           // its solid
                 defaultMaterial,  // its material
                 "Layer");         // its name

  new G4PVReplica(
              "Layer",          // its name
              layerLV,          // its logical volume
              calorLV,          // its mother
              kZAxis,           // axis of replication
              nofLayers,        // number of replica
              layerThickness);  // witdth of replica


  for (G4int i = 0; i < 40; ++i) {
    for (G4int j = 0; j < 40; ++j) {
        auto x = (i - 19.5) * calorSizeXY;
        auto y = (j - 19.5) * calorSizeXY;
        // auto x = 0;
        // auto y = 0;
        LayerConstruction(x, y, layerLV);
    }
  }

  //
  // print parameters
  //
  G4cout
    << G4endl
    << "------------------------------------------------------------" << G4endl
    << "---> The calorimeter is " << nofLayers << " layers of: [ "
    << absoThickness/mm << "mm of " << absorberMaterial->GetName()
    << " + "
    << sensThickness/mm << "mm of " << sensMaterial->GetName()
    << " + "
    << gapThickness/mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
    << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

