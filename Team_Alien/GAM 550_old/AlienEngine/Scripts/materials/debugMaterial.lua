function MaterialMain( graphics )
	----------------------------------------------------------------------------- renderer
	local renderer = graphics.NewRenderer();
	----------------------------------------------------------------------------- material
	local mat = graphics.NewMaterial("debug");
	mat:SetSerializer("color(f3)type(i1)");
	----------------------------------------------------------------------------- effect
	renderer:Effect( effects.debug );
	----------------------------------------------------------------------------- cbuffer
	local frame_cb = graphics.NewCB(16 * 4);
	renderer:ConstantBuffer( "GS", frame_cb );
	----------------------------------------------------------------------------- vbuffer
	local max_objects = 400;
	local vformat_size = 24 * 4;
	local vbuffer_size = max_objects * vformat_size;
	local vbuffer = graphics.NewVB( vbuffer_size, 1, 0 );
	renderer:VertexBuffer( vbuffer, vformat_size );
	------------------------------------------------------------------------------ per object update function
	local data = pointer.Chunk(); 
	local trans = pointer.Transform();
	local pos = pointer.Chunk();
	local color = pointer.Chunk();
	local go = pointer.GfxObj();
	local matrix = array.New(16);
	local vcount = 0;

	local AppendVB = function()
		go:Transform( trans );
		trans:BuildMatrix( matrix );
		go:Data( color );
		data:AppendArray( matrix, 16 );
		data:AppendChunk( color, 3 * 4 );
		data:AppendChunk( color, 3 * 4 );
		color:Offset( color, 3 * 4 );
		data:AppendChunk( color, 4 );
		data:SetI( go:Flag() );
		data:Offset( data, 4 );
		vcount = vcount + 1;
	end
	--end initialization
	--loop

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
		
		graphics.MapBuffer( data, vbuffer );
		vcount = 0;
		mat:ForEachObject( go, AppendVB );
		graphics.UnmapBuffer();
		
		if ( wireframe ) then graphics.SetRasterState( gu.raster_states.Wireframe ) end;
		renderer:Draw( vcount );
		if ( wireframe ) then graphics.SetRasterState( gu.raster_states.CullBack ) end;
		
	end
end
