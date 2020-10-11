function MaterialMain( graphics )
----------------------### init ###-----------------
	----------------------------------------------------------------------------------------- renderer
	local sprite_renderer = graphics.NewRenderer();
	----------------------------------------------------------------------------------------- material
	local text0 = graphics.NewMaterial("guitext0");
	text0:SetSerializer("vbindex(i1)");
	----------------------------------------------------------------------------------------- effect
	sprite_renderer:Effect( effects.sprite );
	----------------------------------------------------------------------------------------- cbuffer
	local frame_cb = graphics.NewCB( 16 * 4 );
	local inst_cb = graphics.NewCB( 16 * 4 );
	sprite_renderer:ConstantBuffer( "GS", frame_cb );
	sprite_renderer:ConstantBuffer( "GS", inst_cb );
	----------------------------------------------------------------------------------------- font data
	local font_tnr, img_tnr = graphics.NewFont( "TimesNewRoman" );
	local font_vsize = 8 * 4;
	----------------------------------------------------------------------------------------- vbuffer
	local font_vb = graphics.NewVB( 80 * font_vsize, 1, 0 );
	sprite_renderer:VertexBuffer( font_vb, font_vsize );
	----------------------------------------------------------------------------------------- shader resources
	sprite_renderer:ShaderResource( "PS", img_tnr );
	----------------------------------------------------------------------------------------- states
	sprite_renderer:Sampler( "PS", gu.samplers.LinearClamp );
	----------------------------------------------------------------------------------------- local matrix
	local mat_world = array.New(16);
	----------------------------------------------------------------------------------------- pointers
	local data = pointer.Chunk();
	local pvb = pointer.Chunk();
	local text = pointer.Text();
	local transform = pointer.Transform();
	local str = pointer.Chunk();
	local go = pointer.GfxObj();
---------------#### init end ####-------------------
-------####### update functions #######----------------
	--------------------- text0 --------------------------
	local Text0Update = function()
		go:Text(text);
		go:Transform( transform );
		transform:BuildMatrix( mat_world );
		graphics.MapBuffer( data, inst_cb );
		data:AppendArray( mat_world, 16 );
		graphics.UnmapBuffer();

		text:Content( str );
		graphics.MapBuffer( data, font_vb );
		local len = graphics.FontBuildVBFromMemory( font_tnr, str, 24, data );
		graphics.UnmapBuffer();

		sprite_renderer:Draw( len );
	end
----------------## main loop ##--------------------
	while true do
		local matrix = coroutine.yield();

		graphics.SetDepthState( gu.depth_states.DepthDisable );
		graphics.SetBlendState( gu.blend_states.BlendAlpha );

		graphics.MapBuffer( data, frame_cb );
		data:AppendArray( matrix, 16 );
		graphics.UnmapBuffer();
		
		text0:ForEachObject( go, Text0Update );
		graphics.SetBlendState( gu.blend_states.BlendNone );
		graphics.SetDepthState( gu.depth_states.DepthEnable );
	end
end
