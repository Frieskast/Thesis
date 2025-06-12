#include "generator.hh"
#include "G4Event.hh" // Include for G4Event
#include "Randomize.hh" // Include for G4UniformRand
#include "G4SystemOfUnits.hh" // For unit definitions
#include "G4ProductionCutsTable.hh" // Include for production cuts table
#include "G4RegionStore.hh" // Include for region store
#include "G4ProductionCuts.hh" // Include for production cuts
#include "runaction.hh"
#include "G4RunManager.hh"
#include "generatorMessenger.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1); // Initialize particle gun with 1 particle per event

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Generate a proton
    G4String particleName = "proton";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);

    // Calculate the starting position based on the event ID
    // G4int eventID = anEvent->GetEventID();
    // G4double angle = eventID * (5.0 / 1000.0) * deg; // Increment angle from 0 to 5 degrees over 100 events
    // if (angle > 5.0 * deg) {
    //     angle = 5.0 * deg; // Cap the angle at 5 degrees
    // }

    // Set the particle's starting position
    // G4double yStart = 0.001 * m + std::tan(angle) * 1.0 * m; // Offset in y-axis based on angle
    // G4ThreeVector pos(0.1 * m, yStart, 0.0 * m); // Start 1 meter above the origin
    // fParticleGun->SetParticlePosition(pos);

    // Set the momentum direction to point toward the origin
    // G4ThreeVector mom(-0.1 * m, -yStart, 0.0); // Momentum direction toward the origin
   

    // // Set the particle's starting position
    // G4double angle = fIncidentAngle;

    // G4double yStart = -0.1 * m;

    // G4double xStart = yStart * std::tan(angle);
    
    // G4ThreeVector pos(xStart, yStart, 0.0*m);
    // fParticleGun->SetParticlePosition(pos);

    // // Set the momentum direction for an incident angle of 1 degree
    // G4double x = -std::cos(angle); // x-component of momentum
    // G4double y = -std::sin(angle); // y-component of momentum (negative for downward direction)
    // G4double z = 0.0; // z-component of momentum (no deviation in z-axis)

    // G4ThreeVector mom(-xStart, -yStart, 0.0*m);
    // mom = mom.unit(); // Normalize the momentum vector
    // fParticleGun->SetParticleMomentumDirection(mom);

    // Set the particle energy
    fParticleGun->SetParticleEnergy(1000 * keV); // Example energy

    // Set the particle definition
    fParticleGun->SetParticleDefinition(particle);

    fMessenger = new GeneratorMessenger(this);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fMessenger;
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
    // Set the particle's starting position and direction based on current fIncidentAngle
    G4double angle = fIncidentAngle;
    G4double yStart = -0.1 * m;
    G4double xStart = yStart * std::tan(angle);
    G4ThreeVector pos(xStart, yStart, 0.0*m);
    fParticleGun->SetParticlePosition(pos);

    G4ThreeVector mom(-xStart, -yStart, 0.0*m);
    mom = mom.unit();
    fParticleGun->SetParticleMomentumDirection(mom);

    // The energy is set by macro, so no need to set it here unless you want to override

    // Generate the primary vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);

    // Set incident energy in run action
    auto* runAction = const_cast<MyRunAction*>(
        static_cast<const MyRunAction*>(G4RunManager::GetRunManager()->GetUserRunAction())
    );
    if (runAction) {
        runAction->SetIncidentAngle(fIncidentAngle / CLHEP::deg);
        runAction->SetEnergy(fParticleGun->GetParticleEnergy() / CLHEP::keV);
        runAction->SetIncidentEnergy(fParticleGun->GetParticleEnergy());
    }
}

