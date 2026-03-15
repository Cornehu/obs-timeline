#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct SourceTimelineInfo {
	std::string name;    // source display name
	int64_t     elapsed; // milliseconds elapsed
	int64_t     total;   // total duration in ms (0 = unknown)
};

class SourceMonitor {
public:
	// Returns timeline info for all active PGM sources that have a defined duration
	static std::vector<SourceTimelineInfo> getActiveTimelines();

private:
	static bool sourceHasTimeline(obs_source_t *source);
};
