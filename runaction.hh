#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"

class G4Run;

class MyRunAction : public G4UserRunAction {
public:
    MyRunAction();
    virtual ~MyRunAction();

    virtual void BeginOfRunAction(const G4Run* run);
    virtual void EndOfRunAction(const G4Run* run);

    void AddExitCount(G4int n) { fExitCount += n; }
    G4int GetIncidentCount() const { return fIncidentCount; }
    void IncrementExitCount() { fExitCount += 1; }
    G4int GetExitCount() const { return fExitCount.GetValue(); }
    G4String GenerateFileName();

private:
    G4Accumulable<G4int> fExitCount;
    G4int fIncidentCount = 0; // Add this line
    G4double fIncidentEnergy = 0;
    double fIncidentAngleDeg = 0;
    double fEnergyKeV = 0;
    std::string fPendingFileName;
public:
    void SetIncidentEnergy(G4double e) { fIncidentEnergy = e; }
    G4double GetIncidentEnergy() const { return fIncidentEnergy; }
    void SetIncidentAngle(double angleDeg) { fIncidentAngleDeg = angleDeg; }
    void SetEnergy(double energyKeV) { fEnergyKeV = energyKeV; }
};

#endif
