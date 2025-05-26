#include <iostream>
#include <fstream>
#include "G4MTRunManager.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "physics.hh"
#include "detector.hh"
#include "action.hh"
#include "G4VExceptionHandler.hh"
#include "G4ios.hh"
#include "construction.hh"
#include "G4UserLimits.hh"
#include "runaction.hh"


class MyExceptionHandler : public G4VExceptionHandler {
public:
    G4bool Notify(const char* originOfException, const char* exceptionCode,
        G4ExceptionSeverity severity, const char* description) override {
        G4cerr << "\n*** Custom G4Exception Handler ***" << G4endl;
        G4cerr << "Exception from: " << originOfException << G4endl;
        G4cerr << "Code: " << exceptionCode << G4endl;
        G4cerr << "Severity: " << severity << G4endl;
        G4cerr << "Description: " << description << G4endl;

        if (severity == FatalException) {
            G4cerr << "\nFatal error encountered! Pausing instead of exiting...\n";
            G4cerr << "Press Enter to continue." << G4endl;
            std::cin.get();  // Wait for user input before continuing
            return true;  // Continue execution instead of aborting
        }
        return false;  // Use default behavior for non-fatal exceptions
    }
};

int main(int argc, char** argv) {
#ifdef G4MULTITHREADED
    G4MTRunManager* runManager = new G4MTRunManager();
    runManager->SetNumberOfThreads(8); // Set the number of threads
#else
    G4RunManager* runManager = new G4RunManager();
#endif
    
    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new MyActionInitialization());

    // runManager->Initialize();

    G4UIExecutive* ui = 0;

    if(argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }
    
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if(ui)
    {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();    
    }
    else
    {
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command+fileName);
    }    

    // UImanager->ApplyCommand("/tracking/verbose 2"); // Enable verbose tracking
    // UImanager->ApplyCommand("/physics_engine/verbose 2"); // Enable verbose tracking
    // UImanager->ApplyCommand("/process/verbose 2"); // Enable verbose tracking
    // UImanager->ApplyCommand("/analysis/verbose 1");
    // UImanager->ApplyCommand("/run/verbose 2");

    // UImanager->ApplyCommand("/vis/open OGL");
    // UImanager->ApplyCommand("/vis/viewer/set/viewpointVector 1 1 1");
    // UImanager->ApplyCommand("/vis/drawVolume");
    // UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
    // UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");
    // UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
    // UImanager->ApplyCommand("/vis/scene/add/axes 0 0 0 0.1"); // Axes length 10 cm, centered at origin
    // UImanager->ApplyCommand("/cuts/setLowEdge 10 eV");
    // UImanager->ApplyCommand("/run/setCut 1 um");
    
    // G4cout << "Physics processes and models for protons:" << G4endl;

    // G4ProcessManager* pManager = G4ParticleTable::GetParticleTable()->FindParticle("proton")->GetProcessManager();
    // if (pManager) {
    //     G4ProcessVector* processVector = pManager->GetProcessList();
    //     for (size_t i = 0; i < processVector->size(); ++i) {
    //         G4VProcess* process = (*processVector)[i];
    //         G4cout << "Process: " << process->GetProcessName() << G4endl;

    //         // Use DumpInfo() to output detailed information about the process
    //         process->DumpInfo();
    //     }
    
    // }

    delete visManager;
    delete runManager;

    return 0;
}

