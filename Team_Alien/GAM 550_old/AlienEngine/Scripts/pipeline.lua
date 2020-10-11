------------------------#### script body ####------------------------------
function Body( object )
	--coroutine.yield();
------------------------## initialization start ##-------------------------
	local graphics = Graphics;								-- get a local reference to global library 'Graphics'
	dofile( "scripts/graphicsUtility.lua" );
	gu.Setup( graphics );
	dofile( "scripts/assets.lua" );
	LoadAssets( graphics );
	--------------------------- pointers ----------------------------------
	local maincamera = pointer.Camera();
	object:Camera( maincamera );
	----------------------------- matrices --------------------------------
	local mat_ortho = array.New(16);
	local mat_viewproj = array.New(16);
	local mat_view = array.New(16);
	local mat_proj = array.New(16);
	local mat_viewproj_inv = array.New(16);
	gu.BuildBasicMatrices( mat_ortho, maincamera );
	---------------------------------------------------------------------------------- rendertargets 
	--------------------------------------------------------- for mesh rendering
	local diffusertv, diffusesrv = graphics.NewRTRelative( 1.0, 'f4' );
	local normalrtv, normalsrv = graphics.NewRTRelative( 1.0, 'f4' );
	local flagrtv, flagsrv = graphics.NewRTRelative( 1.0, 'u1' );
	--------------------------------------------------------- for deferred lighting	
	local deferredrtv, deferredsrv = graphics.NewRTRelative( 1.0, 'f4' );
	--------------------------------------------------------- for post processing
	------------------------------------------- luminance adaptation
	local lumartv, lumasrv = graphics.NewRT( 1024, 1024, 'f1', 1, 0 );
	local currentlumartv = {};
	local currentlumasrv = {};
	local currentlumaindex = 0;
	currentlumartv[0], currentlumasrv[0] = graphics.NewRT( 1, 1, 'f1' );
	currentlumartv[1], currentlumasrv[1] = graphics.NewRT( 1, 1, 'f1' );
	------------------------------------------- for blooming
	local highlightrtv, highlightsrv = graphics.NewRTRelative( 0.5, 'f4' );
	local bloom0rtv, bloom0srv = graphics.NewRTRelative( 0.1 );
	local bloom1rtv, bloom1srv = graphics.NewRTRelative( 0.1 );
	------------------------------------------- postprocess output
	local tonemappedrtv, tonemappedsrv = graphics.NewRTRelative( 1.0, 'f4' );
	---------------------------------------------------------------------------------- depth stencil buffers
	local shadowdsv, shadowsrv = graphics.NewDS( 2048, 2048 );
	local scenedsv, scenedepthsrv = graphics.NewDS( 1.0 );
	---------------------------------------------------------------------------------- threads
	dofile("scripts/materials/debug2d.lua");
	local thread_debug2d = gu.CreateThread( MaterialMain, graphics );
	dofile("scripts/materials/DebugMaterial.lua");
	local thread_debug = gu.CreateThread( MaterialMain, graphics );
	dofile("scripts/materials/heightmappedMesh.lua");
	local thread_terrain = gu.CreateThread( MaterialMain, graphics );
	dofile("scripts/materials/gui.lua");
	local thread_gui = gu.CreateThread( MaterialMain, graphics );
	dofile("scripts/materials/light.lua");
	local thread_light, cblight = gu.CreateThread( MaterialMain, graphics );
	dofile("scripts/materials/sky.lua");
	local thread_sky = gu.CreateThread( MaterialMain, graphics );
	---------------------- model materials -------------------------
	dofile("scripts/materials/Model.lua");
	local models = {};
	for i,id in pairs( meshes ) do
		gu.AddModel( MaterialMain, graphics, id, models );
	end
	dofile("scripts/materials/testModel.lua");
	gu.AddModel( MaterialMain, graphics, "", models );
	--------------- variables for screen space process --------------------
	local ssp_cb = graphics.NewCB( 16 * 4 );
	local matrixshadow_cb = graphics.NewCB( 64 );
	local ssp_vb = graphics.NewVB( 8, 1, 0 );
	local mat_inv_cb = graphics.NewCB( 64 );
	local ssp_data = pointer.Chunk();
	---------------------------------------------------------------------------------- deferred renderer
	local deferred_renderer = graphics.NewRenderer();
	deferred_renderer:Effect( effects.deferred );
	deferred_renderer:VertexBuffer( ssp_vb, 8 );
	deferred_renderer:ConstantBuffer( "GS", ssp_cb );
	deferred_renderer:ConstantBuffer( "PS", mat_inv_cb );
	deferred_renderer:ConstantBuffer( "PS", cblight );
	deferred_renderer:ConstantBuffer( "PS", matrixshadow_cb );
	deferred_renderer:ShaderResource( "PS", diffusesrv );
	deferred_renderer:ShaderResource( "PS", normalsrv );
	deferred_renderer:ShaderResource( "PS", flagsrv );
	deferred_renderer:ShaderResource( "PS", scenedepthsrv );
	deferred_renderer:ShaderResource( "PS", shadowsrv );
	deferred_renderer:ShaderResource( "PS", textures.shade );
	deferred_renderer:Sampler( "PS", gu.samplers.PointClamp );
	deferred_renderer:Sampler( "PS", gu.samplers.LinearWrap );
	deferred_renderer:Sampler( "PS", gu.samplers.ShadowMapPCF );
	---------------------------------------------------------------------------------- luma renderer
	local luma_renderer = graphics.NewRenderer();
	luma_renderer:Effect( effects.luma );
	luma_renderer:VertexBuffer( ssp_vb, 8 );
	luma_renderer:ConstantBuffer( "GS", ssp_cb );
	luma_renderer:ShaderResource( "PS", deferredsrv );
	luma_renderer:Sampler( "PS", gu.samplers.PointClamp );
	---------------------------------------------------------------------------------- highlight pick renderer
	local highlight_renderer = graphics.NewRenderer();
	highlight_renderer:Effect( effects.highlight );
	highlight_renderer:VertexBuffer( ssp_vb, 8 );
	highlight_renderer:ConstantBuffer( "GS", ssp_cb );
	highlight_renderer:ShaderResource( "PS", deferredsrv );
	highlight_renderer:ShaderResource( "PS", lumasrv );
	highlight_renderer:Sampler( "PS", gu.samplers.PointClamp );
	---------------------------------------------------------------------------------- bloom renderer
	local bloom_cb = graphics.NewCB( 16 );
	local bloom_renderer = graphics.NewRenderer();
	bloom_renderer:Effect( effects.bloom );
	bloom_renderer:VertexBuffer( ssp_vb, 8 );
	bloom_renderer:ConstantBuffer( "GS", ssp_cb );
	bloom_renderer:ConstantBuffer( "PS", bloom_cb );
	bloom_renderer:ShaderResource( "PS", highlightsrv );
	bloom_renderer:Sampler( "PS", gu.samplers.PointClamp );
	---------------------------------------------------------------------------------- luma adaptation renderer
	local adaptation_renderer = graphics.NewRenderer();
	adaptation_renderer:Effect( effects.adaptation );
	adaptation_renderer:VertexBuffer( ssp_vb, 8 );
	adaptation_renderer:ConstantBuffer( "GS", ssp_cb );
--	adaptation_renderer:ConstantBuffer( "PS", bloom_cb );
	adaptation_renderer:ShaderResource( "PS", lumasrv );
	adaptation_renderer:ShaderResource( "PS", currentlumasrv[1] );
	adaptation_renderer:Sampler( "PS", gu.samplers.PointClamp );
	---------------------------------------------------------------------------------- tone map renderer
	local tonemap_renderer = graphics.NewRenderer();
	tonemap_renderer:Effect( effects.tonemap );
	tonemap_renderer:VertexBuffer( ssp_vb, 8 );
	tonemap_renderer:ConstantBuffer( "GS", ssp_cb );
	tonemap_renderer:ShaderResource( "PS", deferredsrv );
	tonemap_renderer:ShaderResource( "PS", currentlumasrv[0] );
	tonemap_renderer:ShaderResource( "PS", bloom1srv );
	tonemap_renderer:ShaderResource( "PS", scenedepthsrv );
	tonemap_renderer:ShaderResource( "PS", flagsrv );
	tonemap_renderer:Sampler( "PS", gu.samplers.LinearClamp );
	---------------------------------------------------------------------------------- fxaa renderer
	local fxaa_renderer = graphics.NewRenderer();
	fxaa_renderer:Effect( effects.fxaa );
	fxaa_renderer:VertexBuffer( ssp_vb, 8 );
	fxaa_renderer:ConstantBuffer( "GS", ssp_cb );
	fxaa_renderer:ShaderResource( "PS", tonemappedsrv );
	fxaa_renderer:Sampler( "PS", gu.samplers.LinearClamp );
	--------------------------- clear color -----------------------------
	local clear_diffuse = array.New(4);
	local clear_normal = array.New(4);
	local clear_flag = array.New(4);
	clear_diffuse:Set( 0.5, 0.5, 0.9, 1.0 );
	clear_normal:Set( 0.0, 0.0, 0.0, 0.0 ); 
	clear_flag:Set( 0.0, 0.0, 0.0, 0.0 );
	--------------------------- profiling lines -----------------------------
	local profile0 = engine.DebugText( 0.01, 0.8 );
	local profile1 = engine.DebugText( 0.01, 0.75 );
	local profile2 = engine.DebugText( 0.01, 0.7 );
	local profile3 = engine.DebugText( 0.01, 0.65 );

	--------------------------- msg processing -----------------------------
	local MsgProc = function( msg )
		if ( msg.id == "resolution" ) then
			blackboard.screenw = msg.parama;
			blackboard.screenh = msg.paramb;
			gu.BuildBasicMatrices( mat_ortho, maincamera );
		end
	end
	SubscribeToMsg( MsgProc );
----------------------## initialization end ##----------------------------
--[[
	local testdata = pointer.Chunk();
	graphics.InitData( testdata, 12 );
	testdata:SetF( 0.0, 0.0, 0.0 );
	local testvb = graphics.NewVB( 12, 0, 0, testdata );
	local testso = graphics.NewVB( 1200, 0, 1 );
	local tester = graphics.NewRenderer();
	local renderer = graphics.NewRenderer();
	tester:Effect( effects.testso );
	tester:VertexBuffer( testvb, 12 );
	graphics.SOSetTarget( testso );
	tester:Draw( 1 );
	graphics.SOSetTarget();
	local testcb = graphics.NewCB( 64 );
	renderer:Effect( effects.testculling );
	renderer:VertexBuffer( testso, 12 );
	renderer:ConstantBuffer( "GS", testcb );
	]]--


------------------------### main loop ###----------------------------------
	graphics.SetBlendState( gu.blend_states.BlendNone );
	while true do
		coroutine.yield();
		graphics.ProfileStart( "graphics" );
		if ( blackboard.focused == 1 ) then
			---------------------- update viewing matrix ----------------------
			maincamera:Matrices( mat_view, mat_proj, mat_viewproj );
			--------- set up states(enable depth test/rasterizer cull back ) ------------
			graphics.SetDepthState( gu.depth_states.DepthEnable );
			graphics.SetRasterState( gu.raster_states.CullBack );
			--------------------------------------------------------------------------- render shadow maps
			graphics.ProfileStart( "depths" );
			local matrix_shadow = gu.UpdateThread( thread_light );
			graphics.ClearDSV( shadowdsv, 1.0 );
			graphics.SetDSV( shadowdsv );
			graphics.SetRTVNULL( 2048, 2048 );
			gu.UpdateThreadList( models, matrix_shadow, gu.render_depth );
			--------------------------------------------------------------------------- a cheap depth only pass to create a dsv for later culling
			graphics.ClearDSV( scenedsv, 1.0 );
			graphics.SetDSV( scenedsv );
			graphics.SetRTVNULL( blackboard.screenw, blackboard.screenh );
			gu.UpdateThreadList( models, mat_viewproj, gu.render_depth );
			graphics.ProfileEnd( "depths", profile1, 20 );
			--------------------------------------------------------------------------- setup for mesh rendering
			graphics.ProfileStart( "meshes" );
			graphics.SetDepthState( gu.depth_states.DepthEnable );
			graphics.SetDSV( scenedsv );
			graphics.ClearRTV( diffusertv, clear_diffuse );
			graphics.ClearRTV( normalrtv, clear_normal );
			graphics.ClearRTV( flagrtv, clear_flag );
			graphics.SetRTV( diffusertv, normalrtv, flagrtv );
			---------------------------------------------------------------------------- mesh rendering 
			gu.UpdateThreadList( models, mat_viewproj, gu.render_diffuse );
			---------------------------------------------------------------------------- turn on depth and render the rest meshes
			graphics.SetDepthState( gu.depth_states.DepthEnable );
			---------------------------------------------- terrain
			gu.UpdateThread( thread_terrain, mat_viewproj );
			---------------------------------------------- skybox
			gu.UpdateThread( thread_sky, mat_view, mat_proj );
			---------------------------------------------- debug
			gu.UpdateThread( thread_debug, mat_viewproj );
			graphics.ProfileEnd( "meshes", profile2, 20 );
			---------------------------------------------------------------------------- turn off depth and prepare for deferred pass
			graphics.SetDepthState( gu.depth_states.DepthDisable );
			graphics.SetDSV();
			--------------------------------------------------------------------------- setup for screen space processes
			graphics.ProfileStart( "postp" );
			---------------------------------------------- update lights 
			gu.UpdateThread( thread_light, maincamera );
			---------------------------------------------- screen space constants
			graphics.MapBuffer( ssp_data, ssp_cb );
			ssp_data:AppendArray( mat_ortho, 16 );
			graphics.UnmapBuffer();
			---------------------------------------------- screen space quad
			graphics.MapBuffer( ssp_data, ssp_vb );
			ssp_data:SetF( blackboard.screenw, blackboard.screenh );
			graphics.UnmapBuffer();
			---------------------------------------------- inverse matrix constant buffer
			mmath.Mat4Inverse( mat_viewproj_inv, mat_viewproj );
			graphics.MapBuffer( ssp_data, mat_inv_cb );
			ssp_data:AppendArray( mat_viewproj_inv, 16 );
			graphics.UnmapBuffer();
			---------------------------------------------- shadow matrix constant buffer
			graphics.MapBuffer( ssp_data, matrixshadow_cb );
			ssp_data:AppendArray( matrix_shadow, 16 );
			graphics.UnmapBuffer();
			---------------------------------------------------------------------------- deferred pass
			graphics.SetRTV( deferredrtv );
			deferred_renderer:Draw( 1 );
			---------------------------------------------------------------------------- calculate average luminance
			graphics.SetRTV( lumartv );
			luma_renderer:Draw( 1 );
			graphics.GenerateMips( lumasrv );
			---------------------------------------------------------------------------- highlight pick and bloom pass
			graphics.SetRTV( highlightrtv );
			highlight_renderer:Draw( 1 );
			graphics.SetRTV( bloom0rtv );
			graphics.MapBuffer( ssp_data, bloom_cb );
			ssp_data:SetF( 10.0, 0.0, 64, blackboard.dt );
			graphics.UnmapBuffer();
			bloom_renderer:ShaderResource( "PS", highlightsrv, 0 );
			bloom_renderer:Draw( 1 );
			graphics.SetRTV( bloom1rtv );
			graphics.MapBuffer( ssp_data, bloom_cb );
			ssp_data:SetF( 0.0, 10.0, 64, blackboard.dt );
			graphics.UnmapBuffer();
			bloom_renderer:ShaderResource( "PS", bloom0srv, 0 );
			bloom_renderer:Draw( 1 );
			---------------------------------------------------------------------------- luminance adaptation
			graphics.SetRTV( currentlumartv[currentlumaindex] );
			adaptation_renderer:ShaderResource( "PS", currentlumasrv[1-currentlumaindex], 1 );
			adaptation_renderer:Draw( 1 );
			currentlumaindex = 1 - currentlumaindex;
			---------------------------------------------------------------------------- tonemapping pass
			graphics.SetRTV( tonemappedrtv );
			tonemap_renderer:ShaderResource( "PS", currentlumasrv[currentlumaindex], 1 );
			tonemap_renderer:Draw( 1 );
			---------------------------------------------------------------------------- aa 
			graphics.SetRTVScreen();
			fxaa_renderer:Draw( 1 );
			graphics.ProfileEnd( "postp", profile3, 20 );
			----------------------------------------------------------------------------- present and end frame
			gu.UpdateThread( thread_debug2d, mat_viewproj, mat_ortho );
			gu.UpdateThread( thread_gui, mat_ortho );
			graphics.Present(0);
		end
		
		graphics.ProfileEnd( "graphics", profile0, 20 );
	end
end
