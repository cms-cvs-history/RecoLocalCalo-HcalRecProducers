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
#include "RecoLocalCalo/HcalRecAlgos/interface/HcalIgnoreCellsAlgo.h"
#include "CondFormats/HcalObjects/interface/HcalChannelQuality.h"
#include "CondFormats/HcalObjects/interface/HcalChannelStatus.h"
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHEStatusBitSetter.h"

    /** \class HcalHitReconstructor
	
    $Date: 2008/11/28 20:28:42 $
    $Revision: 1.2 $
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
      HBHEStatusBitSetter* hbheFlagSetter_;
      HcalHFStatusBitFromRecHits* hfrechitbit_;
      HcalHFStatusBitFromDigis*   hfdigibit_;
      HcalIgnoreCellsAlgo*        ignoreCells_;
      DetId::Detector det_;
      int subdet_;
      int ignoreMask_;
      HcalOtherSubdetector subdetOther_;
      edm::InputTag inputLabel_;
      std::vector<std::string> channelStatusToDrop_;
    };

#endif
