import FWCore.ParameterSet.Config as cms

hbheprereco = cms.EDProducer("HcalSimpleReconstructor",
    correctionPhaseNS = cms.double(13.0),
    digiLabel = cms.InputTag("hcalDigis"),
    samplesToAdd = cms.int32(4),
    Subdetector = cms.string('HBHE'),
    firstSample = cms.int32(4),
    correctForPhaseContainment = cms.bool(True),
    correctForTimeslew = cms.bool(True),
    dropZSmarkedPassed = cms.bool(True)
)


