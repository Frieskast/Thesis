#include "mysteppingaction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4VProcess.hh"
#include "G4AnalysisManager.hh"
#include <cmath>
#include <cfloat>
#include "G4Exception.hh"
#include "G4RunManager.hh"
#include "runaction.hh"

MySteppingAction::MySteppingAction() : fExitCount(0) {
    G4AccumulableManager::Instance()->Register(fExitCount);
}

MySteppingAction::~MySteppingAction() {
    G4cout << "Total protons exited the gold block (this thread): " << fExitCount.GetValue() << G4endl;
}

void MySteppingAction::UserSteppingAction(const G4Step* step) {
    auto analysisManager = G4AnalysisManager::Instance();

    G4Track* track = step->GetTrack();
    if (track->GetDefinition()->GetParticleName() == "proton") {
        G4StepPoint* preStepPoint = step->GetPreStepPoint();
        G4StepPoint* postStepPoint = step->GetPostStepPoint();

        // Robust null checks
        if (!preStepPoint || !postStepPoint) return;
        if (!preStepPoint->GetPhysicalVolume()) return;

        G4String preVolume = preStepPoint->GetPhysicalVolume()->GetName();
        G4String postVolume = postStepPoint->GetPhysicalVolume() ? postStepPoint->GetPhysicalVolume()->GetName() : "OutOfWorld";

        // Only count if truly exiting the gold block
        if (preVolume == "physGoldBlock" && postVolume != "physGoldBlock" &&
            postStepPoint->GetStepStatus() == fGeomBoundary &&
            track->GetTrackStatus() == fAlive) {

            // Only analyze if the proton is still alive after exiting
            fExitCount++;

            const MyRunAction* constRunAction = static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
            G4int incidentCount = 0;
            if (constRunAction) {
                auto runAction = const_cast<MyRunAction*>(constRunAction);
                if (runAction) {
                    runAction->AddExitCount(1);
                }
                incidentCount = constRunAction->GetIncidentCount();
            }

            // Get the momentum and energy of the exiting particle
            G4ThreeVector momentum = track->GetMomentum();
            G4double energy = track->GetKineticEnergy();

            // Normalize the momentum vector to map onto the unit sphere
            G4ThreeVector unitVector = momentum.unit();

            // Elevation: angle from x-z plane (y=0), +90 = +y, 0 = x-z plane, -90 = -y
            G4double theta = std::acos(unitVector.y()); // [0, pi] radians
            G4double elevation_deg = 90.0 - theta / deg; // [0, 90] for +y, [0, -90] for -y

            // Azimuth: angle in x-z plane from -z axis, increasing towards +x
            G4double azimuth = std::atan2(unitVector.x(), -unitVector.z());
            G4double azimuth_deg = azimuth / deg;

            // // Print for debugging
            // G4cout << "Unit vector: ("
            //        << unitVector.x() << ", "
            //        << unitVector.y() << ", "
            //        << unitVector.z() << ")" << G4endl;
            // G4cout << "Elevation (deg): " << elevation_deg
            //        << ", Azimuth (deg): " << azimuth_deg << G4endl;

            if (std::isnan(energy) || std::isnan(elevation_deg) || std::isnan(azimuth)) {
                G4cerr << "Error: Invalid data for ntuple filling!" << G4endl;
                return;
            }

            // Fill histograms
            // analysisManager->FillH1(0, energy / keV); // Bin energy
            analysisManager->FillH1(1, elevation_deg); // Bin elevation angle
            analysisManager->FillH1(2, azimuth_deg);  // Bin azimuth angle

            // Calculate X and Z components for the X-Z projection
            G4double x = unitVector.x();
            G4double y = unitVector.y();
            G4double z = unitVector.z();

            analysisManager->FillH2(0, x, z);
            // Fill the 2D histogram for the X-Z projection
            analysisManager->FillH2(1, y, z); // Histogram ID 0 for X-Z projection

            // Fill 2D histogram: Azimuth vs Elevation (on the unit sphere)
            analysisManager->FillH2(2, elevation_deg , azimuth_deg);

            // Calculate Exit Energy/Incident Energy ratio
            G4double incidentEnergy = 1000 * keV; // Example incident energy
            G4double energyRatio = energy / incidentEnergy;

            // Get the current exit count and total incident count
            G4int exitCount = fExitCount.GetValue();
        
            if (constRunAction) {
                incidentCount = constRunAction->GetIncidentCount();
            }
            G4double fraction = (incidentCount > 0) ? (G4double)exitCount / incidentCount : 0.0;

            // Fill 2D histogram: Fraction vs Exit Energy/Incident Energy
            analysisManager->FillH1(3, energyRatio); // Bin energy
            analysisManager->FillH2(3, energyRatio, fraction);


            // Fill ntuple
            // analysisManager->FillNtupleDColumn(0, energy / keV);
            // analysisManager->FillNtupleDColumn(1, theta / CLHEP::deg);
            // analysisManager->FillNtupleDColumn(2, beta / CLHEP::deg);
            // analysisManager->AddNtupleRow();
        }
    }
}