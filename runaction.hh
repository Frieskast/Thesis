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

private:
    G4Accumulable<G4int> fExitCount;
    G4int fIncidentCount = 0; // Add this line
};

#endif
