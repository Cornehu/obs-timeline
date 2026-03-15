#include <obs-module.h>
#include <obs-frontend-api.h>
#include "timeline-dock.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-timeline-monitor", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "OBS Timeline Monitor — displays active source timelines in a dock panel";
}

static TimelineDock *dock = nullptr;

static void obs_event_callback(enum obs_frontend_event event, void *private_data)
{
	(void)private_data;
	if (!dock)
		return;

	switch (event) {
	case OBS_FRONTEND_EVENT_SCENE_CHANGED:
	case OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED:
		dock->refreshSources();
		break;
	case OBS_FRONTEND_EVENT_EXIT:
		dock->stopMonitoring();
		break;
	default:
		break;
	}
}

bool obs_module_load(void)
{
	dock = new TimelineDock();
	obs_frontend_add_dock_by_id("obs-timeline-monitor-dock",
				    obs_module_text("TimelineMonitor"),
				    dock);

	obs_frontend_add_event_callback(obs_event_callback, nullptr);

	blog(LOG_INFO, "[obs-timeline-monitor] Plugin loaded successfully");
	return true;
}

void obs_module_unload(void)
{
	obs_frontend_remove_event_callback(obs_event_callback, nullptr);
	blog(LOG_INFO, "[obs-timeline-monitor] Plugin unloaded");
}
