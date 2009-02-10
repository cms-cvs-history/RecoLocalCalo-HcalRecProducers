#include "FWCore/PluginManager/interface/ModuleDef.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "HcalSimpleReconstructor.h"
#include "HcalHitReconstructor.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"


DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(HcalSimpleReconstructor);
DEFINE_ANOTHER_FWK_MODULE(HcalHitReconstructor);
