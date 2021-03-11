import FWCore.ParameterSet.Config as cms

process = cms.Process("LocalCov")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("Geometry.CaloEventSetup.CaloTopology_cfi");
process.load("Geometry.CaloEventSetup.CaloGeometry_cfi");
process.load("Geometry.CMSCommonData.cmsIdealGeometryXML_cfi");
process.load("Configuration.Geometry.GeometryECALHCAL_cff")

from CondCore.ESSources.GlobalTag import GlobalTag

process.MessageLogger.cerr.FwkReport.reportEvery = 100

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase1_2021_realistic', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '113X_mcRun3_2021_realistic_v2', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring(
#                                    '/store/mc/Run3Summer19MiniAOD/DYToEE_M-50_NNPDF31_TuneCP5_14TeV-powheg-pythia8/MINIAODSIM/2021Scenario_106X_mcRun3_2021_realistic_v3-v2/270000/087644E5-95DB-E24C-B7A9-81646685DBA9.root'
                                    '/store/mc/Run3Winter20DRMiniAOD/ZprimeToEE_M-6000_TuneCP5_14TeV-pythia8/MINIAODSIM/FlatPU0to80_110X_mcRun3_2021_realistic_v6-v3/230000/21158CF3-E023-D647-8473-9C57F754719E.root'
                                )
                            )

process.LocalCov = cms.EDAnalyzer('OldLocalCovMiniAOD',
                                  electrons = cms.InputTag('slimmedElectrons'),
                                  EERecHitCollection = cms.InputTag("reducedEgamma","reducedEERecHits",""), #miniAOD
                                  EBRecHitCollection = cms.InputTag("reducedEgamma","reducedEBRecHits",""), #miniAOD
                                  )

process.TFileService = cms.Service("TFileService", fileName = cms.string('testLocalCov.root'))

process.p = cms.Path(process.LocalCov)

