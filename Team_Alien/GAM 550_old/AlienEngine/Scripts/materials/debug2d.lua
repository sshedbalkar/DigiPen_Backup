function MaterialMain( graphics )
	debug = {};
	local debugobjframe = {};
	local debugobjsolid = {};
	local NewDebugObj = function( id )
		local obj = {};
		obj.id = id;
		obj.show = false;
		obj.x = 0; obj.y = 0; obj.sx = 0; obj.sy = 0;
		obj.cr = 0; obj.cg = 0; obj.cb = 0; obj.ca = 0;
		return obj;
	end
	debug.NewBoxFrame = function()
		local boxframe = NewDebugObj( render_flags.DEBUG_SHAPE_BOX );
		table.insert( debugobjframe, boxframe );
		return boxframe;
	end
	debug.NewBox = function()
		local box = NewDebugObj( render_flags.DEBUG_SHAPE_BOX );
		table.insert( debugobjsolid, box );
		return box;
	end
	debug.NewArrow = function()
		local arrow = NewDebugObj( render_flags.DEBUG_SHAPE_ARROW );
		table.insert( debugobjsolid, arrow );
		return arrow;
	end
	---------------------------------------------------------------------------------------- renderer
	local renderersolid = graphics.NewRenderer();
	renderersolid:Effect( effects.debug2dsolid );

	local rendererframe = graphics.NewRenderer();
	rendererframe:Effect( effects.debug2dframe );

	local frame_cb = graphics.NewCB( 64 );
	renderersolid:ConstantBuffer( "GS", frame_cb );
	rendererframe:ConstantBuffer( "GS", frame_cb );

	local vformat_size = 11 * 4;
	local vbuffer = graphics.NewVB( vformat_size * 20, 1, 0 );
	renderersolid:VertexBuffer( vbuffer, vformat_size );
	rendererframe:VertexBuffer( vbuffer, vformat_size );
	---------------------------------------------------------------------------------------- variables
	local data = pointer.Chunk();

	while true do
		local matrix, matrix2d = coroutine.yield();
		graphics.MapBuffer( data, frame_cb );
		data:AppendArray( matrix2d, 16 );
		graphics.UnmapBuffer();

		------------------------------------------------------------------------------------ drawing solid objects
		graphics.MapBuffer( data, vbuffer );
		vcount = 0;
		for i,object in pairs( debugobjsolid ) do
			if ( object.show == true ) then
				data:SetF( object.x, object.y, 0, 
						   object.sx, object.sy, 0, 
						   object.cr, object.cg, object.cb, object.ca
						   );
				data:Offset( data, vformat_size - 4 );
				data:SetI( object.id );
				data:Offset( data, 4 );
				vcount = vcount + 1;
			end
		end
		graphics.UnmapBuffer();
		graphics.SetRasterState( gu.raster_states.CullNone );
		graphics.SetBlendState( gu.blend_states.BlendAlpha );
		renderersolid:Draw( vcount );
		------------------------------------------------------------------------------------ drawing frame objects
		graphics.MapBuffer( data, vbuffer );
		vcount = 0;
		for i,object in pairs( debugobjframe ) do
			if ( object.show == true ) then
				data:SetF( object.x, object.y, 0, 
						   object.sx, object.sy, 0, 
						   object.cr, object.cg, object.cb, object.ca
						   );
				data:Offset( data, vformat_size - 4 );
				data:SetI( object.id );
				data:Offset( data, 4 );
				vcount = vcount + 1;
			end
		end
		graphics.UnmapBuffer();
		rendererframe:Draw( vcount );
		graphics.SetBlendState( gu.blend_states.BlendNone );
		graphics.SetRasterState( gu.raster_states.CullBack );
	end
end