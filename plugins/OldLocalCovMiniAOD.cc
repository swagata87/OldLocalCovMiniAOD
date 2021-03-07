// Package:    LocalCov/OldLocalCovMiniAOD
// Original Author:  Swagata Mukherjee

#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "CondFormats/EcalObjects/interface/EcalPFRecHitThresholds.h"
#include "CondFormats/DataRecord/interface/EcalPFRecHitThresholdsRcd.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "TTree.h"

class OldLocalCovMiniAOD : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit OldLocalCovMiniAOD(const edm::ParameterSet&);
  ~OldLocalCovMiniAOD();
  edm::Service<TFileService> fs;
  TTree   *tree = fs->make<TTree>("EventTree", "EventData");
  std::vector<double>  eleSigmaIetaIetaOld;
  std::vector<double>  eleSigmaIetaIetaNew;
  std::vector<float>  elePt;

private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  edm::EDGetTokenT<edm::View<reco::GsfElectron> > eleToken_;
  edm::EDGetTokenT<EcalRecHitCollection> EBRecHitCollectionT_;
  edm::EDGetTokenT<EcalRecHitCollection> EERecHitCollectionT_;
  edm::ESHandle<CaloTopology> theCaloTopology;

};

OldLocalCovMiniAOD::OldLocalCovMiniAOD(const edm::ParameterSet& iConfig)
    : 
  eleToken_(consumes<edm::View<reco::GsfElectron> >(iConfig.getParameter<edm::InputTag>("electrons"))),
  EBRecHitCollectionT_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("EBRecHitCollection"))),
  EERecHitCollectionT_(consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("EERecHitCollection")))
{
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif

  tree->Branch("eleSigmaIetaIetaOld_",&eleSigmaIetaIetaOld);
  tree->Branch("eleSigmaIetaIetaNew_",&eleSigmaIetaIetaNew);
  tree->Branch("elePt_",&elePt);

}

OldLocalCovMiniAOD::~OldLocalCovMiniAOD() {
}

void OldLocalCovMiniAOD::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;

  //edm::ESHandle<EcalPFRecHitThresholds> pThresholds;
  //iSetup.get<EcalPFRecHitThresholdsRcd>().get(pThresholds);
  //const EcalPFRecHitThresholds* thresholds = pThresholds.product();
        
  eleSigmaIetaIetaOld.clear();
  eleSigmaIetaIetaNew.clear();
  elePt.clear();

  for(const auto& ele : iEvent.get(eleToken_) ) {

    if ( ele.pt() < 5.0 ) continue;

    const reco::SuperCluster& superClus = *ele.superCluster();
    const reco::CaloCluster &seedCluster = *superClus.seed();
    const bool iseb = seedCluster.hitsAndFractions()[0].first.subdetId() == EcalBarrel;
    edm::Handle<EcalRecHitCollection> EBRecHits_;
    edm::Handle<EcalRecHitCollection> EERecHits_;
    iEvent.getByToken(EBRecHitCollectionT_, EBRecHits_);
    iEvent.getByToken(EERecHitCollectionT_, EERecHits_);
    const EcalRecHitCollection *recHits = iseb ? EBRecHits_.product() : EERecHits_.product();
    
    iSetup.get<CaloTopologyRecord>().get(theCaloTopology);
    const CaloTopology* caloTopology = theCaloTopology.product();
    
    using ClusterTools = noZS::EcalClusterTools;     
    std::vector<float> oldLocalCov = ClusterTools::localCovariances(seedCluster,recHits,caloTopology) ;
    float sigmaIetaIetaOld = sqrt(oldLocalCov[0]);
    
    eleSigmaIetaIetaNew.push_back(ele.full5x5_sigmaIetaIeta());
    eleSigmaIetaIetaOld.push_back(sigmaIetaIetaOld);
    elePt.push_back(ele.pt());
    
  }

  tree->Fill();

}


DEFINE_FWK_MODULE(OldLocalCovMiniAOD);
