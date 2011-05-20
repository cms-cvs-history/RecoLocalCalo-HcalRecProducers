#ifndef HCALSIMPLERECONSTRUCTOR_H
#define HCALSIMPLERECONSTRUCTOR_H 1

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "CondFormats/HcalObjects/interface/HcalRecoParams.h"
#include "CondFormats/HcalObjects/interface/HcalRecoParam.h" 
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoLocalCalo/HcalRecAlgos/interface/HcalSimpleRecAlgo.h"


    /** \class HcalSimpleReconstructor
	
    $Date: 2011/02/25 10:29:33 $
    $Revision: 1.5 $
    \author J. Mans - Minnesota
    */
    class HcalSimpleReconstructor : public edm::EDProducer {
    public:
      explicit HcalSimpleReconstructor(const edm::ParameterSet& ps);
      virtual ~HcalSimpleReconstructor();
      virtual void produce(edm::Event& e, const edm::EventSetup& c);
      virtual void beginRun(edm::Run&r, edm::EventSetup const & es);
      virtual void endRun(edm::Run&r, edm::EventSetup const & es);
    private:      
      HcalSimpleRecAlgo reco_;
      DetId::Detector det_;
      int subdet_;
      HcalOtherSubdetector subdetOther_;
      edm::InputTag inputLabel_;

      bool dropZSmarkedPassed_; // turn on/off dropping of zero suppression marked and passed digis

      // legacy parameters for config-set values compatibility 
      // to be removed after 4_2_0...
      int firstSample_;
      int samplesToAdd_;
      bool tsFromDB_;

      HcalRecoParams* paramTS;  // firstSample & sampleToAdd from DB  

    };

#endif
