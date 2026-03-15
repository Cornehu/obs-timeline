#include "source-monitor.hpp"
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <obs.h>

// Source types that expose a timeline (duration + position)
static const char *TIMELINE_SOURCE_TYPES[] = {
	"ffmpeg_source",  // Media Source (built-in)
	"vlc_source",     // VLC Source
	nullptr
};

bool SourceMonitor::sourceHasTimeline(obs_source_t *source)
{
	if (!source)
		return false;

	const char *id = obs_source_get_unversioned_id(source);
	if (!id)
		return false;

	for (int i = 0; TIMELINE_SOURCE_TYPES[i] != nullptr; ++i) {
		if (strcmp(id, TIMELINE_SOURCE_TYPES[i]) == 0)
			return true;
	}
	return false;
}

struct EnumContext {
	std::vector<SourceTimelineInfo> results;
};

static void enum_scene_items(obs_scene_t *scene, EnumContext *ctx)
{
	obs_scene_enum_items(
		scene,
		[](obs_scene_t *, obs_sceneitem_t *item, void *param) -> bool {
			auto *ctx = static_cast<EnumContext *>(param);

			if (!obs_sceneitem_visible(item))
				return true; // skip invisible items

			obs_source_t *source = obs_sceneitem_get_source(item);
			if (!source)
				return true;

			// Recurse into nested scenes/groups
			if (obs_source_get_type(source) == OBS_SOURCE_TYPE_SCENE) {
				obs_scene_t *nested = obs_scene_from_source(source);
				if (nested)
					enum_scene_items(nested, ctx);
				return true;
			}

			if (!SourceMonitor::sourceHasTimeline(source))
				return true;

			// Get media duration and position via proc_handler
			proc_handler_t *ph = obs_source_get_proc_handler(source);
			if (!ph)
				return true;

			calldata_t cd = {};

			// get_duration
			calldata_init(&cd);
			proc_handler_call(ph, "get_duration", &cd);
			int64_t total = calldata_int(&cd, "duration");
			calldata_free(&cd);

			if (total <= 0)
				return true; // no defined duration, skip

			// get_time (current position)
			calldata_init(&cd);
			proc_handler_call(ph, "get_time", &cd);
			int64_t elapsed = calldata_int(&cd, "time");
			calldata_free(&cd);

			// OBS returns nanoseconds — convert to ms
			SourceTimelineInfo info;
			info.name    = obs_source_get_name(source);
			info.total   = total   / 1000000LL;
			info.elapsed = elapsed / 1000000LL;

			ctx->results.push_back(info);
			return true;
		},
		ctx);
}

std::vector<SourceTimelineInfo> SourceMonitor::getActiveTimelines()
{
	EnumContext ctx;

	obs_source_t *pgm = obs_frontend_get_current_scene();
	if (!pgm)
		return ctx.results;

	obs_scene_t *scene = obs_scene_from_source(pgm);
	if (scene)
		enum_scene_items(scene, &ctx);

	obs_source_release(pgm);
	return ctx.results;
}
