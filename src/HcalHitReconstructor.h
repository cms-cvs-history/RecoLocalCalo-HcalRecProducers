#ifndef HCALHITRECONSTRUCTOR_H 
#define HCALHITRECONSTRUCTOR_H 1

#include "FWCore/Framework/interface/EDProducer.h"
#include "DataFormats/Common/interface/EDProduct.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSimpleRecAlgo.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalHFStatusBitFromRecHits.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalHFStatusBitFromDigis.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalCaloFlagLabels.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/HcalObjects/interface/HcalChannelStatus.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHEStatusBitSetter.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalTimingCorrector.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHETimeProfileStatusBitSetter.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalADCSaturationFlag.h"

    /** \class HcalHitReconstructor
	
    $Date: 2009/03/27 14:47:13 $
    $Revision: 1.3 $
    \author J. Temple & E. Yazgan
    ** Based on HcalSimpleReconstructor.h by J. Mans
    */
    class HcalHitReconstructor : public edm::EDProducer {
    public:
      explicit HcalHitReconstructor(const edm::ParameterSet& ps);
      virtual ~HcalHitReconstructor();
      virtual void produce(edm::Event& e, const edm::EventSetup& c);
    private:      
      HcalSimpleRecAlgo reco_;
      HcalADCSaturationFlag* saturationFlagSetter_;
      HBHEStatusBitSetter* hbheFlagSetter_;
      HBHETimeProfileStatusBitSetter* hbheHSCPFlagSetter_;
      HcalHFStatusBitFromRecHits* hfrechitbit_;
      HcalHFStatusBitFromDigis*   hfdigibit_;
 
      DetId::Detector det_;
      int subdet_;
      HcalOtherSubdetector subdetOther_;
      edm::InputTag inputLabel_;
      //std::vector<std::string> channelStatusToDrop_;
      bool correctTiming_; // turn on/off Ken Rossato's algorithm to fix timing
      bool setNoiseFlags_; // turn on/off basic noise flags
      bool setHSCPFlags_;  // turn on/off HSCP noise flags
      bool setSaturationFlags_; // turn on/off flag indicating ADC saturation

      bool dropZSmarkedPassed_; // turn on/off dropping of zero suppression marked and passed digis
    };

#endif
