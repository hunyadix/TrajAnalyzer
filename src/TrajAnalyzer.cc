#include "../interface/TrajAnalyzer.h"

int TrajAnalyzer::trajectoryHasPixelHit(const edm::Ref<std::vector<Trajectory>>& trajectory)
{
	bool is_barrel_pixel_track = false;
	bool is_endcap_pixel_track = false;
	// Looping on the full track to check if we have pixel hits 
	// and to count the number of strip hits 
	for(auto& measurement: trajectory -> measurements())
	{
		// Check measurement validity
		if(!measurement.updatedState().isValid()) continue;
		auto hit = measurement.recHit();
		// Check hit quality
		if(!hit -> isValid()) continue;
		DetId det_id = hit -> geographicalId();
		uint32_t subdetid = (det_id.subdetId());
		// For saving the pixel hits
		if(subdetid == PixelSubdetector::PixelBarrel) is_barrel_pixel_track = true;
		if(subdetid == PixelSubdetector::PixelEndcap) is_endcap_pixel_track = true;
	}
	if(!is_barrel_pixel_track && !is_endcap_pixel_track)
	{
		return 0;
	}
	return 1;
}

int TrajAnalyzer::subdetidIsOnPixel(uint32_t subdetid)
{
	bool isPixelHit = false;
	isPixelHit |= subdetid == PixelSubdetector::PixelBarrel;
	isPixelHit |= subdetid == PixelSubdetector::PixelEndcap;
	return isPixelHit;
}

reco::VertexCollection::const_iterator TrajAnalyzer::findClosestVertexToTrack(const reco::TrackRef& track, const edm::Handle<reco::VertexCollection>& vertexCollectionHandle)
{
	reco::VertexCollection::const_iterator closestVtx = vertexCollectionHandle -> end();
	// FIXME: This is awkward, change the minDistance to the distance of the first valid vertex
	double minDistance = 9999;
	for(reco::VertexCollection::const_iterator it = vertexCollectionHandle -> begin() ; it != vertexCollectionHandle -> end() ; ++it)
	{
		// Filter out invalid vertices
		if(!it -> isValid()) continue;
		double trkVtxD0 = track -> dxy(it -> position()) * -1.0;
		double trkVtxDz = track -> dz (it -> position());
		// Comparing squareroots should be quick enough, if required, change this to a comparison of squares
		double trkVtxDB = sqrt(trkVtxD0 * trkVtxD0 + trkVtxDz * trkVtxDz);
		if(trkVtxDB < minDistance)
		{
			minDistance = trkVtxDB;
			closestVtx=it;
		}
	}
	return closestVtx;
}