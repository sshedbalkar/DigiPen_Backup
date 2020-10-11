function MaterialMain( graphics )
	coroutine.yield();
	-------------------------------------------- renderer -------------------------------------------
	local sky_renderer = graphics.NewRenderer();
	--------------------------------------------------------------------------------- vertices for the skydome
	local vformat = 20;
	local n = 18;
	local vcount = ( 2 * n + 1 ) * ( n + 1 );
	local icount = 2 * n * n * 6;
	local data = pointer.Chunk();
	graphics.InitData( data, vformat * vcount );
	for i = 0, n * 2 do
		local s = math.pi * i / n;
		for j = 0, n do
			local t = math.pi * j / n;
			local x = math.sin( s ) * math.sin( t );
			local y = math.cos( t );
			local z = math.cos( s ) * math.sin( t );
			local A = 1.0 * j / n;
			A = A * A * 2.0;
			local u = A * math.sin( s ) + 0.5;
			local v = 0.5 - A * math.cos( s );
			data:SetF( x, y, z, u, v );
			data:Offset( data, vformat );
		end
	end
	data:Offset( data, - vcount * vformat );
	local vbuffer = graphics.NewVB( vcount * vformat, 0, 0, data );
	sky_renderer:VertexBuffer( vbuffer, vformat );
	--------------------------------------------------------------------------------- indices for the skydome
	graphics.InitData( data, icount * 4 );
	for i = 0, n * 2 - 1 do
		for j = 0, n - 1 do
			local first = i * ( n + 1 ) + j;
			local second = ( i + 1 ) * ( n + 1 ) + j;
			data:SetI( first, second, first + 1, second, second + 1, first + 1 );
			data:Offset( data, 6 * 4 );
		end
	end
	data:Offset( data, - icount * 4 );
	local ibuffer = graphics.NewIB( icount, data );
	sky_renderer:IndexBuffer( ibuffer );
	--------------------------------------------------------------------------------- constant buffer
	local cbuffer = graphics.NewCB( 32 * 4 );
	sky_renderer:ConstantBuffer( "VS", cbuffer );
	local pscbuffer = graphics.NewCB( 4 * 4 );
	sky_renderer:ConstantBuffer( "PS", pscbuffer );
	local speed_x = 0.002;
	local speed_y = 0.002;
	local translation_x = 0.0;
	local translation_y = 0.0;
	local noise_scale = 0.6;
	--------------------------------------------------------------------------------- effect
	sky_renderer:Effect( effects.sky );
	--------------------------------------------------------------------------------- image
	sky_renderer:ShaderResource( "PS", textures.perlin );
	sky_renderer:ShaderResource( "PS", textures.cloud );
	sky_renderer:Sampler( "PS", gu.samplers.LinearWrap );
	--------------------------------------------------------------------------------- loop
	while true do
		local mat_view, mat_proj = coroutine.yield();
		-------------------------------- build cbuffer -----------------------------
		graphics.MapBuffer( data, cbuffer );
		data:AppendArray( mat_view, 16 );
		data:AppendArray( mat_proj, 16 );
		graphics.UnmapBuffer();
		graphics.MapBuffer( data, pscbuffer );
		translation_x = translation_x + speed_x * blackboard.dt;
		translation_y = translation_y + speed_y * blackboard.dt;
		data:SetF( translation_x, translation_y, noise_scale, 1.0 );
		graphics.UnmapBuffer();
		----------------------------- draw -----------------------------------------
		--graphics.SetRasterState( gu.raster_states.Wireframe );
		graphics.SetViewportDepth( 1.0, 1.0 );
		sky_renderer:DrawIndexed( icount );
		--graphics.SetRasterState( gu.raster_states.CullBack );
		----------------------------- restore --------------------------------------
		graphics.SetViewportDepth( 0.0, 1.0 );
	end

end