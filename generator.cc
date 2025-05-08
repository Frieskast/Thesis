#include "generator.hh"
#include "G4Event.hh" // Include for G4Event
#include "Randomize.hh" // Include for G4UniformRand
#include "G4SystemOfUnits.hh" // For unit definitions
#include "G4ProductionCutsTable.hh" // Include for production cuts table
#include "G4RegionStore.hh" // Include for region store
#include "G4ProductionCuts.hh" // Include for production cuts

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1); // Initialize particle gun with 1 particle per event

    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(0.1 * eV, 100 * GeV);

    G4double lowEnergyCut = 0.0001 * keV;
    G4ProductionCuts* cuts = new G4ProductionCuts();
    cuts->SetProductionCut(lowEnergyCut, "gamma");
    cuts->SetProductionCut(lowEnergyCut, "e-");
    cuts->SetProductionCut(lowEnergyCut, "e+");
    cuts->SetProductionCut(lowEnergyCut, "proton");

    G4Region* region = G4RegionStore::GetInstance()->GetRegion("DefaultRegionForTheWorld");
    region->SetProductionCuts(cuts);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Generate a proton
    G4String particleName = "proton";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);

    // Calculate the starting position based on the event ID
    G4int eventID = anEvent->GetEventID();
    G4double angle = eventID * (5.0 / 1000.0) * deg; // Increment angle from 0 to 5 degrees over 100 events
    if (angle > 5.0 * deg) {
        angle = 5.0 * deg; // Cap the angle at 5 degrees
    }

    // Set the particle's starting position
    G4double yStart = 0.001 * m + std::tan(angle) * 1.0 * m; // Offset in y-axis based on angle
    G4ThreeVector pos(0.1 * m, yStart, 0.0 * m); // Start 1 meter above the origin
    fParticleGun->SetParticlePosition(pos);

    // Set the momentum direction to point toward the origin
    G4ThreeVector mom(-0.1 * m, -yStart, 0.0); // Momentum direction toward the origin
    mom = mom.unit(); // Normalize the momentum vector

    // // Generate a random position on a disk around the x-axis
    // G4double rMin = 2.0 * cm; // Minimum radius
    // G4double rMax = 5.0 * cm; // Maximum radius
    // G4double r = std::sqrt(G4UniformRand() * (rMax * rMax - rMin * rMin) + rMin * rMin); // Random radius
    // G4double theta = 2.0 * M_PI * G4UniformRand(); // Random angle

    // G4double yStart = r * std::cos(theta); // y-coordinate on the disk
    // G4double zStart = r * std::sin(theta); // z-coordinate on the disk
    // G4ThreeVector pos(0.2 * m, yStart, zStart); // Position on the disk
    // fParticleGun->SetParticlePosition(pos);

    // Set the momentum direction to point along the negative x-axis
    // G4ThreeVector mom(-1.0, 0.0, 0.0); // Momentum direction along -x-axis
    fParticleGun->SetParticleMomentumDirection(mom);

    // Set the particle energy
    fParticleGun->SetParticleMomentum(1400* keV); // Example energy

    // Set the particle definition
    fParticleGun->SetParticleDefinition(particle);

    // Generate the primary vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);
}

