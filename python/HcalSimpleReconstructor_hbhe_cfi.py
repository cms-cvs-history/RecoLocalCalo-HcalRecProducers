import FWCore.ParameterSet.Config as cms

hbheprereco = cms.EDProducer("HcalSimpleReconstructor",
    correctionPhaseNS = cms.double(13.0),
    digiLabel = cms.InputTag("hcalDigis"),
    Subdetector = cms.string('HBHE'),
    correctForPhaseContainment = cms.bool(True),
    correctForTimeslew = cms.bool(True),
    dropZSmarkedPassed = cms.bool(True)
)


