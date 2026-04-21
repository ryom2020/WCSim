#ifndef WCSimEventAction_h
#define WCSimEventAction_h 1


#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4ios.hh"

#include "WCSimDetectorConstruction.hh"
#include "G4TrajectoryContainer.hh"
#include "WCSimWCHit.hh"
#include "WCSimWCDigi.hh"
#include "WCSimWCTrigger.hh"
#include "WCSimWCDAQMessenger.hh"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"

#include "TRandom3.h"

class WCSimRunAction;
class WCSimPrimaryGeneratorAction;
class G4Event;

class WCSimEventAction : public G4UserEventAction
{
private:
  WCSimRunAction* runAction;
  WCSimPrimaryGeneratorAction* generatorAction;
  WCSimDetectorConstruction*   detectorConstructor;

  TRandom3 * randGen;
  WCSimWCDAQMessenger* DAQMessenger;

  
public:
  WCSimEventAction(WCSimRunAction*, WCSimDetectorConstruction*,
		   WCSimPrimaryGeneratorAction*);
  ~WCSimEventAction();
  
public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void FillRootEvent(G4int, 
		     const struct ntupleStruct&, 
		     G4TrajectoryContainer*,
		     WCSimWCDigitsCollection*,
		     WCSimWCTriggeredDigitsCollection*,
		     G4String detectorElement="tank",
		     bool skipFillingTracks=false);
  void FillRootEventHybrid(G4int, 
			   const struct ntupleStruct&, 
			   G4TrajectoryContainer*,
			   WCSimWCDigitsCollection*,
			   WCSimWCTriggeredDigitsCollection*,
			   G4String,
			   WCSimRootEvent*,
			   WCSimRootTrigger*,
			   bool skipFillingTracks);
  WCSimRunAction* GetRunAction(){return runAction;}
  void SetDigitizerChoice(G4String digitizer) { DigitizerChoice = digitizer; }
  void SetTriggerChoice  (G4String trigger)   { TriggerChoice   = trigger;   }
  void SetRelativeDigitizedHitTime (bool val) { RelativeHitTime = val;       }

  void FillFlatTree(G4int,
		    const struct ntupleStruct&, 
		    G4TrajectoryContainer*,
		    WCSimWCDigitsCollection*,
		    WCSimWCTriggeredDigitsCollection*,
		    G4String detectorElement);

  
private:
  G4int WCSimEventFindStartingVolume( G4ThreeVector vtx);
  G4int WCSimEventFindStoppingVolume( G4String stopVolumeName);

  G4String vtxVolumeName;         //TF new

  ///Create instances of the user-chosen digitizer and trigger classes
  void  CreateDAQInstances();

  G4String DigitizerChoice;
  G4String TriggerChoice;
  bool     RelativeHitTime;
  bool     ConstructedDAQClasses;
  bool     SavedOptions;

  G4int fEvNum;


  //for evis
private:
  std::array<Double_t, 1000> fEdepID;//energy deposit
  std::array<Double_t, 1000> fEdepDS;
  std::array<Double_t, 1000> fEdepOD;
public:
  Double_t GetEdepBinWidth(){ return 10; }//ns
  int GetEdepTindex(G4double time){
    if(time<0) return 0;
    int it = time/GetEdepBinWidth();//ns
    if(it>=fEdepID.size()) return fEdepID.size()-1;
    return it;
  }
  void AddEdepID(G4double edep, G4double time) { fEdepID[GetEdepTindex(time)] += edep;  }
  void AddEdepDS(G4double edep, G4double time) { fEdepDS[GetEdepTindex(time)] += edep;  }
  void AddEdepOD(G4double edep, G4double time) { fEdepOD[GetEdepTindex(time)] += edep;  }
  std::array<Double_t, 1000> GetEdepID() const { return fEdepID; }
  std::array<Double_t, 1000> GetEdepDS() const { return fEdepDS; }
  std::array<Double_t, 1000> GetEdepOD() const { return fEdepOD; }
  
  //for entering gamma BG study
private:
  std::vector<double> gammaEnergies;
  std::vector<std::array<double, 3>> gammaPositions;
public:
  const std::vector<double>& GetGammaEnergies() const { return gammaEnergies; }
  const std::vector<std::array<double, 3>>& GetGammaPositions() const { return gammaPositions; }
  void AddGammaHit(double energy, const G4ThreeVector& position) {
    if(gammaEnergies.size()>500) return;
    gammaEnergies.push_back(energy);
    std::array<double, 3> pos;
    pos[0] = position.x();
    pos[1] = position.y();
    pos[2] = position.z();
    gammaPositions.push_back(pos);
  }  
  void ClearGammaHits() {
    gammaEnergies.clear();
    gammaPositions.clear();
  }
  
  
};


#endif
    
