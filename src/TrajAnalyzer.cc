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