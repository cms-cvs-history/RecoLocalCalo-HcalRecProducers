#ifndef ZDCHITRECONSTRUCTOR_H 
#define ZDCHITRECONSTRUCTOR_H 1

#include "FWCore/Framework/interface/EDProducer.h"
#include "DataFormats/Common/interface/EDProduct.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/ZdcSimpleRecAlgo.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalHFStatusBitFromRecHits.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalHFStatusBitFromDigis.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalCaloFlagLabels.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/HcalObjects/interface/HcalChannelStatus.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHEStatusBitSetter.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalTimingCorrector.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHETimeProfileStatusBitSetter.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHETimingShapedFlag.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalADCSaturationFlag.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HFTimingTrustFlag.h"

    /** \class ZdcHitReconstructor
	
    $Date: 2010/01/18 00:00:66 $
    $Revision: 1.6 $
    \author E. Garcia - CSU
    ** Based on HcalSimpleReconstructor.h by J. Mans
    */
    class ZdcHitReconstructor : public edm::EDProducer {
    public:
      explicit ZdcHitReconstructor(const edm::ParameterSet& ps);
      virtual ~ZdcHitReconstructor();
      virtual void produce(edm::Event& e, const edm::EventSetup& c);
    private:      
      ZdcSimpleRecAlgo reco_;
      HcalADCSaturationFlag* saturationFlagSetter_;
      HFTimingTrustFlag* HFTimingTrustFlagSetter_;
      HBHEStatusBitSetter* hbheFlagSetter_;
      HBHETimeProfileStatusBitSetter* hbheHSCPFlagSetter_;
      HBHETimingShapedFlagSetter* hbheTimingShapedFlagSetter_;
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
      bool setTimingTrustFlags_; // turn on/off HF timing uncertainty flag 

      bool dropZSmarkedPassed_; // turn on/off dropping of zero suppression marked and passed digis
    };

#endif
