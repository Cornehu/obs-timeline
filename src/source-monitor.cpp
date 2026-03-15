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

			// Get media duration and position (values in milliseconds)
			int64_t total   = obs_source_media_get_duration(source);
			if (total <= 0)
				return true; // no defined duration, skip

			int64_t elapsed = obs_source_media_get_time(source);

			SourceTimelineInfo info;
			info.name    = obs_source_get_name(source);
			info.total   = total;
			info.elapsed = elapsed;

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
