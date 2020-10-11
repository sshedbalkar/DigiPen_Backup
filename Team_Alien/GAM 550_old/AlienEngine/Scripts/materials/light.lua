function MaterialMain( graphics )
	-------------------------------- material -------------------------------
	local mat_dir = graphics.NewMaterial( "lightdirectional" );
	mat_dir:SetSerializer("direction(f3)");
	local mat_pt = graphics.NewMaterial( "lightpoint" );
	local mat_spot = graphics.NewMaterial( "lightspot" );
	-------------------------------- cbuffers -------------------------------
	local cbmain = graphics.NewCB( 8 * 4 );
	local cbdata = pointer.Chunk();
	-------------------------------- pointers -------------------------------
	local go = pointer.GfxObj();
	local data0 = pointer.Chunk();
	local shadow_camera = pointer.Camera();
	local eye = array.New(3);
	local matrix_shadow = array.New(16);
	-------------------------------- functions ------------------------------
	---------- build cbuffer for directional light -----------
	local LightDirUpdate = function()
		mat_dir:LocateGfxObjData( data0, go, "direction" );
		local dx,dy,dz = data0:GetF(3);
		cbdata:AppendChunk( data0, 3 * 4 );
	end
	----- build shadow matrix for directional light ----------
	local LightDirShadow = function()
		go:Camera( shadow_camera );
		shadow_camera:ProjectionOrtho( 16, 16, 0.1, 50 );
		--shadow_camera:Rotate( blackboard.dt * 0.3, 0, 0 );
		shadow_camera:Matrices( matrix_shadow );
	end
	
	while true do
		coroutine.yield( cbmain );
		mat_dir:ForEachObject( go, LightDirShadow );
		local camera = coroutine.yield( matrix_shadow );
		eye:Set( camera:Eye() );
		graphics.MapBuffer( cbdata, cbmain );
		cbdata:AppendArray( eye, 3 );
		cbdata:Offset( cbdata, 4 );
		mat_dir:ForEachObject( go, LightDirUpdate );
		graphics.UnmapBuffer();

		--graphics.PSSetCB( cbmain, 1 );
	end
end
