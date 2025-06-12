#include "physics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4CoulombScattering.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4hIonisation.hh"
#include "G4Proton.hh"
#include "G4ParticleTable.hh"
#include "G4EmParameters.hh"
#include "G4ProductionCutsTable.hh"
#include "G4ProcessManager.hh"
#include "G4DecayPhysics.hh"
#include "G4GoudsmitSaundersonMscModel.hh"
#include "G4UrbanMscModel.hh"
#include "G4hMultipleScattering.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

MyPhysicsMessenger::MyPhysicsMessenger() {
    thetaLimitCmd = new G4UIcmdWithADoubleAndUnit("/msc/thetaLimit", this);
    thetaLimitCmd->SetGuidance("Set the MscThetaLimit parameter (in radians)");
    thetaLimitCmd->SetParameterName("thetaLimit", false);
    thetaLimitCmd->SetDefaultUnit("rad");
    thetaLimitCmd->SetRange("thetaLimit>=0.0");
}

MyPhysicsMessenger::~MyPhysicsMessenger() {
    delete thetaLimitCmd;
}

void MyPhysicsMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == thetaLimitCmd) {
        G4double val = thetaLimitCmd->GetNewDoubleValue(newValue);
        // Call the global instance to set the value
        G4EmParameters::Instance()->SetMscThetaLimit(val);
        G4cout << "Set MscThetaLimit to " << val << G4endl;
    }
}

MyPhysicsList::MyPhysicsList() {
    // Register electromagnetic physics
    RegisterPhysics(new G4EmStandardPhysics_option4());

    // Optional: Register Livermore physics for low-energy accuracy (<10 keV)
    // RegisterPhysics(new G4EmLivermorePhysics());

    // Register hadronic physics
    // RegisterPhysics(new G4HadronPhysicsQGSP_BIC());
    RegisterPhysics(new G4HadronElasticPhysics());
    

    // Configure step-cut parameters
    G4EmParameters* emParams = G4EmParameters::Instance();
    emParams->SetStepFunction(0.05, 0.01* CLHEP::um);      // Default or slightly tighter
    emParams->SetMscStepLimitType(fUseSafetyPlus);        // More physical at boundaries
    emParams->SetMscRangeFactor(0.04);                    // Default is 0.04, try 0.02–0.04
    emParams->SetMscGeomFactor(3.5);                      // Default is 3.5
    emParams->SetMscSkin(3);                              // Default is 3
    emParams->SetMscThetaLimit(0.14);                      // 0 disables theta limit

    // Set global energy range for production cuts
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(0.000001 * CLHEP::eV, 100 * CLHEP::GeV);

    mscMessenger = new MyPhysicsMessenger();
}

MyPhysicsList::~MyPhysicsList() {
    delete mscMessenger;
}

void MyPhysicsList::SetCuts() {
    // Call the base class method
    G4VUserPhysicsList::SetCuts();

    // Set specific cuts for individual particles
    SetCutValue(0.000001 * CLHEP::keV, "gamma");
    SetCutValue(0.000001 * CLHEP::keV, "e-");
    SetCutValue(0.000001 * CLHEP::keV, "e+");
    SetCutValue(0.000001 * CLHEP::keV, "proton");

    // Print the cuts for verification
    // G4ProductionCutsTable::GetProductionCutsTable()->DumpCouples();
}

void MyPhysicsList::ConstructProcess() {
    // Call the base class implementation to register processes from physics lists
    G4VModularPhysicsList::ConstructProcess();

    // --- Set Multiple Scattering Model for Protons ---
    G4ParticleDefinition* proton = G4Proton::ProtonDefinition();
    G4ProcessManager* pmanager = proton->GetProcessManager();

    // Remove existing msc processes for protons
    G4int nProc = pmanager->GetProcessListLength();
    for (G4int i = nProc - 1; i >= 0; --i) {
        G4VProcess* proc = (*pmanager->GetProcessList())[i];
        if (proc->GetProcessName() == "msc") {
            pmanager->RemoveProcess(proc);
        }
    }

    // --- Goudsmit-Saunderson Model ---
    auto gsModel = new G4GoudsmitSaundersonMscModel();
    auto msc = new G4hMultipleScattering();
    msc->AddEmModel(0, gsModel);
    pmanager->AddProcess(msc, -1, 1, 1);

    // // --- Urban Model (uncomment to use instead) ---
    // auto urbanModel = new G4UrbanMscModel();
    // auto mscUrban = new G4hMultipleScattering();
    // mscUrban->AddEmModel(0, urbanModel);
    // pmanager->AddProcess(mscUrban, -1, 1, 1);
}