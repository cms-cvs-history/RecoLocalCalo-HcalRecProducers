using namespace std;
#include "HcalHitReconstructor.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"
#include "DataFormats/Common/interface/EDCollection.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Selector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "CalibFormats/HcalObjects/interface/HcalCoderDb.h"
#include "CalibFormats/HcalObjects/interface/HcalCalibrations.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputer.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSeverityLevelComputerRcd.h"

#include <iostream>

/*  Hcal Hit reconstructor allows for CaloRecHits with status words */

HcalHitReconstructor::HcalHitReconstructor(edm::ParameterSet const& conf):
  reco_(conf.getParameter<int>("firstSample"),
	conf.getParameter<int>("samplesToAdd"),
	conf.getParameter<bool>("correctForTimeslew"),
	conf.getParameter<bool>("correctForPhaseContainment"),
	conf.getParameter<double>("correctionPhaseNS")),
  det_(DetId::Hcal),
  inputLabel_(conf.getParameter<edm::InputTag>("digiLabel")),
  correctTiming_(conf.getParameter<bool>("correctTiming")),
  setNoiseFlags_(conf.getParameter<bool>("setNoiseFlags")),
  setHSCPFlags_(conf.getParameter<bool>("setHSCPFlags")),
  setSaturationFlags_(conf.getParameter<bool>("setSaturationFlags")),
  setTimingTrustFlags_(conf.getParameter<bool>("setTimingTrustFlags")),
  dropZSmarkedPassed_(conf.getParameter<bool>("dropZSmarkedPassed"))
{
  std::string subd=conf.getParameter<std::string>("Subdetector");
  hbheFlagSetter_=0;
  hbheHSCPFlagSetter_=0;
  hbheTimingShapedFlagSetter_ = 0;
  hfdigibit_=0;
  hfrechitbit_=0;
  hfS9S1_=0;
  hfPET_=0;
  saturationFlagSetter_=0;
  HFTimingTrustFlagSetter_=0;
  
  if (setSaturationFlags_)
    {
      const edm::ParameterSet& pssat      = conf.getParameter<edm::ParameterSet>("saturationParameters");
      saturationFlagSetter_ = new HcalADCSaturationFlag(pssat.getParameter<int>("maxADCvalue"));
    }

  if (!strcasecmp(subd.c_str(),"HBHE")) {
    subdet_=HcalBarrel;
    bool timingShapedCutsFlags = conf.getParameter<bool>("setTimingShapedCutsFlags");
    if (timingShapedCutsFlags)
      {
	const edm::ParameterSet& psTshaped = conf.getParameter<edm::ParameterSet>("timingshapedcutsParameters");
	hbheTimingShapedFlagSetter_ = new HBHETimingShapedFlagSetter(psTshaped.getParameter<std::vector<double> >("tfilterEnvelope"),
								     psTshaped.getParameter<bool>("ignorelowest"),
								     psTshaped.getParameter<bool>("ignorehighest"),
								     psTshaped.getParameter<double>("win_offset"),
								     psTshaped.getParameter<double>("win_gain"));
      }
      
    if (setNoiseFlags_)
      {
	const edm::ParameterSet& psdigi    =conf.getParameter<edm::ParameterSet>("flagParameters");
	hbheFlagSetter_=new HBHEStatusBitSetter(psdigi.getParameter<double>("nominalPedestal"),
						psdigi.getParameter<double>("hitEnergyMinimum"),
						psdigi.getParameter<int>("hitMultiplicityThreshold"),
						psdigi.getParameter<std::vector<edm::ParameterSet> >("pulseShapeParameterSets"),
						conf.getParameter<int>("firstSample"),
						conf.getParameter<int>("samplesToAdd"));
      } // if (setNoiseFlags_)
    if (setHSCPFlags_)
      {
	const edm::ParameterSet& psHSCP = conf.getParameter<edm::ParameterSet>("hscpParameters");
	hbheHSCPFlagSetter_ = new HBHETimeProfileStatusBitSetter(psHSCP.getParameter<double>("r1Min"),
								 psHSCP.getParameter<double>("r1Max"),
								 psHSCP.getParameter<double>("r2Min"),
								 psHSCP.getParameter<double>("r2Max"),
								 psHSCP.getParameter<double>("fracLeaderMin"),
								 psHSCP.getParameter<double>("fracLeaderMax"),
								 psHSCP.getParameter<double>("slopeMin"),
								 psHSCP.getParameter<double>("slopeMax"),
								 psHSCP.getParameter<double>("outerMin"),
								 psHSCP.getParameter<double>("outerMax"),
								 psHSCP.getParameter<double>("TimingEnergyThreshold"));
      } // if (setHSCPFlags_) 
    produces<HBHERecHitCollection>();
  } else if (!strcasecmp(subd.c_str(),"HO")) {
    subdet_=HcalOuter;
    produces<HORecHitCollection>();
  } else if (!strcasecmp(subd.c_str(),"HF")) {
    subdet_=HcalForward;
    HFNoiseAlgo_=conf.getParameter<int>("HFNoiseAlgo"); // get noise algorithm type
    HFsetTimingFlagsInMC_ = cms.getParameter<bool>("HFsetTimingFlagsInMC");

    if (setTimingTrustFlags_) {
      
      const edm::ParameterSet& pstrust      = conf.getParameter<edm::ParameterSet>("hfTimingTrustParameters");
      HFTimingTrustFlagSetter_=new HFTimingTrustFlag(pstrust.getParameter<int>("hfTimingTrustLevel1"),
						     pstrust.getParameter<int>("hfTimingTrustLevel2"));
    }

    if (setNoiseFlags_)
      {
	const edm::ParameterSet& psdigi    =conf.getParameter<edm::ParameterSet>("digistat");
	const edm::ParameterSet& psTimeWin =conf.getParameter<edm::ParameterSet>("HFInWindowStat");
	const edm::ParameterSet& psrechit  =conf.getParameter<edm::ParameterSet>("rechitstat");
	hfdigibit_=new HcalHFStatusBitFromDigis(conf.getParameter<int>("firstSample"),
						conf.getParameter<int>("samplesToAdd"),
						psdigi, psTimeWin);

	hfrechitbit_=new HcalHFStatusBitFromRecHits(psrechit.getParameter<double>("short_HFlongshortratio"),
						    psrechit.getParameter<double>("short_HFETthreshold"),
						    psrechit.getParameter<double>("short_HFEnergythreshold"),
						    psrechit.getParameter<double>("long_HFlongshortratio"),
						    psrechit.getParameter<double>("long_HFETthreshold"),
						    psrechit.getParameter<double>("long_HFEnergythreshold")	
						    );
	const edm::ParameterSet& psS9S1   = conf.getParameter<edm::ParameterSet>("S9S1stat");
	hfS9S1_   = new HcalHF_S9S1algorithm(psS9S1.getParameter<std::vector<double> >("short_optimumSlope"),
					     psS9S1.getParameter<std::vector<double> >("shortEnergyParams"),
					     psS9S1.getParameter<std::vector<double> >("shortETParams"),
					     psS9S1.getParameter<std::vector<double> >("long_optimumSlope"),
					     psS9S1.getParameter<std::vector<double> >("longEnergyParams"),
					     psS9S1.getParameter<std::vector<double> >("longETParams")
					     );
	const edm::ParameterSet& psPET    = conf.getParameter<edm::ParameterSet>("PETstat");
	hfPET_    = new HcalHF_PETalgorithm(psPET.getParameter<std::vector<double> >("short_R"),
					    psPET.getParameter<std::vector<double> >("shortEnergyParams"),
					    psPET.getParameter<std::vector<double> >("shortETParams"),
					    psPET.getParameter<std::vector<double> >("long_R"),
					    psPET.getParameter<std::vector<double> >("longEnergyParams"),
					    psPET.getParameter<std::vector<double> >("longETParams")
					    );
      }
    produces<HFRecHitCollection>();
  } else if (!strcasecmp(subd.c_str(),"ZDC")) {
    det_=DetId::Calo;
    subdet_=HcalZDCDetId::SubdetectorId;
    produces<ZDCRecHitCollection>();
  } else if (!strcasecmp(subd.c_str(),"CALIB")) {
    subdet_=HcalOther;
    subdetOther_=HcalCalibration;
    produces<HcalCalibRecHitCollection>();
  } else {
    std::cout << "HcalHitReconstructor is not associated with a specific subdetector!" << std::endl;
  }       
  
}

HcalHitReconstructor::~HcalHitReconstructor() {
  if (hbheFlagSetter_)        delete hbheFlagSetter_;
  if (hfdigibit_)             delete hfdigibit_;
  if (hfrechitbit_)           delete hfrechitbit_;
  if (hbheHSCPFlagSetter_)    delete hbheHSCPFlagSetter_;
  if (hfS9S1_)                delete hfS9S1_;
  if (hfPET_)                 delete hfPET_;
}

void HcalHitReconstructor::produce(edm::Event& e, const edm::EventSetup& eventSetup)
{
  bool isData=e.isRealData(); // some flags should only be applied to real data

  // get conditions
  edm::ESHandle<HcalDbService> conditions;
  eventSetup.get<HcalDbRecord>().get(conditions);
  const HcalQIEShape* shape = conditions->getHcalShape (); // this one is generic
  
  edm::ESHandle<HcalChannelQuality> p;
  eventSetup.get<HcalChannelQualityRcd>().get(p);
  HcalChannelQuality* myqual = new HcalChannelQuality(*p.product());

  edm::ESHandle<HcalSeverityLevelComputer> mycomputer;
  eventSetup.get<HcalSeverityLevelComputerRcd>().get(mycomputer);
  const HcalSeverityLevelComputer* mySeverity = mycomputer.product();
  
  if (det_==DetId::Hcal) {
    if (subdet_==HcalBarrel || subdet_==HcalEndcap) {
      edm::Handle<HBHEDigiCollection> digi;
      
      e.getByLabel(inputLabel_,digi);
      
      // create empty output
      std::auto_ptr<HBHERecHitCollection> rec(new HBHERecHitCollection);
      rec->reserve(digi->size());
      // run the algorithm
      if (setNoiseFlags_) hbheFlagSetter_->Clear();
      HBHEDigiCollection::const_iterator i;
      std::vector<HBHEDataFrame> HBDigis;
      std::vector<int> RecHitIndex;

      // Vote on majority TS0 CapId
      int favorite_capid = 0; 
      if (correctTiming_) {
        long capid_votes[4] = {0,0,0,0};
        for (i=digi->begin(); i!=digi->end(); i++) {
          capid_votes[(*i)[0].capid()]++;
        }
        for (int k = 0; k < 4; k++)
          if (capid_votes[k] > capid_votes[favorite_capid])
            favorite_capid = k;
      }

      for (i=digi->begin(); i!=digi->end(); i++) {
	HcalDetId cell = i->id();
	DetId detcell=(DetId)cell;

	// check on cells to be ignored and dropped: (rof,20.Feb.09)
	const HcalChannelStatus* mydigistatus=myqual->getValues(detcell.rawId());
	if (mySeverity->dropChannel(mydigistatus->getValue() ) ) continue;
	if (dropZSmarkedPassed_)
	  if (i->zsMarkAndPass()) continue;

	const HcalCalibrations& calibrations=conditions->getHcalCalibrations(cell);
	const HcalQIECoder* channelCoder = conditions->getHcalCoder (cell);
	HcalCoderDb coder (*channelCoder, *shape);
	rec->push_back(reco_.reconstruct(*i,coder,calibrations));
	(rec->back()).setFlags(0);
	if (hbheTimingShapedFlagSetter_!=0)
	  hbheTimingShapedFlagSetter_->SetTimingShapedFlags(rec->back());
	if (setNoiseFlags_)
	  hbheFlagSetter_->SetFlagsFromDigi(rec->back(),*i,coder,calibrations);
	if (setSaturationFlags_)
	  saturationFlagSetter_->setSaturationFlag(rec->back(),*i);
	if (correctTiming_)
	  HcalTimingCorrector::Correct(rec->back(), *i, favorite_capid);
	if (setHSCPFlags_ && i->id().ietaAbs()<16)
	  {
	    double DigiEnergy=0;
            for(int j=0; j!=i->size(); DigiEnergy += i->sample(j++).nominal_fC());
            if(DigiEnergy > hbheHSCPFlagSetter_->EnergyThreshold())
              {
                HBDigis.push_back(*i);
                RecHitIndex.push_back(rec->size()-1);
              }
	    
	  } // if (set HSCPFlags_ && |ieta|<16)
      } // loop over HBHE digis


      if (setNoiseFlags_) hbheFlagSetter_->SetFlagsFromRecHits(*rec);
      if (setHSCPFlags_)  hbheHSCPFlagSetter_->hbheSetTimeFlagsFromDigi(rec.get(), HBDigis, RecHitIndex);
      // return result
      e.put(rec);
    } else if (subdet_==HcalOuter) {
      edm::Handle<HODigiCollection> digi;
      e.getByLabel(inputLabel_,digi);
      
      // create empty output
      std::auto_ptr<HORecHitCollection> rec(new HORecHitCollection);
      rec->reserve(digi->size());
      // run the algorithm
      HODigiCollection::const_iterator i;

      // Vote on majority TS0 CapId
      int favorite_capid = 0; 
      if (correctTiming_) {
        long capid_votes[4] = {0,0,0,0};
        for (i=digi->begin(); i!=digi->end(); i++) {
          capid_votes[(*i)[0].capid()]++;
        }
        for (int k = 0; k < 4; k++)
          if (capid_votes[k] > capid_votes[favorite_capid])
            favorite_capid = k;
      }

      for (i=digi->begin(); i!=digi->end(); i++) {
	HcalDetId cell = i->id();
	DetId detcell=(DetId)cell;
	// check on cells to be ignored and dropped: (rof,20.Feb.09)
	const HcalChannelStatus* mydigistatus=myqual->getValues(detcell.rawId());
	if (mySeverity->dropChannel(mydigistatus->getValue() ) ) continue;
	if (dropZSmarkedPassed_)
	  if (i->zsMarkAndPass()) continue;

	const HcalCalibrations& calibrations=conditions->getHcalCalibrations(cell);
	const HcalQIECoder* channelCoder = conditions->getHcalCoder (cell);
	HcalCoderDb coder (*channelCoder, *shape);
	rec->push_back(reco_.reconstruct(*i,coder,calibrations));
	(rec->back()).setFlags(0);
	if (setSaturationFlags_)
	  saturationFlagSetter_->setSaturationFlag(rec->back(),*i);
	if (correctTiming_)
	  HcalTimingCorrector::Correct(rec->back(), *i, favorite_capid);
      }
      // return result
      e.put(rec);    
    } else if (subdet_==HcalForward) {
      edm::Handle<HFDigiCollection> digi;
      e.getByLabel(inputLabel_,digi);
      ///////////////////////////////////////////////////////////////// HF
      // create empty output
      std::auto_ptr<HFRecHitCollection> rec(new HFRecHitCollection);
      rec->reserve(digi->size());
      // run the algorithm
      HFDigiCollection::const_iterator i;

      bool setTimeFlag= HFsetTimingFlagsInMC_||isData;

      // Vote on majority TS0 CapId
      int favorite_capid = 0; 
      if (correctTiming_) {
        long capid_votes[4] = {0,0,0,0};
        for (i=digi->begin(); i!=digi->end(); i++) {
          capid_votes[(*i)[0].capid()]++;
        }
        for (int k = 0; k < 4; k++)
          if (capid_votes[k] > capid_votes[favorite_capid])
            favorite_capid = k;
      }

      for (i=digi->begin(); i!=digi->end(); i++) {
	HcalDetId cell = i->id();
	DetId detcell=(DetId)cell;
	// check on cells to be ignored and dropped: (rof,20.Feb.09)
	const HcalChannelStatus* mydigistatus=myqual->getValues(detcell.rawId());
	if (mySeverity->dropChannel(mydigistatus->getValue() ) ) continue;
	if (dropZSmarkedPassed_)
	  if (i->zsMarkAndPass()) continue;

	const HcalCalibrations& calibrations=conditions->getHcalCalibrations(cell);
	const HcalQIECoder* channelCoder = conditions->getHcalCoder (cell);
	HcalCoderDb coder (*channelCoder, *shape);
	rec->push_back(reco_.reconstruct(*i,coder,calibrations));
	(rec->back()).setFlags(0);
	// This calls the code for setting the HF noise bit determined from digi shape
	if (setNoiseFlags_) hfdigibit_->hfSetFlagFromDigi(rec->back(),*i,coder,calibrations, setTimeFlag);
	if (setSaturationFlags_)
	  saturationFlagSetter_->setSaturationFlag(rec->back(),*i);
	if (setTimingTrustFlags_)
	  HFTimingTrustFlagSetter_->setHFTimingTrustFlag(rec->back(),*i);
	if (correctTiming_)
	  HcalTimingCorrector::Correct(rec->back(), *i, favorite_capid);
      }
      // This sets HF noise bit determined from L/S rechit energy comparison
      if (setNoiseFlags_) 
	{
	  if (HFNoiseAlgo_==1)  // old-style bit setting ("Algo 1")
	    hfrechitbit_->hfSetFlagFromRecHits(*rec,myqual,mySeverity); // old code passes full rechit collection to algo; probably not the best way to go about this.  Instead, loop over rechits directly here

	  else if (HFNoiseAlgo_>1)  // HFNoiseAlgo_==3 is the default
	    {
	      for (HFRecHitCollection::iterator i = rec->begin();i!=rec->end();++i)
		{
		  int depth=i->id().depth();
		  int ieta=i->id().ieta();
		  // Short fibers and all channels at |ieta|=29 use PET settings in Algo 3
		  if ((HFNoiseAlgo_==4) || (HFNoiseAlgo_==3 && depth==1 && abs(ieta)!=29 )) 
		    hfS9S1_->HFSetFlagFromS9S1(*i,*rec,myqual, mySeverity);
		  else
		    hfPET_->HFSetFlagFromPET(*i,*rec,myqual,mySeverity);
		}
	    }
	}
      // return result
      e.put(rec);     
    } else if (subdet_==HcalOther && subdetOther_==HcalCalibration) {
      edm::Handle<HcalCalibDigiCollection> digi;
      e.getByLabel(inputLabel_,digi);
      
      // create empty output
      std::auto_ptr<HcalCalibRecHitCollection> rec(new HcalCalibRecHitCollection);
      rec->reserve(digi->size());
      // run the algorithm
      HcalCalibDigiCollection::const_iterator i;
      for (i=digi->begin(); i!=digi->end(); i++) {
	HcalCalibDetId cell = i->id();
	DetId detcell=(DetId)cell;
	// check on cells to be ignored and dropped: (rof,20.Feb.09)
	const HcalChannelStatus* mydigistatus=myqual->getValues(detcell.rawId());
	if (mySeverity->dropChannel(mydigistatus->getValue() ) ) continue;
	if (dropZSmarkedPassed_)
	  if (i->zsMarkAndPass()) continue;

	const HcalCalibrations& calibrations=conditions->getHcalCalibrations(cell);
	const HcalQIECoder* channelCoder = conditions->getHcalCoder (cell);
	HcalCoderDb coder (*channelCoder, *shape);
	rec->push_back(reco_.reconstruct(*i,coder,calibrations));
	//(rec->back()).setFlags(0); // Not yet implemented for HcalCalibRecHit
      }
      // return result
      e.put(rec);     
    }
  } 

  delete myqual;
} // void HcalHitReconstructor::produce(...)
