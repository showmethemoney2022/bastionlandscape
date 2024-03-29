scenetest00 =
{
	name = "SceneTest00",
	render_targets =
	{
		additionals =
		{
			{
				name = "rt_reflection1",
				format = "D3DFMT_A8R8G8B8",
			},
			{
				name = "rt_reflection2",
				format = "D3DFMT_A8R8G8B8",
			},
			{
				name = "rt_reflection3",
				format = "D3DFMT_A8R8G8B8",
			},
			{
				name = "rt_reflection4",
				format = "D3DFMT_A8R8G8B8",
			},
			{
				name = "ui",
				format = "D3DFMT_A8R8G8B8",
			},
		},
	},
	materials =
	{
		"data/materials/MaterialLib00.lua",
	},
	cameras =
	{
		"data/scenes/CameraLib00.lua",
	},
	water_config = "data/scenes/WaterSceneTest00.lua",
	octree =
	{
		leaf_size = 2000.0,
		depth = 4,
		position = { 0.0, 0.0, 0.0 },
	},
	hierarchy =
	{
		{
			class = "landscape",
			name = "ground00",
			material = "terrain05",
			vertex_format = "default",
			grid_size = 16,
			grid_chunk_size = 16,
			position = { 0.0, 0.0, 0.0 },
			pixel_error_max = 2.5,
			floor_scale = 100.0,
			height_scale = 10.0,
			heightmap = "data/landscapes/land02.tga",
			layers_config = "data/landscapes/layers00.lua",
			target_stage = "scene",
		},
		{
			class = "sky",
			name = "sky00",
			material = "sky",
			radius = { 20000.0, 20000.0, 20000.0 },
			bottom_hemisphere = true,
			top_hemisphere = true,
			view_from_inside = true,
			position = { 0.0, 0.0, 0.0 },
			rotation = { 0.0, 0.0, 0.0 },
			horiz_slices = 50,
			vert_slices = 50,
			color = { 26.0 / 255.0, 103.0 / 255.0, 149.0 / 255.0, 1.0 },
			daytime = 12.0 * 60.0,
			target_stage = "scene",
			always_visible = true,
			in_octree = false,
		},
		{
			class = "lines",
			name = "debuglines",
			material = "geomhelper",
			position = { 0.0, 0.0, 0.0 },
			rotation = { 0.0, 0.0, 0.0 },
			target_stage = "scene",
		},
		{
			class = "sphere",
			name = "picking00",
			material = "geomhelper",
			radius = { 50.0, 25.0, 50.0 },
			bottom_hemisphere = true,
			top_hemisphere = true,
			view_from_inside = false,
			position = { 0.0, 0.0, 0.0 },
			rotation = { 0.0, 0.0, 0.0 },
			horiz_slices = 5,
			vert_slices = 20,
			color = { 26.0 / 255.0, 103.0 / 255.0, 149.0 / 255.0, 1.0 },
			target_stage = "scene",
		},
		{
			class = "sphere",
			name = "dummy",
			material = "geomhelper",
			radius = { 50.0, 100.0, 50.0 },
			bottom_hemisphere = false,
			top_hemisphere = true,
			view_from_inside = false,
			position = { 0.0, 0.0, 0.0 },
			rotation = { 0.0, 0.0, 0.0 },
			horiz_slices = 5,
			vert_slices = 20,
			color = { 220.0 / 255.0, 122.0 / 255.0, 149.0 / 14.0, 1.0 },
			target_stage = "scene",
		},
	},
	render_stages =
	{
		{
			name = "scene",
			camera = "scenecamera00",
			normal_processes =
			{
				{
					name = "reflection",
					viewport = "default",
					clear = true,
					render_targets =
					{
						{
							type = "tex2d",
							name = "rt_reflection1",
							index = 0,
						},
					},
				},
				{
					name = "reflection2",
					viewport = "default",
					clear = true,
					render_targets =
					{
						{
							type = "tex2d",
							name = "rt_reflection2",
							index = 0,
						},
					},
				},
				{
					name = "base",
					viewport = "default",
					clear = true,
					render_targets =
					{
						{
							type = "gbuffer",
							index = 0,
						},
						{
							type = "gbuffer",
							index = 1,
						},
						{
							type = "gbuffer",
							index = 2,
						},
					},
				},
			},
			post_processes =
			{
				{
					name = "water",
					material = "waterpost02",
					immediate_write = true,
				},
				-- {
					-- name = "monochrome",
					-- material = "monochromepost00",
				-- },
				-- {
					-- name = "inverse",
					-- material = "inversepost00",
				-- },
				-- {
					-- name = "basicblur",
					-- material = "basicblurpost00",
				-- },
				-- {
					-- name = "dofcombine",
					-- material = "dofcombinepost00",
				-- },
			},
		},
		{
			name = "ui",
			camera = "uicameraortho00",
			normal_processes =
			{
				{
					name = "ui",
					viewport = "default",
					clear = false,
				},
			},
		},
	},
}
