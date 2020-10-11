function MaterialMain( graphics )
	--### init ###---
	---------------------------------------------------------------------------------------- renderer
	local renderer = graphics.NewRenderer();
	---------------------------------------------------------------------------------------- material
	--local mat = graphics.NewMaterial("grid");
	---------------------------------------------------------------------------------------- effect
	renderer:Effect( effects.heightmappedmesh );
	---------------------------------------------------------------------------------------- cbuffer
	local frame_cb = graphics.NewCB(16 * 4);
	renderer:ConstantBuffer( "VS", frame_cb );
	local size_cb = graphics.NewCB( 4 * 4 );
	renderer:ConstantBuffer( "VS", size_cb );
	---------------------------------------------------------------------------------------- vbuffer
	local data = pointer.Chunk();
	local vpos = array.New(2);
	local rows = 100;
	local cols = 100; 
	local area_scale = 0.2;
	local height_scale = 10;
	graphics.MapBuffer( data, size_cb );
	data:SetF( rows, cols, height_scale, area_scale );
	graphics.UnmapBuffer();
	local vcount = rows * cols * 6;
	local vformat_size = 2 * 4;
	local vbuffer_size = vcount * vformat_size;
	graphics.InitData( data, vbuffer_size );
	for i = 0, rows - 1 do
		for j = 0, cols - 1 do
			vpos:Set( i - rows / 2, j - cols / 2 );
			data:AppendArray( vpos, 2 );
			vpos:Set( i - rows / 2, j - cols / 2 + 1 );
			data:AppendArray( vpos, 2 );
			vpos:Set( i - rows / 2 + 1, j - cols / 2 );
			data:AppendArray( vpos, 2 );
			vpos:Set( i - rows / 2, j - cols / 2 + 1 );
			data:AppendArray( vpos, 2 );
			vpos:Set( i - rows / 2 + 1, j - cols / 2 + 1);
			data:AppendArray( vpos, 2 );
			vpos:Set( i - rows / 2 + 1, j - cols / 2 );
			data:AppendArray( vpos, 2 );
		end
	end
	data:Offset( data, -vbuffer_size );
	local vbuffer = graphics.NewVB( vbuffer_size, 0, 0, data );
	renderer:VertexBuffer( vbuffer, vformat_size );
	renderer:ShaderResource( "VS", textures.perlin );
	renderer:Sampler( "VS", gu.samplers.LinearClamp );
	--#### init end ####--

	local wireframe = true;
	local MsgProc = function( msg )
		if ( msg.id == "vkup" and msg.parama == C.VK_W ) then
			wireframe = not wireframe;
		end
	end
	SubscribeToMsg( MsgProc );

	while true do
		local matrix = coroutine.yield();
		graphics.MapBuffer( data, frame_cb );
		data:AppendArray( matrix, 16 );
		graphics.UnmapBuffer();
		
		if ( wireframe ) then graphics.SetRasterState( gu.raster_states.Wireframe ) end;
		renderer:Draw( vcount );
		if ( wireframe ) then graphics.SetRasterState( gu.raster_states.CullBack ) end;
	end
end
