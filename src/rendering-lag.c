#include <obs-module.h>
#include <obs.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <stdlib.h>
#include "plugin-macros.generated.h"


struct rendering_lag
{
	double rate;
	int sleep_time;
	uint64_t sleep_to;
};


static const char *rendering_lag_name(void *unused)
{
	UNUSED_PARAMETER(unused);

	return obs_module_text("Rendering Lag");
}

static obs_properties_t *rendering_lag_get_properties(void *unused)
{
	UNUSED_PARAMETER(unused);
	obs_properties_t *props;
	obs_property_t *prop;

	props = obs_properties_create();

	prop = obs_properties_add_float_slider(props, "rate", obs_module_text("Rate"), 0.0, 100.0, 1.0);
	obs_property_float_set_suffix(prop, "%");
	prop = obs_properties_add_int_slider(props, "sleep_time", obs_module_text("Sleep time"), 1, 100, 1);
	obs_property_int_set_suffix(prop, "ms");
	struct obs_video_info ovi;
	obs_get_video_info(&ovi);
	if (ovi.fps_den>0 && ovi.fps_num>0) {
		struct dstr desc;
		dstr_init(&desc);
		dstr_printf(&desc, obs_module_text("sleep_time.tooltip"), ovi.fps_den * 2.1e3 / ovi.fps_num);
		obs_property_set_long_description(prop, desc.array);
		dstr_free(&desc);
	}

	return props;
}

static void rendering_lag_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "rate", 5.0);
	obs_data_set_default_int(settings, "sleep_time", 70);
}

static void rendering_lag_destroy(void *data)
{
	struct rendering_lag *s = data;

	bfree(s);
}

static void rendering_lag_update(void *data, obs_data_t *settings)
{
	struct rendering_lag *s = data;

	s->rate = obs_data_get_double(settings, "rate") * 1e-2;
	s->sleep_time = obs_data_get_int(settings, "sleep_time");
}

static void *rendering_lag_create(obs_data_t *settings, obs_source_t *source)
{
	struct rendering_lag *s = bzalloc(sizeof(struct rendering_lag));

	rendering_lag_update(s, settings);

	return s;
}

static void rendering_lag_tick(void *data, float sec)
{
	struct rendering_lag *s = data;

	s->sleep_to = 0;
	if (rand() < s->rate * RAND_MAX)
		s->sleep_to = obs_get_video_frame_time() + 1000000LL * s->sleep_time;
}

static void rendering_lag_render(void *data, gs_effect_t *effect)
{
	struct rendering_lag *s = data;
	if (s->sleep_to) {
		os_sleepto_ns(s->sleep_to);
		s->sleep_to = 0;
	}
}

static uint32_t rendering_lag_width(void *data)
{
	UNUSED_PARAMETER(data);
	return 0;
}

struct obs_source_info rendering_lag_info = {
	.id = "rendering_lag",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
	.get_name = rendering_lag_name,
	.create = rendering_lag_create,
	.destroy = rendering_lag_destroy,
	.update = rendering_lag_update,
	.get_properties = rendering_lag_get_properties,
	.get_defaults = rendering_lag_get_defaults,
	.video_tick = rendering_lag_tick,
	.video_render = rendering_lag_render,
	.get_width = rendering_lag_width,
	.get_height = rendering_lag_width,
};
